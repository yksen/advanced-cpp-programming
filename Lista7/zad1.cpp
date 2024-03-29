#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <queue>
#include <vector>

namespace cpplab
{
    class ThreadPool
    {
    public:
        ThreadPool(size_t threadCount)
        {
            for (size_t i = 0; i < threadCount; ++i)
                threads.emplace_back([this] {
                    while (waitingForTasks)
                    {
                        std::function<double()> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex);
                            condition.wait(lock, [this] { return stopProcessing || !tasks.empty(); });
                            if (!tasks.empty())
                            {
                                task = std::move(tasks.front());
                                tasks.pop();
                            }
                        }
                        if (task)
                        {
                            double result = task();
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++tasksFinished;
                                sumOfResults += result;
                            }
                        }
                        else
                            waitingForTasks = false;         
                    }
                });
        }

        ~ThreadPool()
        {
            this->stop();
        }

        void add_task(std::function<double()> task)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                tasks.emplace(task);
            }
            condition.notify_one();
        }

        double average()
        {
            return (tasksFinished == 0) ? 0 : sumOfResults / static_cast<double>(tasksFinished);
        }

        void stop()
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                stopProcessing = true;
            }
            condition.notify_all();
            for (std::thread &thread : threads)
                if (thread.joinable())
                    thread.join();
        }

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<double()>> tasks;
        std::mutex mutex;
        std::condition_variable condition;
        size_t tasksFinished{0};
        double sumOfResults{0};
        bool stopProcessing{false};
        bool waitingForTasks{true};
    };
};

int main()
{
    cpplab::ThreadPool pool{4};

    for (size_t i = 0; i <= 1'000'000; ++i)
        pool.add_task([i] { return i; });

    std::cout << "Average: " << pool.average() << std::endl;
    pool.stop();
    std::cout << "Average: " << pool.average() << std::endl;
}