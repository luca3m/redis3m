#pragma once

#include <redis3m/connection.h>
#include <redis3m/patterns/script_exec.h>

namespace redis3m
{
namespace patterns
{

class median_filter
{
public:
    median_filter(const std::string& prefix="", int samples=11);

    void add_sample(connection::ptr_t connection, const std::string& tag, double value);

    double median(connection::ptr_t connection, const std::string& tag);

    inline std::string list_key(const std::string& tag)
    {
         return _prefix + ":list-" + tag;
    }

    inline std::string zset_key(const std::string& tag)
    {
        return _prefix + ":zset-" + tag;
    }

private:
    std::string _prefix;
    int _samples;
    static script_exec add_sample_script;
    static script_exec get_median_script;
};
}
}
