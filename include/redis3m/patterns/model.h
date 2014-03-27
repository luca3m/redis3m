#pragma once

#include <string>
#include <boost/lexical_cast.hpp>
#include <map>
#include <redis3m/utils/exception.h>

namespace redis3m
{
namespace patterns
{

REDIS3M_EXCEPTION(model_not_loaded)

#define REDIS3M_MODEL_RO_ATTRIBUTE(type, name) \
public:\
    inline const type& name() const { if (_loaded) return _##name; else throw redis3m::patterns::model_not_loaded(); }\
private:\
    type _##name;

/**
 * @brief Class useful to define models used by {@link orm} and {@link simple_obj_store}.
 * REDIS3M_MODEL_RO_ATTRIBUTE(type, name) macro defines automatically an attribute with
 * a public getter.
 * All fields need to be serialized to a std::map<std::string, std::string>. Model class
 * contains various helpers to to that easily.
 */
class model
{
public:
    model():
        _loaded(false)
    {

    }

    model(const std::string& id, const std::map<std::string, std::string>& map):
        _id(id),
        _loaded(true)
    {

    }

    std::map<std::string, std::string> to_map()
    {
        return std::map<std::string, std::string>();
    }

    inline const std::string& id() const { if (_loaded) return _id; else throw model_not_loaded(); }
    inline bool loaded() const { return _loaded; }

protected:
    inline static std::string read_str_from_map(const std::map<std::string, std::string>& map,
                                             const std::string& key,
                                             const std::string& default_value="")
    {
        if (map.find(key) != map.end())
        {
            return map.at(key);
        }
        else
        {
            return default_value;
        }
    }

    inline static void write_str_to_map(std::map<std::string, std::string>& map,
                                            const std::string& key,
                                            const std::string& value,
                                            const std::string& default_value="")
    {
        if (value != default_value)
        {
            map[key] = value;
        }
    }

    template<typename IntegerType>
    inline static IntegerType read_int_from_map(const std::map<std::string, std::string>& map,
                                                    const std::string& key,
                                                    const IntegerType default_value=0)
    {
        if (map.find(key) != map.end())
        {
            return boost::lexical_cast<IntegerType>(map.at(key));
        }
        else
        {
            return default_value;
        }
    }

    template<typename IntegerType>
    inline static void write_int_to_map(std::map<std::string, std::string>& map,
                                            const std::string& key,
                                            const IntegerType value,
                                            const IntegerType default_value=0)
    {
        if (value != default_value)
        {
            map[key] = boost::lexical_cast<std::string>(value);
        }
    }

    inline static bool read_bool_from_map(const std::map<std::string, std::string>& map,
                                              const std::string& key)
    {
        if (map.find(key) != map.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline static void write_bool_to_map(std::map<std::string, std::string>& map,
                                             const std::string& key,
                                             const bool value)
    {
        if (value)
        {
            map[key] = "true";
        }
    }

    std::string _id;
    bool _loaded;
};


}
}
