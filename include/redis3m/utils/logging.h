// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace redis3m
{

class logging
{
public:
    typedef boost::shared_ptr<logging> ptr_t;
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
    boost::mutex access;
    static logging::ptr_t logger;
};
}
