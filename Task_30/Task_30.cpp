

#include <iostream>
#include <queue>
#include <future>
#include <condition_variable>
#include <vector>
#include <assert.h>

#include "RequestHandler.h"
//#include "ThreadPool.h"

auto taskFunc(std::vector<int>& vec, int id, int delay) -> void;
std::mutex m_cout_locker;
static bool make_thread = true;

void taskFunc(std::vector<int>& vec, int id, int delay)
{
    std::this_thread::sleep_for(std::chrono::seconds(delay));
    std::unique_lock<std::mutex> lock(m_cout_locker);
    std::cout << "task " << id << " made by thread_id " << std::this_thread::get_id() << std::endl;
}

template <typename T>
auto quickSort(T* arr, int start, int stop) -> void
{
    if (start >= stop) return;

    auto mid = (start + stop) / 2;
    //if (arr[start] > arr[mid]) std::swap(arr[start], arr[mid]);
    //if (arr[start] > arr[stop]) std::swap(arr[start], arr[stop]);
    //if (arr[mid] > arr[stop]) std::swap(arr[mid], arr[stop]);

    auto left_bound = start;
    auto right_bound = stop;
    auto middle = arr[mid];

    do
    {
        while (arr[left_bound] < middle)
        {
            left_bound++;
        }
        while (arr[right_bound] > middle)
        {
            right_bound--;
        }

        //Меняем элементы местами
        if (left_bound <= right_bound)
        {
            std::swap(arr[left_bound], arr[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);

    if (make_thread && (right_bound - start > 10000))
    {
        // если элементов в левой части больше чем 10000
        // вызываем асинхронно рекурсию для правой части
        auto f = std::async(std::launch::async, [&]() { quickSort(arr, start, right_bound); });
        quickSort(arr, left_bound, stop);
    }
    else
    {
        //запускаем обе части синхронно 
        quickSort(arr, start, right_bound);
        quickSort(arr, left_bound, stop);
    }
}

//int main(int argc, const char* argv[])
//{
//    std::vector<int> v(1000000);
//    std::generate(v.begin(), v.end(), std::rand);
//
//    std::vector<int> copy = v;
//
//    using namespace std::chrono;
//
//    std::cout << "Single thread" << std::endl;
//
//    auto begin = system_clock::now();
//
//    std::sort(copy.begin(), copy.end());
//
//    auto end = system_clock::now();
//    auto interval = end - begin;
//
//    std::cout << "The time: " << duration_cast<milliseconds>(interval).count() << " milliseconds" << std::endl << std::endl;
//
//    std::cout << "Multithread" << std::endl;
//
//    begin = system_clock::now();
//
//    quickSort<int>(v.data(), 0, v.size() - 1);
//
//    end = system_clock::now();
//
//    interval = end - begin;
//
//    std::cout << "The time: " << duration_cast<milliseconds>(interval).count() << " milliseconds" << std::endl;
//
//    assert(copy == v);
//
//    std::cout << "Done" << std::endl;
//
//    return 0;
//}

 int main()
{
    std::vector<res_type> results;
    srand(0);
    RequestHandler rh;
    std::vector<int> vec;
    for (int i = 0; i < 20; i++)
        rh.pushRequest(taskFunc, vec, i, 1 + rand() % 4);
    //for (auto& r : results)
    //{
    //    r.wait();
    //}

    return 0;
}

