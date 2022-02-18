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
    // ������:
    void start();
    // ���������:
    void stop();
    // ������� �����
    void push_task(FuncType f, int id, int arg);
    // ������� ����� ��� ������
    void threadFunc(int qindex);

private:
    // ���������� �������
    int m_thread_count;
    // ������
    std::vector<std::thread> m_threads;
    // ������� ����� ��� �������
    std::vector<BlockedQueue<task_type>> m_thread_queues;
    // ��� ������������ ������������� �����
    int m_index;
};