local binding={}

function InitResourceManager()
  ResourceManager={}
  
  ResourceManager.Load=function (name)
    assert(name,"ERROR: ResourceManager.Load(name) - name is nil")
    local res=binding[name]
    if  not(res) then
      res=Engine.ResourceManager.CreateFileGroupBinding ("data/res/"..name..".xreslist")
      binding[name]=res
    end
    res:Load()
    --print("===== ResourceManager.Load(",name,")")
  end
  
  ResourceManager.Unload=function (name)
    assert(name,"ERROR: ResourceManager.Unload(name) - name is nil")
    binding[name]=nil
    collectgarbage("collect")
    Engine.ResourceManager.FlushUnusedResources()
    --print("===== ResourceManager.Unload(",name,")")
  end
  
  return ResourceManager
end


