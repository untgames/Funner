function UnpackString (S, terminal, ReturnAsTable)
  -- S -строка, которую нужно распаковать
  -- terminal - разделитель
  if S==nil then return nil end
  S=S..terminal --присоединяем еще один терминал в конец строки, чтобы не пришлось 
                --послений найденный элемент записывать в результаты вне основного цикла
  local results={}
  local ResultNumber=0
  local init=1
  local found = string.find(S, terminal, init)
  local terminal_len = string.len(terminal)
  while found do
    ResultNumber=ResultNumber+1
    results[ResultNumber]=string.sub(S, init, found-1)
    init=found+terminal_len
    found =string.find(S, terminal, init)
  end
  if ReturnAsTable then
    return results
  end
  return unpack(results)
end
