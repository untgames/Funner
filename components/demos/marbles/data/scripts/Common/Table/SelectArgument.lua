function SelectArgument(number, ...)
  local params={...}
  local  index,value=next(params)
  while index do
    number=number-1
    if number<1 then
      return value
    end
    index,value=next(params, index)
  end
  return value
end
