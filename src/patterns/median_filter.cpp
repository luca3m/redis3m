#include <redis3m/patterns/median_filter.h>
#include <redis3m/utils/file.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;
using namespace redis3m::patterns;

script_exec median_filter::add_sample_script(redis3m::utils::datadir("/lua/add_sample.lua"), true);
script_exec median_filter::get_median_script(redis3m::utils::datadir("/lua/get_median.lua"), true);

median_filter::median_filter(const std::string &prefix, int samples):
    _prefix(prefix),
    _samples(samples)
{
}

void median_filter::add_sample(connection::ptr_t connection, const std::string &tag, double value)
{
    add_sample_script.exec(connection, command(list_key(tag))(zset_key(tag))(_prefix+":id"),
                           command(boost::lexical_cast<std::string>(value))
                                (boost::lexical_cast<std::string>(_samples)));
}

double median_filter::median(connection::ptr_t connection, const std::string &tag)
{
    reply r = get_median_script.exec(connection, command(zset_key(tag)));
    return boost::lexical_cast<double>(r.str());
}
