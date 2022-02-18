#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <future>

#include "BlockedQueue.h"

using task_type = std::function<void()>;
using FuncType = void (*)(std::vector<int>&, int, int);
using res_type = std::future<void>;

struct TaskWithPromise
{
    std::promise<void> prom;
    task_type task{};
};

class ThreadPool
{
public:
    ThreadPool();
    // запуск:
    void start();
    // остановка:
    void stop();
    // проброс задач
    auto push_task(FuncType f, std::vector<int>& vec, int id, int arg) -> res_type;
    // функция входа для потока
    void threadFunc(int qindex);

private:
    // количество потоков
    int m_thread_count;
    // потоки
    std::vector<std::thread> m_threads;
    // очереди задач для потоков
    std::vector<BlockedQueue<TaskWithPromise>> m_thread_queues;
    // для равномерного распределения задач
    int m_index{0};
};