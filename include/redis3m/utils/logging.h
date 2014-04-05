#pragma once

#include <string>
#include <memory>
#include <mutex>

namespace redis3m
{

class logging
{
public:
    typedef std::shared_ptr<logging> ptr_t;
    inline static void debug(const std::string& s)
    {
        logger->debug_impl(s);
    }

    inline static void error(const std::string& s)
    {
        logger->error_impl(s);
    }

    virtual void debug_impl(const std::string& s);
    virtual void error_impl(const std::string& s);

private:
    std::mutex access;
    static logging::ptr_t logger;
};
}
