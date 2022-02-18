#pragma once
#include "ThreadPool.h"

class RequestHandler
{
public:
    RequestHandler();
    ~RequestHandler();
    auto pushRequest(FuncType f, std::vector<int>& vec,int id, int arg) -> void;

private:
    ThreadPool m_tpool;
};
