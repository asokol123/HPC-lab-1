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

constexpr size_t BATCH_SIZE = 50;
std::vector<std::pair<int, int>> elems;
std::atomic_int best_result = 0;
const int MAX_THREADS_CNT = std::thread::hardware_concurrency();
std::atomic_bool cv_ready = false;
std::atomic_int threads_cnt = 0;
std::queue<Task> global_tasks;
std::mutex global_mx;
std::condition_variable global_cv;

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
    while (true) {
        if (tasks.empty()) {
            ++threads_cnt;
            std::unique_lock lk(global_mx);
            global_cv.wait(lk, []() -> bool { return cv_ready; });
            if (global_tasks.empty()) {
                return;
            }
            std::swap(tasks, global_tasks);
            --threads_cnt;
            continue;
        }
        if (threads_cnt != 0 && tasks.size() >= 2 * BATCH_SIZE) {
            std::unique_lock lk(global_mx, std::try_to_lock);
            if (!lk.owns_lock()) {
                continue;
            }
            std::queue<Task> batch;
            for (size_t i = 0; i < BATCH_SIZE; ++i) {
                batch.push(std::move(tasks.front()));
                tasks.pop();
            }
            std::swap(global_tasks, batch);
            cv_ready = true;
            global_cv.notify_one();
        }
        Task t = tasks.front();
        tasks.pop();
        Solve(t.k, t.s, t.curr_cost, t.remaining_weight, tasks);
    }
}

int main() {
    int n, k;
    std::cin >> n >> k;
    elems.resize(n);

    for (auto&[price, weight] : elems) {
        std::cin >> price >> weight;
    }

    std::vector<std::thread> workers;
    workers.reserve(MAX_THREADS_CNT);
    for (int i = 1; i < MAX_THREADS_CNT; ++i) {
        workers.push_back(std::thread(Worker, std::queue<Task>()));
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::sort(elems.begin(), elems.end(), SortCmp);
    std::queue<Task> tasks;
    for (size_t i = 0; i < elems.size(); ++i) {
        tasks.push(Task{i, i, 0, k});
    }

    workers.push_back(std::thread(Worker, std::move(tasks)));

    while (threads_cnt != MAX_THREADS_CNT || cv_ready) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    {
        std::lock_guard lg(global_mx);
        cv_ready = true;
        global_cv.notify_all();
    }

    for (auto& t : workers) {
        t.join();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << best_result << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
}
