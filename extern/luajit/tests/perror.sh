#!/usr/bin/env luajit
function f (x)
  local z = x.x + 1.0
end

print ("before error")

f ("Hello")

print ("after error")