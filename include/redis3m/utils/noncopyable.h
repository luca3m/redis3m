#pragma once

namespace redis3m
{

namespace utils
{

class noncopyable
{
public:
	noncopyable( const noncopyable& ) = delete;
	noncopyable& operator=( const noncopyable& ) = delete;

protected:
	noncopyable() = default;
	virtual ~noncopyable() = default;
};

}
}