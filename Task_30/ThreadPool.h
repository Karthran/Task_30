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
    // ������:
    void start();
    // ���������:
    void stop();
    // ������� �����
    auto push_task(FuncType f, std::vector<int>& vec, int id, int arg) -> res_type;
    // ������� ����� ��� ������
    void threadFunc(int qindex);

private:
    // ���������� �������
    int m_thread_count;
    // ������
    std::vector<std::thread> m_threads;
    // ������� ����� ��� �������
    std::vector<BlockedQueue<TaskWithPromise>> m_thread_queues;
    // ��� ������������ ������������� �����
    int m_index{0};
};