--[[
  эта функци€ используетс€ дл€ обертки error(message)
  фактически выпон€ет то же, что и assert(value, message)
  только, в отличии от assert, позвол€ет поределить путь по которому проходили вызовы
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

