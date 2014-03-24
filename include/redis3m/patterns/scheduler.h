#pragma once

#include <string>
#include <redis3m/patterns/script_exec.h>
#include <redis3m/connection.h>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace redis3m
{
namespace patterns
{
class scheduler
{
public:
    scheduler(const std::string& queue_name);

    void append_enqueue(connection::ptr_t connection, const std::string& object_id, const boost::posix_time::ptime& time);
    void append_enqueue(connection::ptr_t connection, const std::string& object_id, const boost::posix_time::time_duration& delay);

    void enqueue(connection::ptr_t connection, const std::string& object_id, const boost::posix_time::ptime& time);
    void enqueue(connection::ptr_t connection, const std::string& object_id, const boost::posix_time::time_duration& delay);

    void append_dequeue(connection::ptr_t connection, const std::string& object_id);
    void dequeue(connection::ptr_t connection, const std::string& object_id);

    std::string find_expired(connection::ptr_t connection, const boost::posix_time::time_duration& lock_for=boost::posix_time::seconds(60));

private:
    std::string _queue;
    static script_exec find_expired_script;
};
}
}
