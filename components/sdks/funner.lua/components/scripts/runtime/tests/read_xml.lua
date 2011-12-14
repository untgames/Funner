print ("Results of read_xml_test:")

require "com.untgames.runtime"

local function CreateClassA ()
  local function Foo ()
    print ("ClassA.Foo")
  end  
  
  local function SetProperty (name, value)
    print (string.format ("SetProperty('%s', '%s')", name, value))
  end
  
  local function GetProperty (name)
    print (string.format ("GetProperty('%s')", name))
  end
    
  local function Create ()
    local function Baz ()
      print ("ClassA.Baz")
    end

    return class { Baz = Baz }
  end

  return class "ClassA"
  {
    Foo    = Foo,
    set_A  = Runtime.Attribute.String (SetProperty),
    get_A  = Runtime.Attribute.String (GetProperty),
    Create = constructor (Create)
  }
end

ClassA = CreateClassA ()

local instance = Runtime.Serialize.FromXml ("data/test.xml")
