#include "RequestHandler.h"

RequestHandler::RequestHandler()
{
    m_tpool.start();
}

RequestHandler::~RequestHandler()
{
    m_tpool.stop();
}

auto RequestHandler::pushRequest(FuncType f, int id, int arg) -> void
{
    m_tpool.push_task(f, id, arg);
}
