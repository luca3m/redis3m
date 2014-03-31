redis3m
=======
[![Build Status](https://travis-ci.org/luca3m/redis3m.png?branch=master)](https://travis-ci.org/luca3m/redis3m)

A C++ [Redis](http://redis.io) driver

It's an experimental project, born to bring my experience using Redis and C++ on a opensource library.


### Main goals

1. Wrap official [hiredis](http://github.com/redis/hiredis) C driver, for easy upgrade in the future
2. Provide High Availability connection pooling using redis sentinel
3. Provide some useful redis patterns, like [scheduler](http://luca3m.me/2013/12/03/redis-scheduler.html), [orm](http://github.com/soveran/ohm), counters or message queueing


### Dependencies

redis3m requires hiredis and boost libraries.

### Install

First step install all required dependencies, on a Debian system you can use:

```bash
sudo apt-get install libmsgpack-dev libboost-thread-dev libboost-date-time-dev libboost-test-dev libboost-filesystem-dev libboost-system-dev libhiredis-dev cmake build-essential
```

Then checkout the code and compile it
```bash
git clone https://github.com/luca3m/redis3m
cd redis3m
cmake -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

### Documentation

See [examples](https://github.com/luca3m/redis3m/tree/master/examples) directory for some examples, you can compile them with:

```bash
g++ <example.cpp> $(pkg-config --cflags --libs redis3m) -o <example.bin>
```

As reference you can read [include](https://github.com/luca3m/redis3m/tree/master/include) files, they are pretty simple and some of them are already documented with Doxygen.
