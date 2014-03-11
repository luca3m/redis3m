#include <redis3m/patterns/scheduler.h>
#include <boost/assign/list_of.hpp>
#include <redis3m/utils/datetime.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;
using namespace redis3m::patterns;

script_exec scheduler::find_expired_script(
        "local res = redis.call('ZRANGEBYSCORE',KEYS[1],0,ARGV[1],'LIMIT',0,1)\n"
        "if #res > 0 then\n"
        "redis.call('ZADD', KEYS[1], ARGV[2], res[1])\n"
        "return res[1]\n"
        "else\n"
        "return false\n"
        "end\n");


scheduler::scheduler(const std::string &queue_name):
    _queue(queue_name)
{

}

void scheduler::enqueue(connection::ptr_t connection, const std::string &object_id, const uint64_t delay)
{
    connection->run_command(boost::assign::list_of<std::string>("ZADD")(_queue)
                            (boost::lexical_cast<std::string>(datetime::utc_now_in_seconds()+delay))
                            (object_id));
}

void scheduler::dequeue(connection::ptr_t connection, const std::string &object_id)
{
    connection->run_command(boost::assign::list_of<std::string>("ZREM")(_queue)(object_id));
}

std::string scheduler::find_expired(connection::ptr_t connection, const uint64_t lock_for)
{
    uint64_t now = datetime::utc_now_in_seconds();
    std::string now_s = boost::lexical_cast<std::string>(now);
    std::string now_and_lock_for = boost::lexical_cast<std::string>(now+lock_for);
    reply r = find_expired_script.exec(connection,
                                boost::assign::list_of(_queue),
                                boost::assign::list_of(now_s)(now_and_lock_for));
    std::string ret;
    if (r.type() == reply::STRING)
    {
        ret = r.str();
    }
    return ret;
}

