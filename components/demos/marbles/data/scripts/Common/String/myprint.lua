local p=print

function myprint(...)
  --[[
  создает строку из нескольких принимаемых параметров и печатает ее, например:
  myprint("a=",1," b=",2," count:",3,4,5) ==> " a=1 b=2 count:345"
  ]]
  local params={...}
  local value=nil
  local index=nil
  index, value = next(params)
  local s=""
  while index do
    if type(value)=="number" then
      value=" "..string.format("%4.4f",value).." "
    else
      value=tostring(value)
    end
    s=s..value
    index, value = next(params, index)
  end
  p(s)
  return s
end

print=myprint