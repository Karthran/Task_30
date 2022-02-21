#include "ThreadPool.h"

ThreadPool::ThreadPool() : m_thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4), m_thread_queues(m_thread_count)
{
}

auto ThreadPool::start() -> void
{
    for (int i = 0; i < m_thread_count; i++)
    {
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

auto ThreadPool::threadFunc(int qindex) -> void
{
    while (true)
    {
        // обработка очередной задачи
        TaskWithPromise twp;

        bool res{false};
        int i = 0;
        for (; i < m_thread_count; i++)
        {
            // попытка быстро забрать задачу из любой очереди, начиная со своей
            if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(twp)) break;
        }
        if (!res)
        {
            // вызываем блокирующее получение очереди
            m_thread_queues[qindex].pop(twp);
        }
        else if (!twp.task)
        {
            // чтобы не допустить зависания потока
            // кладем обратно задачу-пустышку
            m_thread_queues[(qindex + i) % m_thread_count].push(twp);
        }
        if (!twp.task)
        {
            return;
        }
        // выполняем задачу
        twp.task();
        twp.prom.set_value();
    }
}

auto ThreadPool::runTask() -> void
{
    bool res{false};
    int i = 0;
    TaskWithPromise twp;

    for (; i < m_thread_count; i++)
    {
        // попытка быстро забрать задачу из любой очереди, начиная со своей
        res = m_thread_queues[i % m_thread_count].fast_pop(twp);
        if (res) break;
    }

    if (!res)
    {
        return;
    }
    else if (!twp.task)
    {
        // чтобы не допустить зависания потока
        // кладем обратно задачу-пустышку
        m_thread_queues[i % m_thread_count].push(twp);
    }
    if (!twp.task)
    {
        return;
    }
    // выполняем задачу
    twp.task();
    twp.prom.set_value();
}

auto ThreadPool::stop() -> void
{
    for (int i = 0; i < m_thread_count; i++)
    {
        // кладем задачу-пустышку в каждую очередь
        // для завершения потока
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
    // вычисляем индекс очереди, куда положим задачу
    int queue_to_push = m_index++ % m_thread_count;
    // формируем функтор
    task_type task = [=,&vec] { f(vec,id, arg); };
    TaskWithPromise twp;
    twp.task = task;
    // кладем в очередь
    res_type res = twp.prom.get_future();
    m_thread_queues[queue_to_push].push(twp);
    return res;
}
