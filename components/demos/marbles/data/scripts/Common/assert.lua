--[[
  эта функция используется для обертки error(message)
  фактически выпоняет то же, что и assert(value, message)
  только, в отличии от assert, позволяет поределить путь по которому проходили вызовы
]]
local AssertionPrefixString=""

function AssertionPrefix(str)
  if str then
    AssertionPrefixString=str
  else
    AssertionPrefixString=""
  end
end

function assert(value, str)
  if value then
    return
  end
  error(AssertionPrefixString..str)
end

