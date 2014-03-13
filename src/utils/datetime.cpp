#include <redis3m/utils/datetime.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace redis3m;

uint64_t datetime::utc_now_in_seconds()
{
    static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
    boost::posix_time::time_duration now_in_seconds = now - epoch;
    return now_in_seconds.total_seconds();
}
