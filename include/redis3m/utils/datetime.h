// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <stdint.h>
#ifndef NO_BOOST
#include <boost/date_time/posix_time/posix_time.hpp>
#else
#include <chrono>
#endif

namespace redis3m
{
namespace datetime
{

#ifndef NO_BOOST
typedef boost::posix_time::ptime ptime;
typedef boost::posix_time::time_duration time_duration;
typedef boost::posix_time::seconds seconds;
#else
using ptime = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
using time_duration = std::chrono::seconds;
using seconds = std::chrono::seconds;
#endif

uint64_t utc_now_in_seconds();
uint64_t ptime_in_seconds(const ptime& time);
ptime now();

}
}
