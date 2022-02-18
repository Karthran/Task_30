#include "RequestHandler.h"

RequestHandler::RequestHandler()
{
    m_tpool.start();
}

RequestHandler::~RequestHandler()
{
    m_tpool.stop();
}

auto RequestHandler::pushRequest(FuncType f, std::vector<int>& vec, int id, int arg) -> void
{
    m_tpool.push_task(f, vec, id, arg);
}
