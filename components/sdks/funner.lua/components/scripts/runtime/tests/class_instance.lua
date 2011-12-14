print ("Results of class_instance_test:")

require "com.untgames.runtime"

local function CreateMyClass ()
  local function Foo ()
    print ("MyClass.Foo")
  end
  
  local function Bar ()
    print ("MyClass.Bar")
  end
  
  local function SetProperty (name, value)
    print (string.format ("SetProperty('%s', '%s')", name, value))
  end
  
  local function GetProperty (name)
    print (string.format ("GetProperty('%s')", name))
  end
  
  local function Create ()
    local function Baz ()
      print ("MyClass.Baz")
    end

    return class { Baz = Baz }
  end

  return class "MyClass" { Foo = Foo, Bar = Bar, set_A = SetProperty, get_A = GetProperty, Create = constructor (Create)}
end

local MyClass = CreateMyClass ()

local instance = MyClass.Create ()

instance.Foo ()
instance.Bar ()
instance.Baz ()

instance.A = 1

local x = instance.A

print (string.format ("Instance name: '%s'", tostring (instance)))
