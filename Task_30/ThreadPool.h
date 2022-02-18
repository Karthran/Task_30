#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <future>

#include "BlockedQueue.h"

using task_type = std::function<void()>;
using FuncType = void (*)(int, int);
using res_type = std::future<void>;


class ThreadPool
{
public:
    ThreadPool();
    // запуск:
    void start();
    // остановка:
    void stop();
    // проброс задач
    void push_task(FuncType f, int id, int arg);
    // функция входа для потока
    void threadFunc(int qindex);

private:
    // количество потоков
    int m_thread_count;
    // потоки
    std::vector<std::thread> m_threads;
    // очереди задач для потоков
    std::vector<BlockedQueue<task_type>> m_thread_queues;
    // для равномерного распределения задач
    int m_index;
};