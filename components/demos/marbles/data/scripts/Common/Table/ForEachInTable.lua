--[[
  выполняет функцию для всех обьектов из таблицы
  если функция возвращает false - то происходит досрочный выход из цикла
]]

function ForEachInTable (t, func, ...)
  local index, value = next (t)
  while index do
    if func (value, ...)==false then
      return
    end
    index, value = next (t, index)
  end
end
