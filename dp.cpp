#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

int main() {
    int n, k;
    std::cin >> n >> k;
    std::vector<std::pair<int, int>> elems(n);
    for (auto& [cost, weight] : elems) {
        std::cin >> cost >> weight;
    }
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(k + 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= k; ++j) {
            dp[i + 1][j] = std::max(dp[i][j], dp[i + 1][j]);
            if (j + elems[i].second <= k) {
                dp[i + 1][j + elems[i].second] =
                    std::max(dp[i + 1][j + elems[i].second], dp[i][j] + elems[i].first);
            }
        }
    }
    std::cout << *std::max_element(dp.back().begin(), dp.back().end()) << std::endl;
}
