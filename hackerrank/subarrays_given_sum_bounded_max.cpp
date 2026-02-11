
#include <bits/stdc++.h>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

/*
 * Complete the 'countSubarraysWithSumAndMaxAtMost' function below.
 *
 * The function is expected to return a LONG_INTEGER.
 * The function accepts following parameters:
 *  1. INTEGER_ARRAY nums
 *  2. LONG_INTEGER k
 *  3. LONG_INTEGER M
 */

using usize = std::size_t;
using ValueT = int;

auto count_naive(vector<ValueT> &nums, long long k, long long M) -> long long {
    long long out{0};
    for (usize i{0}; i < nums.size(); ++i) {
        if (nums[i] > M) {
            continue;
        }
        long long running_sum{0l};
        for (usize j{i}; j < nums.size(); ++j) {
            if (static_cast<long long>(nums[j]) > M) {
                break;
            }
            running_sum += nums[j];
            if (running_sum == static_cast<long long>(k)) {
                ++out;
            }
        }
    }
    return out;
}

auto countSubarraysWithSumAndMaxAtMost(
    vector<ValueT> nums, // numbers
    long long k,         // sum([a1, ..., ar]) = k
    long long M          // max([a1, ..., ar]) <= M
    ) -> long long {
    if (nums.empty()) {
        return 0l;
    } else if (nums.size() == 1) {
        return (nums[0] == k && nums[0] <= M) ? 1l : 0l;
    }
    long long out{0ll};
    long long pref_sum{0ll};
    std::unordered_map<long long, long long> counts{{0, 1}};

    for (usize i{0}; i < nums.size(); ++i) {
        if (nums[i] > M) {
            pref_sum = 0ll;
            counts.clear();
        }
        pref_sum += nums[i];
        if (auto it = counts.find(pref_sum - k); it != counts.end()) {
            out += it->second;
        }
        ++counts[pref_sum];
    }
    return out;
}

/*
6
2
-1
2
1
-2
3
3
2
*/

int main() {
    string nums_count_temp;
    getline(cin, nums_count_temp);

    int nums_count = stoi(ltrim(rtrim(nums_count_temp)));

    vector<int> nums(nums_count);

    for (int i = 0; i < nums_count; i++) {
        string nums_item_temp;
        getline(cin, nums_item_temp);

        int nums_item = stoi(ltrim(rtrim(nums_item_temp)));

        nums[i] = nums_item;
    }

    string k_temp;
    getline(cin, k_temp);

    long k = stol(ltrim(rtrim(k_temp)));

    string M_temp;
    getline(cin, M_temp);

    long M = stol(ltrim(rtrim(M_temp)));

    long result = countSubarraysWithSumAndMaxAtMost(nums, k, M);

    cout << result << "\n";

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end());

    return s;
}
