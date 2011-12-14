print ("Results of attributes_test:")

require "com.untgames.runtime"

serializable = attribute ()
atomic = attribute ()
composite = attribute (serializable, atomic)

local function f ()
end

local function g ()
end

local function h ()
end

serializable (f)
atomic (g)
composite (h)

print ("Is serializable: " .. tostring (serializable.Check (f)))
print ("Is serializable: " .. tostring (serializable.Check (g)))
print ("Is serializable: " .. tostring (serializable.Check (h)))
print ("Is atomic:       " .. tostring (atomic.Check (f)))
print ("Is atomic:       " .. tostring (atomic.Check (g)))
print ("Is atomic:       " .. tostring (atomic.Check (h)))
print ("Is composite:    " .. tostring (composite.Check (f)))
print ("Is composite:    " .. tostring (composite.Check (g)))
print ("Is composite:    " .. tostring (composite.Check (h)))
