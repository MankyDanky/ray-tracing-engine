#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

struct RenderTask {
    int startRow;
    int endRow;
};

class ThreadPool {
public:
    ThreadPool(int numThreads) : stop(false), activeWorkers(0) {\
        // Create the specified number of worker threads
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(&ThreadPool::WorkerThread, this);
        }
    }

    ~ThreadPool() {
        {
            // Lock the queue mutex before changing thread pool state
            std::unique_lock<std::mutex> lock(queueMutex);

            // Signal threads to stop
            stop = true;
        }
        
        // Wake up all threads so they can check the stop flag and exit
        workAvailable.notify_all();

        // Wait for all threads to finish
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void SubmitAndWait(const std::vector<RenderTask>& tasks, std::function<void(const RenderTask&)> taskFunction) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            this->processTask = taskFunction;

            taskQueue.clear();

            taskQueue = tasks;

            activeWorkers = tasks.size();
        }

        workAvailable.notify_all();

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            workComplete.wait(lock, [this]() {
                return activeWorkers == 0;
            });
        }
    }
private:
    void WorkerThread() {
        while (true) {
            RenderTask task;
            bool hasTask = false;

            {
                std::unique_lock<std::mutex> lock(queueMutex);

                workAvailable.wait(lock, [this]() {
                    return stop || !taskQueue.empty();
                });

                if (stop && taskQueue.empty()) {
                    return;
                }

                if (!taskQueue.empty()) {
                    task = taskQueue.back();
                    taskQueue.pop_back();
                    hasTask = true;
                }
            }

            if (hasTask) {
                processTask(task);

                std::unique_lock<std::mutex> lock(queueMutex);
                --activeWorkers;

                if (activeWorkers == 0) {
                    workComplete.notify_one();
                }
            }
        }
    }

    std::vector<std::thread> threads;
    std::vector<RenderTask> taskQueue;
    std::function<void(const RenderTask&)> processTask;
    std::mutex queueMutex;
    std::condition_variable workAvailable;
    std::condition_variable workComplete;

    bool stop;
    std::atomic<int> activeWorkers;
};