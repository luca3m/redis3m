
for x in 6380 6381 6382; do
  redis-server --port $x --save "" &
done
sleep 0.1
for x in 6381 6382; do
  redis-cli -p $x slaveof 127.0.0.1 6380
done
redis-server tests/redis-sentinel.conf --sentinel
