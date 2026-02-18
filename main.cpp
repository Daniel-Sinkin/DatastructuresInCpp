// main.cpp

#include <cassert>    // IWYU pragma: keep
#include <cstddef>    // IWYU pragma: keep
#include <cstdint>    // IWYU pragma: keep
#include <cstdlib>    // IWYU pragma: keep
#include <exception>  // IWYU pragma: keep
#include <functional> // IWYU pragma: keep
#include <iostream>
#include <iterator> // IWYU pragma: keep
#include <limits>   // IWYU pragma: keep
#include <map>      // IWYU pragma: keep
#include <numeric>
#include <print>     // IWYU pragma: keep
#include <random>    // IWYU pragma: keep
#include <stdexcept> // IWYU pragma: keep
#include <stdlib.h>  // IWYU pragma: keep
#include <string>
#include <type_traits> // IWYU pragma: keep
#include <vector>      // IWYU pragma: keep

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using Byte = std::byte;
using usize = std::size_t;
/*
Given Input

timestamp,server_id,request_type,duration_ms,status_code
1000,srv-01,GET,45,200
1003,srv-02,POST,120,500
1005,srv-01,GET,30,200
1007,srv-03,POST,200,200
1010,srv-02,GET,15,200
1012,srv-01,POST,500,500
1015,srv-03,GET,25,200
1018,srv-02,POST,80,200

For each unique server, return the total number of requests it received,
    its average duration in milliseconds,
    and the percentage of its requests that had a non-200 status code.
    Return the results sorted alphabetically by server id.

For each server, determine whether it is overloaded.
    A server is overloaded if, when you look at all requests sorted by timestamp,
    that server has 3 or more consecutive requests with a duration greater than 100ms.
    Return the ids of all overloaded servers.

For all requests sorted by timestamp, find the longest chain where each request's timestamp
    is within 5ms of the one before it. Return the length of that chain
    and the timestamp of its first request.
*/

constexpr char k_seperator{','};

enum class RequestType : u8 {
    UNKNOWN = 0,
    GET,
    POST,
};

auto to_string(RequestType type) -> std::string {
    switch (type) {
    case RequestType::UNKNOWN: {
        return "UNKNOWN";
    }
    case RequestType::GET: {
        return "GET";
    }
    case RequestType::POST: {
        return "POST";
    }
    };
}

struct Request {                // 37 byte
    std::string server_id{};    // 24 byte
    u32 timestamp{};            // 4 byte
    u32 duration_ms{};          // 4 byte
    u32 status_code{};          // 4 byte
    RequestType request_type{}; // 1 byte
};

struct RequestInner {           // 13 byte
    u32 timestamp{};            // 4 byte
    u32 duration_ms{};          // 4 byte
    u32 status_code{};          // 4 byte
    RequestType request_type{}; // 1 byte
};

auto to_string(const Request &request) -> std::string {
    std::string out;
    out += std::to_string(request.timestamp);
    out.push_back(k_seperator);
    out += request.server_id;
    out.push_back(k_seperator);
    out += to_string(request.request_type);
    out.push_back(k_seperator);
    out += std::to_string(request.duration_ms);
    out.push_back(k_seperator);
    out += std::to_string(request.status_code);
    out.push_back('\n');
    return out;
}

auto scan(std::string_view line, usize &idx) -> void {
    while (idx < line.size() && line[idx] != k_seperator) {
        ++idx;
    }
}

auto parse_u32(std::string_view line, usize &idx) -> u32 {
    const auto start_idx = idx;
    scan(line, idx);

    auto tmp = std::string{line.substr(start_idx, idx - start_idx)};
    const auto out = std::stoi(tmp);
    return static_cast<u32>(out);
}

auto parse_str(std::string_view line, usize &idx) -> std::string {
    const auto start_idx = idx;
    scan(line, idx);
    return std::string{line.substr(start_idx, idx - start_idx)};
}

auto parse_request_type(std::string_view line, usize &idx) -> RequestType {
    const auto parsed = parse_str(line, idx);
    if (parsed == "GET") {
        return RequestType::GET;
    } else if (parsed == "POST") {
        return RequestType::POST;
    } else {
        return RequestType::UNKNOWN;
    }
};

auto parse_input() -> std::vector<Request> {
    std::vector<Request> requests;
    requests.reserve(128); // 128 * 37 = 4636 byte ~ 4.5KB
    auto skip_first = true;
    for (;;) {
        std::string line;
        std::getline(std::cin, line);
        if (skip_first) {
            skip_first = false;
            continue;
        }
        if (line.empty()) {
            break;
        }

        // timestamp,server_id,request_type,duration_ms,status_code
        // u32      ,str      ,RequestType ,u32        ,u32
        auto idx = 0zu;
        // std::cout << line << "\n";
        const auto timestamp = parse_u32(line, idx);
        ++idx;
        const auto server_id = parse_str(line, idx);
        ++idx;
        const auto request_type = parse_request_type(line, idx);
        ++idx;
        const auto duration_ms = parse_u32(line, idx);
        ++idx;
        const auto status_code = parse_u32(line, idx);
        ++idx;
        requests.emplace_back(server_id, timestamp, duration_ms, status_code, request_type);
    }
    return requests;
}

int main() {
    const auto requests = parse_input();

    std::map<std::string, std::vector<RequestInner>> requests_by_server{};
    for (const auto &request : requests) {
        requests_by_server[request.server_id].emplace_back(
            request.timestamp, request.duration_ms, request.status_code, request.request_type);
    }

    // Problem 1
    std::cout << "Problem 1\n";
    for (const auto &[k, v] : requests_by_server) {
        auto total_dur = 0;
        auto count_non_200 = 0;
        for (const auto &req : v) {
            total_dur += req.duration_ms;
            if (req.status_code != 200) {
                ++count_non_200;
            }
        }
        const auto avg_dur = static_cast<double>(total_dur) / static_cast<double>(v.size());
        const auto pct_non_200 = static_cast<double>(count_non_200) / static_cast<double>(v.size());
        std::cout << k << ","
                  << v.size() << " Requests" << ","
                  << avg_dur << "ms Average Duration, "
                  << "Percentage of non-200: " << pct_non_200 * 100 << "%\n";
    }
    std::cout << "\n";

    // Problem 2
    std::cout << "Problem 2\n";
    for (const auto &[k, v] : requests_by_server) {
        auto current_overloaded_counter = 0;
        auto is_overloaded = false;
        for (const auto &req : v) {
            if (req.duration_ms > 100) {
                ++current_overloaded_counter;
                if (current_overloaded_counter >= 3) {
                    is_overloaded = true;
                    break;
                }
            } else {
                current_overloaded_counter = 0;
            }
        }
        if (is_overloaded) {
            std::cout << k << " is overloaded!\n";
        } else {
            std::cout << k << " is not overloaded.\n";
        }
    }
    std::cout << "\n";

    /*
For all requests sorted by timestamp, find the longest chain where each request's timestamp
    is within 5ms of the one before it. Return the length of that chain
    and the timestamp of its first request.
    */
    // Problem 3
    std::cout << "Problem 3\n";
    auto best_chain = 0;
    auto best_chain_idx = 0zu;
    auto current_chain = 0;
    auto current_chain_idx = 0zu;
    for (auto i = 1zu; i < requests.size(); ++i) {
        const auto curr = requests[i];
        const auto prev = requests[i - 1];

        if (curr.timestamp - prev.timestamp <= 5) {
            ++current_chain;
        } else {
            if (current_chain > best_chain) {
                best_chain = current_chain;
                best_chain_idx = current_chain_idx;
            }
            current_chain = 0;
            current_chain_idx = i;
        }
    }
    if (current_chain > best_chain) {
        best_chain = current_chain;
        best_chain_idx = current_chain_idx;
    }
    if (best_chain > 0) {
        std::cout << "The best chain was at timestamp " << requests[best_chain_idx].timestamp << " and had length " << best_chain + 1 << "\n";
    } else {
        std::cout << "There is no chain\n";
    }
}
