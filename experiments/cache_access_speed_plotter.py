import matplotlib.pyplot as plt
import pandas as pd


def main() -> None:
    df = (
        pd.read_csv("./cache_access_speed.csv")
        .sort_values("bytes")
        .reset_index(drop=True)
    )

    x_kib = df["bytes"] / 1024.0
    warm_ns = df["warm_ns"].astype(float)
    cold_ns = df["cold_ns"].astype(float)

    warm_eff = x_kib / warm_ns
    cold_eff = x_kib / cold_ns

    fig, (ax_top, ax_bot) = plt.subplots(
        nrows=2, ncols=1, figsize=(12, 10), sharex=True
    )

    ax_top.loglog(x_kib, warm_ns, label="Warm cache")
    ax_top.loglog(x_kib, cold_ns, label="Cold cache")
    ax_top.set_ylabel("Latency per access [ns]")
    ax_top.set_title("Cache / TLB Access Latency vs Working Set Size (log–log scale)")
    ax_top.grid(True, which="both", ls="--", alpha=0.4)

    ax_bot.loglog(x_kib, warm_eff, label="Warm cache efficiency")
    ax_bot.loglog(x_kib, cold_eff, label="Cold cache efficiency")
    ax_bot.set_xlabel("Working set size [KiB]")
    ax_bot.set_ylabel("Access efficiency [KiB / ns]")
    ax_bot.set_title("Access Efficiency vs Working Set Size (log–log scale)")
    ax_bot.grid(True, which="both", ls="--", alpha=0.4)

    L1_L2_KIB = 192
    TLB_KIB = 12 * 1024

    for ax in (ax_top, ax_bot):
        ax.axvline(
            x=L1_L2_KIB,
            ls="--",
            alpha=0.8,
            label="L1 → L2 boundary (~192 KiB)",
        )
        ax.axvline(
            x=TLB_KIB,
            ls="--",
            alpha=0.8,
            label="TLB / page-walk limit (~12 MiB)",
        )

    ax_top.legend()
    ax_bot.legend()

    plt.tight_layout()
    plt.savefig("cache_access_plot.png", dpi=300)
    plt.show()


if __name__ == "__main__":
    main()
