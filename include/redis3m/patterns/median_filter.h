#pragma once

#include <redis3m/connection.h>
#include <redis3m/patterns/script_exec.h>

namespace redis3m
{
namespace patterns
{

/**
 * @brief Implement median filter algorithm on Redis.
 * A single instance can handle multiple filters, with a prefix.
 */
class median_filter
{
public:
    /**
     * @brief Constructor
     * @param prefix prefix to use for filters managed by this instance
     * @param samples numbers of samples
     */
    median_filter(const std::string& prefix="", int samples=11);

    /**
     * @brief Adds a sample to a filter
     * @param connection
     * @param tag identifies a specific filter
     * @param value sample value
     */
    void add_sample(connection::ptr_t connection, const std::string& tag, double value);

    /**
     * @brief Get actual median from samples stored on db
     * @param connection
     * @param tag identifies a specific filter
     * @return median value
     */
    double median(connection::ptr_t connection, const std::string& tag);

    /**
     * @brief List used to store samples in arrival order.
     * Use this method to do something on this key, like set an expiring.
     * @param tag identifies a specific filter
     * @return
     */
    inline std::string list_key(const std::string& tag)
    {
         return _prefix + ":" + tag + ":list";
    }

    /**
     * @brief ZSET used to store samples by score. Used to compute
     * median value. Same concerns as {@link list_key()}
     * @param tag identifies a specific filter
     * @return
     */
    inline std::string zset_key(const std::string& tag)
    {
        return _prefix + ":" + tag + ":zset";
    }

private:
    std::string _prefix;
    int _samples;
    static script_exec add_sample_script;
    static script_exec get_median_script;
};
}
}
