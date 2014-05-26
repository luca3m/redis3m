#include <redis3m/patterns/median_filter.h>
#include <redis3m/utils/file.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;
using namespace redis3m::patterns;

script_exec median_filter::get_median_script(redis3m::utils::datadir("/lua/get_median.lua"), true);

median_filter::median_filter(const std::string &prefix, int samples):
    _samples(samples)
{
    if (!prefix.empty())
    {
        _prefix= prefix+":";
    }
}

void median_filter::add_sample(connection::ptr_t connection, const std::string &tag, double value)
{
  connection->append(command("MULTI"));
  connection->append(command("LPUSH")(list_key(tag))(boost::lexical_cast<std::string>(value)));
  connection->append(command("LTRIM")(list_key(tag))("0")(boost::lexical_cast<std::string>(_samples-1)));
  connection->append(command("EXEC"));
  connection->get_replies(4);
}

double median_filter::median(connection::ptr_t connection, const std::string &tag)
{
    reply r = get_median_script.exec(connection, command(list_key(tag)));
    if (r.type() == reply::STRING)
    {
        return boost::lexical_cast<double>(r.str());
    }
    else
    {
        return 0;
    }
}
