#include <redis3m/redis3m.hpp>
#include <iostream>

int main(int argc, char **argv)
{
        redis3m::connection::ptr_t conn = redis3m::connection::create();
        conn->run(redis3m::command("SET") << "foo" << "bar" );
        redis3m::reply r = conn->run(redis3m::command("GET") << "foo" );        
        std::cout << "FOO is: " << r.str() << std::endl;
}
