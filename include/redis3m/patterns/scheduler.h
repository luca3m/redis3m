#pragma once

#include <string>
#include <redis3m/patterns/script_exec.h>
#include <redis3m/connection.h>
#include <inttypes.h>

namespace redis3m
{
namespace patterns
{
class scheduler
{
public:
    scheduler(const std::string& queue_name);

    void enqueue(connection::ptr_t connection, const std::string& object_id, const uint64_t delay);
    void dequeue(connection::ptr_t connection, const std::string& object_id);

    std::string find_expired(connection::ptr_t connection, const uint64_t lock_for=60);

private:
    std::string _queue;
    static script_exec find_expired_script;
};
}
}
