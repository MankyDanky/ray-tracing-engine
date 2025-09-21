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

    // Submit a batch of tasks and wait for their completion
    void SubmitAndWait(const std::vector<RenderTask>& tasks, std::function<void(const RenderTask&)> taskFunction) {
        {
            // Lock the queue mutex before modifying the task queue
            std::unique_lock<std::mutex> lock(queueMutex);

            // Store the task processing function for workers
            this->processTask = taskFunction;

            // Clear any old tasks (shouldn't happen but just in case)
            taskQueue.clear();

            // Add all new tasks to the queue
            taskQueue = tasks;

            // Set the number of active workers we expect
            activeWorkers = tasks.size();
        }

        // Wake up all worker threads
        workAvailable.notify_all();

        // Wait for all tasks to complete
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            workComplete.wait(lock, [this]() {
                return activeWorkers == 0;
            });
        }
    }
private:
    // Worker thread function
    void WorkerThread() {
        while (true) {
            RenderTask task;
            bool hasTask = false;

            // Scope for queue lock
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                
                // Wait until there is work to do or we are stopping
                workAvailable.wait(lock, [this]() {
                    return stop || !taskQueue.empty();
                });

                // Exit if pool is stopping and no tasks are left
                if (stop && taskQueue.empty()) {
                    return;
                }

                // Get the next task from the queue 
                if (!taskQueue.empty()) {
                    task = taskQueue.back();
                    taskQueue.pop_back();
                    hasTask = true;
                }
            }

            // Process the task if we got one
            if (hasTask) {
                processTask(task);

                // Notify that we've completed a task
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