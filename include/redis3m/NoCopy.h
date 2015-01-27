#pragma once

#define NOCOPY(classname)					private: \
												explicit classname(const classname&); \
												classname& operator=(const classname&);
#ifdef NO_BOOST
class NoCopy
{
	NOCOPY(NoCopy);
public:

};
#define NOCOPY_BASE  
#else
#include <boost/noncopyable.hpp>
#define NOCOPY_BASE : boost::noncopyable
#endif