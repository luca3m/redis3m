// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/utils/datetime.h>

using namespace redis3m;

uint64_t datetime::utc_now_in_seconds()
{
#ifndef NO_BOOST
    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
    return ptime_in_seconds(now);
#else
    datetime::ptime now = std::chrono::time_point_cast<datetime::ptime::duration>(std::chrono::system_clock::now());
    return ptime_in_seconds(now);
#endif
}

uint64_t datetime::ptime_in_seconds(const datetime::ptime &time)
{
#ifndef NO_BOOST
    static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration seconds_since_epoch = time - epoch;
    return seconds_since_epoch.total_seconds();
#else
    return std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
#endif
}

datetime::ptime datetime::now()
{
#ifndef NO_BOOST
    return boost::posix_time::second_clock::universal_time();
#else
    return std::chrono::time_point_cast<datetime::ptime::duration>(std::chrono::system_clock::now());
#endif
}
