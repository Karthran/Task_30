#include "RequestHandler.h"

RequestHandler::RequestHandler()
{
    m_tpool.start();
}

RequestHandler::~RequestHandler()
{
    m_tpool.stop();
}

auto RequestHandler::pushRequest(FuncType f, std::vector<int>& vec, int id, int arg) -> res_type
{
    return m_tpool.push_task(f, vec, id, arg);
}
