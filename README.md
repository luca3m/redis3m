redis3m
=======

A C++ [Redis](http://redis.io) driver

It's an experimental project, born to bring my experience using Redis and C++ on a opensource library.


### Main goals

1. Wrap official [hiredis](http://github.com/redis/hiredis) C driver, for easy upgrade in the future
2. Provide High Availability connection pooling using redis sentinel
3. Provide some useful redis patterns, like [scheduler](http://luca3m.me/2013/12/03/redis-scheduler.html), [orm](http://github.com/soveran/ohm), counters or message queueing


### Dependencies

redis3m requires hiredis and boost libraries.
