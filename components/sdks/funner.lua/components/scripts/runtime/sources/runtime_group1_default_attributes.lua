Runtime = Runtime or {}

Runtime.Attribute =
{
  Type                = attribute (),
  Class               = function (value)            Runtime.Attribute.Type [value] = "class"; return value end,
  Number              = function (value)            Runtime.Attribute.Type [value] = "number"; return value end,
  String              = function (value)            Runtime.Attribute.Type [value] = "string"; return value end,
  Function            = function (value, signature) Runtime.Attribute.Type [value] = "function"; return value end,  

  StringTreeConverter = attribute ()
}
