print ("Results of static_class_test:")

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

  return class "MyClass" { Foo = Foo, Bar = Bar, set_A = SetProperty, get_A = GetProperty}
end

local MyClass = CreateMyClass ()

MyClass.Foo ()
MyClass.Bar ()

MyClass.A = 1

local x = MyClass.A
