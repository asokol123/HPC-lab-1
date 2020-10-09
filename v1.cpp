#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
#include <queue>

struct Task {
    size_t k;
    size_t s;
    int curr_cost;
    int remaining_weight;
};

std::vector<std::pair<int, int>> elems;
std::atomic_int best_result = 0;
std::atomic_int threads_cnt;

bool SortCmp(const std::pair<int, int>& left, const std::pair<int, int>& right) {
    return left.first * right.second > right.first * left.second;
}

void UpdateMaximum(std::atomic_int& maximum_value, int value) noexcept
{
    int prev_value = maximum_value;
    while(prev_value < value &&
            !maximum_value.compare_exchange_weak(prev_value, value))
        {}
}

void Solve(size_t k, size_t s, int curr_cost, int remaining_weight, std::queue<Task>& tasks) {
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
        tasks.push(Task{i, s, curr_cost, remaining_weight});
        curr_cost -= elems[i].first;
        remaining_weight += elems[i].second;
    }
    while (s < elems.size()) {
        tasks.push(Task{s, s, curr_cost, remaining_weight});
        ++s;
    }
}

void Worker(std::queue<Task> tasks) {
    std::vector<std::thread> futures;
    while (!tasks.empty()) {
        if (threads_cnt > 0 && tasks.size() > 10) {
            --threads_cnt;

            std::queue<Task> delegated_tasks;
            size_t sz = tasks.size();
            for (size_t i = 0; 2 * i < sz; ++i) {
                delegated_tasks.push(std::move(tasks.front()));
                tasks.pop();
            }

            futures.push_back(std::thread(Worker, std::move(delegated_tasks)));
            continue;
        }
        Task t = tasks.front();
        tasks.pop();
        Solve(t.k, t.s, t.curr_cost, t.remaining_weight, tasks);
    }
    ++threads_cnt;
    for (auto& x : futures) {
        x.join();
    }
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <threads count>" << std::endl;
        return 1;
    }
    threads_cnt = std::atoi(argv[1]);

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
        tasks.push(Task{i, i, 0, k});
    }

    --threads_cnt;
    Worker(std::move(tasks));

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << best_result << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
}
