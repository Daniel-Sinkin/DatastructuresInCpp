// cache_access_speed.cpp
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <numeric>
#include <print>
#include <random>
#include <string>
#include <utility>
#include <vector>

using u64 = std::uint64_t;
using u32 = std::uint32_t;

static inline void escape(u64 x)
{
#if defined(__clang__) || defined(__GNUC__)
    asm volatile("" : : "r"(x) : "memory");
#else
    (void)x;
#endif
}

static std::string format_bytes(u64 bytes)
{
    constexpr u64 KiB = 1024ULL;
    constexpr u64 MiB = 1024ULL * KiB;
    constexpr u64 GiB = 1024ULL * MiB;

    const auto as_d = [](u64 x) -> double { return static_cast<double>(x); };

    if (bytes >= GiB) return std::format("{:.2f} GiB", as_d(bytes) / as_d(GiB));
    if (bytes >= MiB) return std::format("{:.2f} MiB", as_d(bytes) / as_d(MiB));
    if (bytes >= KiB) return std::format("{:.2f} KiB", as_d(bytes) / as_d(KiB));
    return std::format("{} B", bytes);
}

static double median(std::vector<double> xs)
{
    std::sort(xs.begin(), xs.end());
    const std::size_t n = xs.size();
    if (n == 0) return 0.0;
    if (n & 1U) return xs[n / 2];
    return 0.5 * (xs[n / 2 - 1] + xs[n / 2]);
}

// Store/load u32 without alignment UB.
static inline void store_u32(std::byte* p, u32 v) { std::memcpy(p, &v, sizeof(v)); }
static inline u32 load_u32(const std::byte* p)
{
    u32 v = 0;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

static void touch_pages(std::vector<std::byte>& buf, std::size_t page_size)
{
    volatile std::uint8_t sink = 0;
    for (std::size_t i = 0; i < buf.size(); i += page_size)
        sink ^= static_cast<std::uint8_t>(buf[i]);
    escape(static_cast<u64>(sink));
}

static void thrash_cache(std::vector<std::byte>& buf)
{
    volatile std::uint8_t sink = 0;
    for (std::size_t i = 0; i < buf.size(); i += 64)
        sink ^= static_cast<std::uint8_t>(buf[i]);
    escape(static_cast<u64>(sink));
}

static u64 align_down(u64 x, u64 stride) { return (x / stride) * stride; }

struct RunConfig
{
    u64 min_ws = 4ULL * 1024;
    u64 max_ws = 128ULL * 1024 * 1024;

    // Default: 128B (often a line). For "variations" you can also try 16/64 or 16KiB.
    u64 stride_bytes = 128;

    // Recommended lower defaults for granular sweeps:
    std::size_t iters = 3'000'000;
    std::size_t warmup = 30'000;

    int trials_warm = 9;
    int trials_cold = 9;

    bool skip_first = true;
    bool do_cold = true;

    u64 thrash_bytes = 256ULL * 1024 * 1024;

    // Apple Silicon typically uses 16 KiB pages.
    std::size_t page_size = 16ULL * 1024;

    // Jump detection thresholds (ratio vs previous point).
    double jump_ratio = 1.40;
    double big_jump_ratio = 2.00;

    // After detecting a jump centered at size S, add points:
    // ws = S*(k/extra_den) for k in [extra_num_lo..extra_num_hi]
    // With den=16 and k=12..20 => 0.75S .. 1.25S in 1/16 steps.
    int extra_den = 16;
    int extra_num_lo = 12;
    int extra_num_hi = 20;
};

static void touch_prefix_pages(std::vector<std::byte>& buf, u64 ws_bytes, std::size_t page_size)
{
    volatile std::uint8_t sink = 0;
    for (u64 off = 0; off < ws_bytes; off += static_cast<u64>(page_size))
        sink ^= static_cast<std::uint8_t>(buf[static_cast<std::size_t>(off)]);
    escape(static_cast<u64>(sink));
}

static void build_chain_prefix(std::vector<std::byte>& buf,
                               u64 ws_bytes,
                               u64 stride,
                               std::mt19937_64& rng)
{
    const std::size_t n_nodes = static_cast<std::size_t>(ws_bytes / stride);
    if (n_nodes < 2) return;

    std::vector<u32> perm(n_nodes);
    std::iota(perm.begin(), perm.end(), 0U);
    std::shuffle(perm.begin(), perm.end(), rng);

    for (std::size_t i = 0; i + 1 < n_nodes; ++i) {
        const u32 from = perm[i];
        const u32 to = perm[i + 1];
        store_u32(buf.data() + static_cast<std::size_t>(from) * stride, to);
    }
    store_u32(buf.data() + static_cast<std::size_t>(perm.back()) * stride, perm[0]);
}

static double time_chase_ns_per_access(const std::vector<std::byte>& buf,
                                       u64 ws_bytes,
                                       u64 stride,
                                       std::size_t iters,
                                       std::size_t warmup)
{
    const std::byte* base = buf.data();
    const std::size_t n_nodes = static_cast<std::size_t>(ws_bytes / stride);

    u32 idx = 0;

    for (std::size_t i = 0; i < warmup; ++i) {
        const std::byte* p = base + static_cast<std::size_t>(idx) * stride;
        idx = load_u32(p);
    }

    const auto t0 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < iters; ++i) {
        const std::byte* p = base + static_cast<std::size_t>(idx) * stride;
        idx = load_u32(p);
    }
    const auto t1 = std::chrono::steady_clock::now();

    escape(static_cast<u64>(idx) + static_cast<u64>(n_nodes));

    const double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
    return ns / static_cast<double>(iters);
}

static double run_median(const std::vector<std::byte>& buf,
                         u64 ws_bytes,
                         u64 stride,
                         std::size_t iters,
                         std::size_t warmup,
                         int trials,
                         bool skip_first,
                         bool cold,
                         std::vector<std::byte>& thrash_buf)
{
    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(trials));

    for (int t = 0; t < trials; ++t) {
        if (cold) thrash_cache(thrash_buf);

        const double ns = time_chase_ns_per_access(buf, ws_bytes, stride, iters, warmup);

        if (skip_first && t == 0) continue;
        samples.push_back(ns);
    }

    return median(samples);
}

struct Point
{
    u64 ws = 0;
    double warm = 0.0;
    double cold = 0.0;
};

static std::vector<u64> make_pow2_sizes(u64 min_ws, u64 max_ws, u64 stride)
{
    std::vector<u64> sizes;
    for (u64 ws = min_ws; ws <= max_ws; ws *= 2) {
        const u64 aligned = align_down(ws, stride);
        if (aligned >= 2 * stride && (sizes.empty() || aligned != sizes.back()))
            sizes.push_back(aligned);
    }
    return sizes;
}

static void add_granular_around(std::vector<u64>& sizes,
                                u64 center,
                                u64 min_ws,
                                u64 max_ws,
                                u64 stride,
                                int den,
                                int k_lo,
                                int k_hi)
{
    // Add ws = center * (k/den) aligned down to stride.
    // Do multiplication in a way that avoids overflow and keeps it integer.
    // center*(k/den) = (center/den)*k for den dividing is OK for our coarse sizes.
    const u64 base = center / static_cast<u64>(den);
    for (int k = k_lo; k <= k_hi; ++k) {
        const u64 ws = align_down(base * static_cast<u64>(k), stride);
        if (ws >= min_ws && ws <= max_ws) sizes.push_back(ws);
    }
}

int main()
{
    RunConfig cfg;

    const u64 stride = std::max<u64>(cfg.stride_bytes, static_cast<u64>(sizeof(u32)));
    const u64 min_ws = std::max<u64>(cfg.min_ws, 2 * stride);
    const u64 max_ws = align_down(cfg.max_ws, stride);

    std::vector<std::byte> buf(static_cast<std::size_t>(max_ws));
    std::vector<std::byte> thrash_buf(static_cast<std::size_t>(cfg.thrash_bytes));

    // Fault-in pages once to reduce paging/compression noise.
    touch_pages(buf, cfg.page_size);
    touch_pages(thrash_buf, cfg.page_size);

    std::mt19937_64 rng(123456789);

    std::println("Settings:");
    std::println("  working set: {} .. {}", format_bytes(min_ws), format_bytes(max_ws));
    std::println("  stride: {}", format_bytes(stride));
    std::println("  iters/trial: {}", cfg.iters);
    std::println("  warmup/trial: {}", cfg.warmup);
    std::println("  trials warm/cold: {}/{}", cfg.trials_warm, cfg.trials_cold);
    std::println("  skip first trial: {}", cfg.skip_first ? "yes" : "no");
    std::println("  thrash: {}", format_bytes(cfg.thrash_bytes));
    std::println("  page size: {}", format_bytes(static_cast<u64>(cfg.page_size)));
    std::println("  jump ratios: {:.2f} / {:.2f}", cfg.jump_ratio, cfg.big_jump_ratio);
    std::println("  granular window: [{:.2f}S .. {:.2f}S] step 1/{}",
                 static_cast<double>(cfg.extra_num_lo) / static_cast<double>(cfg.extra_den),
                 static_cast<double>(cfg.extra_num_hi) / static_cast<double>(cfg.extra_den),
                 cfg.extra_den);
    std::println("");

    // Phase 1: coarse sweep (power-of-two) to detect jump centers.
    const std::vector<u64> coarse_sizes = make_pow2_sizes(min_ws, max_ws, stride);

    std::vector<Point> coarse_results;
    coarse_results.reserve(coarse_sizes.size());

    for (u64 ws : coarse_sizes) {
        build_chain_prefix(buf, ws, stride, rng);
        touch_prefix_pages(buf, ws, cfg.page_size);

        const double warm = run_median(buf, ws, stride, cfg.iters, cfg.warmup,
                                       cfg.trials_warm, cfg.skip_first, false, thrash_buf);
        const double cold = cfg.do_cold
                                ? run_median(buf, ws, stride, cfg.iters, cfg.warmup,
                                             cfg.trials_cold, cfg.skip_first, true, thrash_buf)
                                : 0.0;

        coarse_results.push_back(Point{ws, warm, cold});
    }

    // Determine jump centers based on selected series (cold if enabled else warm).
    std::vector<u64> jump_centers;
    for (std::size_t i = 1; i < coarse_results.size(); ++i) {
        const double prev = cfg.do_cold ? coarse_results[i - 1].cold : coarse_results[i - 1].warm;
        const double curr = cfg.do_cold ? coarse_results[i].cold : coarse_results[i].warm;
        if (prev <= 0.0) continue;
        if ((curr / prev) >= cfg.jump_ratio) jump_centers.push_back(coarse_results[i].ws);
    }
    std::sort(jump_centers.begin(), jump_centers.end());
    jump_centers.erase(std::unique(jump_centers.begin(), jump_centers.end()), jump_centers.end());

    // Build final size set: coarse + granular around jumps.
    std::vector<u64> all_sizes = coarse_sizes;
    for (u64 center : jump_centers) {
        add_granular_around(all_sizes, center, min_ws, max_ws, stride,
                            cfg.extra_den, cfg.extra_num_lo, cfg.extra_num_hi);
    }
    std::sort(all_sizes.begin(), all_sizes.end());
    all_sizes.erase(std::unique(all_sizes.begin(), all_sizes.end()), all_sizes.end());

    // Phase 2: measure ONCE for all sizes, store results.
    std::vector<std::pair<u64, Point>> results;
    results.reserve(all_sizes.size());

    for (u64 ws : all_sizes) {
        build_chain_prefix(buf, ws, stride, rng);
        touch_prefix_pages(buf, ws, cfg.page_size);

        const double warm = run_median(buf, ws, stride, cfg.iters, cfg.warmup,
                                       cfg.trials_warm, cfg.skip_first, false, thrash_buf);
        const double cold = cfg.do_cold
                                ? run_median(buf, ws, stride, cfg.iters, cfg.warmup,
                                             cfg.trials_cold, cfg.skip_first, true, thrash_buf)
                                : 0.0;

        results.emplace_back(ws, Point{ws, warm, cold});
    }

    // Render: table from stored results.
    std::println("{:>12} | {:>10} | {:>10} | {:>8}", "WorkingSet", "warm(ns)", "cold(ns)", "note");
    std::println("{:-<12}-+-{:-<10}-+-{:-<10}-+-{:-<8}", "", "", "", "");

    double prev_series = 0.0;

    for (const auto& [ws, p] : results) {
        const double series = cfg.do_cold ? p.cold : p.warm;

        const char* note = "";
        if (prev_series > 0.0) {
            const double ratio = series / prev_series;
            if (ratio >= cfg.big_jump_ratio) note = "BIG JUMP";
            else if (ratio >= cfg.jump_ratio) note = "jump";
        }

        if (cfg.do_cold) {
            std::println("{:>12} | {:>10.3f} | {:>10.3f} | {:>8}",
                         format_bytes(ws), p.warm, p.cold, note);
        } else {
            std::println("{:>12} | {:>10.3f} | {:>10} | {:>8}",
                         format_bytes(ws), p.warm, "-", note);
        }

        prev_series = series;
    }

    // Render: CSV from the same stored results.
    std::println("");
    std::println("CSV (bytes,warm_ns,cold_ns):");
    for (const auto& [ws, p] : results) {
        std::println("{},{:.6f},{:.6f}", ws, p.warm, p.cold);
    }

    return 0;
}