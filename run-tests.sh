#!/bin/bash

# Standalone instance
redis-server --port 6379 --save "" &> /dev/null &

# HA instaces with sentinel
for x in 6380 6381 6382; do
  redis-server --port $x --save "" &> /dev/null &
done
sleep 0.1
for x in 6381 6382; do
  redis-cli -p $x slaveof 127.0.0.1 6380
done
redis-server tests/redis-sentinel.conf --sentinel &> /dev/null &

sleep 0.3

# Run tests
make test
RESULT=$?

# Cleanup all instances
jobs -p | xargs kill

exit $RESULT
