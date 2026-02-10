
#include <bits/stdc++.h>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

/*
 * Complete the 'mergeHighDefinitionIntervals' function below.
 *
 * The function is expected to return a 2D_INTEGER_ARRAY.
 * The function accepts 2D_INTEGER_ARRAY intervals as parameter.
 */

using usize = std::size_t;
using Interval = vector<int>;

constexpr bool k_do_print{false};
constexpr bool k_do_assert{false};

[[nodiscard]] auto le_interval(const Interval &a, const Interval &b) -> bool {
    return (a[0] < b[0]) || (a[0] == b[0] && a[1] < b[1]);
}

[[nodiscard]] auto has_overlap(const Interval &a, const Interval &b) -> bool {
    // This assumes that a <= b
    if constexpr (k_do_assert) {
        assert(le_interval(a, b));
    }
    return b[0] <= a[1];
}

[[nodiscard]] auto to_string(const Interval &a) -> std::string {
    assert(a.size() == 2);
    std::string out;
    out.reserve(22);
    out.push_back('[');
    out += std::to_string(a[0]); // At most 9 digits
    out.push_back(',');
    out.push_back(' ');
    out += std::to_string(a[1]); // At most 9 digits
    out.push_back(']');
    return out;
}

auto mergeHighDefinitionIntervals(vector<Interval> intervals) -> vector<Interval> {
    if (intervals.empty()) {
        return {};
    } else if (intervals.size() == 1) {
        return {intervals[0]};
    }
    std::sort(
        intervals.begin(),
        intervals.end(),
        le_interval);
    const auto n = intervals.size();

    std::vector<Interval> out{};
    // Prolly overly careful
    out.reserve(intervals.size());

    usize i{0};
    while (i < n) {
        const auto base_interval = intervals[i];
        Interval new_interval{base_interval[0], base_interval[1]};
        ++i;
        while (i < n) {
            const auto interval = intervals[i];
            if constexpr (k_do_assert) {
                assert(le_interval(base_interval, interval));
            }
            if (has_overlap(new_interval, interval)) {
                new_interval[1] = std::max(new_interval[1], interval[1]);
            } else {
                break;
            }
            ++i;
        }
        out.push_back(new_interval);
    }

    if constexpr (k_do_print) {
        for (usize i{0}; i < n; ++i) {
            const auto interval = intervals[i];
            std::cout << to_string(interval) << "\n";
        }
    }

    return out;
}

int main() {
    string intervals_rows_temp;
    getline(cin, intervals_rows_temp);

    int intervals_rows = stoi(ltrim(rtrim(intervals_rows_temp)));

    string intervals_columns_temp;
    getline(cin, intervals_columns_temp);

    int intervals_columns = stoi(ltrim(rtrim(intervals_columns_temp)));

    vector<vector<int>> intervals(intervals_rows);

    for (int i = 0; i < intervals_rows; i++) {
        intervals[i].resize(intervals_columns);

        string intervals_row_temp_temp;
        getline(cin, intervals_row_temp_temp);

        vector<string> intervals_row_temp = split(rtrim(intervals_row_temp_temp));

        for (int j = 0; j < intervals_columns; j++) {
            int intervals_row_item = stoi(intervals_row_temp[j]);

            intervals[i][j] = intervals_row_item;
        }
    }

    vector<vector<int>> result = mergeHighDefinitionIntervals(intervals);

    for (size_t i = 0; i < result.size(); i++) {
        for (size_t j = 0; j < result[i].size(); j++) {
            cout << result[i][j];

            if (j != result[i].size() - 1) {
                cout << " ";
            }
        }

        if (i != result.size() - 1) {
            cout << "\n";
        }
    }

    cout << "\n";

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

vector<string> split(const string &str) {
    vector<string> tokens;

    string::size_type start = 0;
    string::size_type end = 0;

    while ((end = str.find(" ", start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));

        start = end + 1;
    }

    tokens.push_back(str.substr(start));

    return tokens;
}
