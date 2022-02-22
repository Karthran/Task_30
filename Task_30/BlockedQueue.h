#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

template <class T>
class BlockedQueue
{
public:
    auto push(T& item) -> void
    {
        std::lock_guard<std::mutex> l(m_locker);
        // обычный потокобезопасный push
        m_task_queue.push(std::move(item));
        // делаем оповещение, чтобы поток, вызвавший
        // pop проснулся и забрал элемент из очереди
        m_notifier.notify_one();
    }
    // блокирующий метод получения элемента из очереди
    auto pop(T& item) -> void
    {
        std::unique_lock<std::mutex> l(m_locker);
        if (m_task_queue.empty())
            // ждем, пока вызовут push
            m_notifier.wait(l, [this] { return !m_task_queue.empty(); });
        item = std::move(m_task_queue.front());
        m_task_queue.pop();
    }
    // неблокирующий метод получения элемента из очереди
    // возвращает false, если очередь пуста
    auto fast_pop(T& item) -> bool
    {
        std::lock_guard<std::mutex> l(m_locker);
        if (m_task_queue.empty())
            // просто выходим
            return false;
        // забираем элемент
        item = std::move(m_task_queue.front());
        m_task_queue.pop();
        return true;
    }

private:
    std::mutex m_locker;
    // очередь задач
    std::queue<T> m_task_queue;
    // уведомитель
    std::condition_variable m_notifier;
};