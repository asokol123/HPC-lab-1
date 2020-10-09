#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <queue>

struct Task {
    size_t k;
    size_t s;
    int curr_cost;
    int remaining_weight;
};

std::vector<std::pair<int, int>> elems;
int best_result = 0;

bool SortCmp(const std::pair<int, int>& left, const std::pair<int, int>& right) {
    return left.first * right.second > right.first * left.second;
}

void UpdateMaximum(int& maximum_value, int value) {
    if (maximum_value < value) {
        maximum_value = value;
    }
}

void Solve(size_t k, size_t s, int curr_cost, int remaining_weight) {
    while (s < elems.size() && elems[s].second <= remaining_weight) {
        curr_cost += elems[s].first;
        remaining_weight -= elems[s].second;
        ++s;
    }
    if (s <= k) {
        return;
    }

    int min_result = curr_cost;
    UpdateMaximum(best_result, min_result);
    int max_result = min_result;
    if (s < elems.size()) {
        max_result += elems[s].first * remaining_weight / elems[s].second;
    }
    if (max_result <= best_result) {
        return;
    }

    for (size_t i = k + 1; i < s; ++i) {
        Solve(i, s, curr_cost, remaining_weight);
        curr_cost -= elems[i].first;
        remaining_weight += elems[i].second;
    }
    while (s < elems.size()) {
        Solve(s, s, curr_cost, remaining_weight);
        ++s;
    }
}

int main() {
    int n, k;
    std::cin >> n >> k;
    elems.resize(n);

    for (auto&[price, weight] : elems) {
        std::cin >> price >> weight;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::sort(elems.begin(), elems.end(), SortCmp);
    std::queue<Task> tasks;
    for (size_t i = 0; i < elems.size(); ++i) {
        Solve(i, i, 0, k);
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << best_result << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
}
