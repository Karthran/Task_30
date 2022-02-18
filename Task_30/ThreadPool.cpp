#include "ThreadPool.h"

ThreadPool::ThreadPool() : m_thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4), m_thread_queues(m_thread_count)
{
}

void ThreadPool::start()
{
    for (int i = 0; i < m_thread_count; i++)
    {
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

void ThreadPool::threadFunc(int qindex)
{
    while (true)
    {
        // ��������� ��������� ������
        TaskWithPromise twp;

        bool res{false};
        int i = 0;
        for (; i < m_thread_count; i++)
        {
            // ������� ������ ������� ������ �� ����� �������, ������� �� �����
            if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(twp)) break;
        }
        if (!res)
        {
            // �������� ����������� ��������� �������
            m_thread_queues[qindex].pop(twp);
        }
        else if (!twp.task)
        {
            // ����� �� ��������� ��������� ������
            // ������ ������� ������-��������
            m_thread_queues[(qindex + i) % m_thread_count].push(twp);
        }
        if (!twp.task)
        {
            return;
        }
        // ��������� ������
        twp.task();
        //twp.prom.set_value();
    }
}

//auto ThreadPool::runTask() -> void
//{
//    task_type task_to_do;
//    bool res{false};
//    int i = 0;
//    TaskWithPromise twp;
//
//    for (; i < m_thread_count; i++)
//    {
//        // ������� ������ ������� ������ �� ����� �������, ������� �� �����
//        res = m_thread_queues[i % m_thread_count].fast_pop(twp);
//        if (res) break;
//    }
//
//    if (!res)
//    {
//        return;
//    }
//    else if (!twp.task)
//    {
//        // ����� �� ��������� ��������� ������
//        // ������ ������� ������-��������
//        task_type empty_task;
//        twp.task = empty_task;
//
//        m_thread_queues[i % m_thread_count].push(twp);
//    }
//    if (!twp.task)
//    {
//        return;
//    }
//    // ��������� ������
//    twp.task();
//}

void ThreadPool::stop()
{
    for (int i = 0; i < m_thread_count; i++)
    {
        // ������ ������-�������� � ������ �������
        // ��� ���������� ������
        TaskWithPromise twp;
        m_thread_queues[i].push(twp);
    }
    for (auto& t : m_threads)
    {
        t.join();
    }
}
auto ThreadPool::push_task(FuncType f, std::vector<int>& vec, int id, int arg) -> res_type
{
    // ��������� ������ �������, ���� ������� ������
    int queue_to_push = m_index++ % m_thread_count;
    // ��������� �������
    task_type task = [=,&vec] { f(vec,id, arg); };
    TaskWithPromise twp;
    twp.task = task;
    // ������ � �������
    res_type res = twp.prom.get_future();
    m_thread_queues[queue_to_push].push(twp);
    return res;
}
