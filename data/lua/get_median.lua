local list_key = KEYS[1]
local sorted_values = redis.call("SORT", list_key)
local size = #sorted_values
local median = 0.0

if size % 2 == 0 then
  -- Use to string because median value may be floating point
  median = (sorted_values[size/2]+sorted_values[size/2+1]) / 2
else
  median = sorted_values[(size+1)/2]
end
return tostring(median)
