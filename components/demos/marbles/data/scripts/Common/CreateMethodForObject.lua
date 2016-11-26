function CreateMethodForObject(func, obj)
  assert(func,"ERROR: MakeMethodForObject(func, obj) - func is nil")
  assert(obj,"ERROR: MakeMethodForObject(func, obj) - obj is nil")
  -- создает из универсальной функции метод для конкретного обьекта
  local meth=function(...)
    return func( obj, ...)
  end
  return meth
end

