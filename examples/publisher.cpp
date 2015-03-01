/*
 * Carmelo Porcelli
 */

#include <redis3m/redis3m.hpp>
#include <iostream>

using namespace redis3m;

int main(int argc, char **argv)
{
        connection::ptr_t conn = connection::create();
        std::string send;
        if (argv[1] != NULL)
        {
            send = argv[1];
        }
        else
        {
            send = "Undefined";
        }
        reply r = conn->run(command("PUBLISH") << "topic" << send);
}
