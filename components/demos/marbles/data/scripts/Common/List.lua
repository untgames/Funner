List = {}
List.__index = List

function List.Create () 
  local list = {}
    
  setmetatable (list, List)
    
  list.first = nil
  list.size = 0
    
  return list
end

function List:CreateLink (value)
  local link = {}

  link.value = value
  link.next  = nil
  link.prev  = nil

  return link
end

function List:PushFront (something)
  if something == nil then
    return
  end
    
  local link = self:CreateLink (something)

  link.next = self.first

  if self.first ~= nil then
    self.first.prev = link
  end

  self.first = link
  self.size = self.size + 1
    
  return link
end

function List:RemoveByLink (link)
  if (link == nil) then
    return
  end

  if link.prev ~= nil then
    link.prev.next = link.next
  else
    self.first = link.next
  end
  
  if link.next ~= nil then
    link.next.prev = link.prev
  end
  
  self.size = self.size - 1
end

function List:RemoveByValue (value)
  local link = self.first

  while not (link == nil) do
    if link.value == value then
      local nxt = link.next

      self:RemoveByLink (link)
      link = nxt
    else
      link = link.next
    end
  end
end

function List:ForEach (f, ...)
  local link = self.first
  local i = 0
    
  while not (link == nil) do
    f (link.value, ...)
    link = link.next
  end
end
