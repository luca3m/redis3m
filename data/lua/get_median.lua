local zset_key = KEYS[1]

local size = redis.call("ZCARD", zset_key)

if size % 2 == 0 then
  local middles = redis.call("ZRANGE", zset_key, size/2-1, size/2, "WITHSCORES")
  -- Use to string because median value may be floating point
  return tostring((middles[2]+middles[4]) / 2)
else
  local middle = redis.call("ZRANGE", zset_key, math.floor(size/2), math.floor(size/2), "WITHSCORES")
  return tostring(middle[2])
end
