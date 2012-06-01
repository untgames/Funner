function ToNumbers(...)
  --[[
  преобразует принятые аргументы из строк в числа
  ]]
  local results={}
  local ResultNumber=0
  local params={...}
  local index, value = next(params)
  while index do
    value=tonumber(value)
    ResultNumber=ResultNumber+1
    results[ResultNumber]=value
    index, value = next(params, index)
  end
  return unpack(results)
end
