-- Разбиение строки на несколько
function SplitString (str, terminal, return_as_table)
  if not str then return nil end

  str = str .. terminal

  local results       = {}
  local result_number = 0
  local init          = 1
  local found         = string.find (str, terminal, init)
  local terminal_len  = string.len (terminal)

  while found do
    local substr = string.sub (str, init, found - 1)

    if substr ~= "" then
      result_number           = result_number + 1    
      results [result_number] = substr
    end
    
    init  = found + terminal_len
    found = string.find (str, terminal, init)
  end

  if return_as_table then return results end

  return unpack (results)
end

-- Преобразование таблицы строк в таблицу номеров
function ToNumbers (...)
  local result = {}

  for _, value in pairs ({...}) do
    table.insert (result, tonumber (value))
  end

  return unpack (result)
end

-- Преобразованеи строки в вектор
function StringToVec (str)
  local a, b, c, d = ToNumbers (SplitString (str, ";"))

  if not b then return nil end

  if not c then return vec2 (a, b) end
  
  if not d then return vec3 (a, b, c) end
  
  return vec4 (a, b, c, d)
end
