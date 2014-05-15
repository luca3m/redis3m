local list_key = KEYS[1]
local zset_key = KEYS[2]
local sample_id = KEYS[3]
local value = tonumber(ARGV[1])
local max_samples = tonumber(ARGV[2])

local id = redis.call("INCR", sample_id)

redis.call("LPUSH", list_key, id)
redis.call("ZADD", zset_key, value, id)

local count = redis.call("LLEN", list_key)

if count > max_samples then
  local removed = redis.call("RPOP", list_key)
  redis.call("ZREM", zset_key, removed)
end
