function PackString(terminal, ...)
  --[[
  создает строку из нескольких принимаемых параметров, разделяя их терминалом
  ]]
  terminal=tostring(terminal)
  local params={...}
  local value=nil
  local index=nil
  index, value = next(params)
  local s=""
  while index do
    if s~="" then
      --если строка уже не пустая - вписать терминал перед следующим элементом
      s=s..terminal
    end
    if type(value)=="string" then
      s=s..value
    else
      s=s..tostring(value)
    end
    index, value = next(params, index)
  end
  return s
end