#include <redis3m/utils/logging.h>
#include <iostream>

using namespace redis3m;

logging::ptr_t logging::logger(new logging());

void logging::debug_impl(const std::string &string)
{
    boost::unique_lock<boost::mutex> lock(access);
    std::cerr << string << std::endl;
}

void logging::error_impl(const std::string &string)
{
    boost::unique_lock<boost::mutex> lock(access);
    std::cerr << string << std::endl;
}

