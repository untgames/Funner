-- Инициализация графики
local function InitGraphics ()
  -- Создание экрана
  
  local screen = Scene.Screen.Create ()
  
  screen:DisableBackground ()
  
  -- Привязка экрана к окну в config.xml

  Engine.Screens.Register ("Screen1", screen)
  
  -- Добавление на экран области вывода

  local viewport = Scene.Viewport.Create ()
  
  viewport.Technique       = "Render2d"
  viewport.ZOrder          = 0
  viewport.BackgroundColor = vec4 (0, 1, 0, 1)  

  viewport:SetArea (0, 0, 100, 100)
  viewport:EnableBackground ()

  screen:Attach (viewport)
  
  local methods = {}
  
  methods.SetCamera = function (camera) viewport.Camera = camera end
  
  return methods
end

-- Инициализация ввода  
local function InitInput () 
  local this = {}  
  
  local mouse_x = 0
  local mouse_y = 0

  -- Обработчики
  
  this.OnWindowClosed = function ()
    print ("Window closed")
    
    System.Application.Exit (0)
  end
  
  this.OnCursorX = function (value)      
    mouse_x = value
  end
  
  this.OnCursorY = function (value)
    mouse_y = value
  end  
  
  this.OnMouseDown = function () 
    print (string.format ("left button down (x=%g, y=%g)", mouse_x, mouse_y))
  end
  
  this.OnMouseUp = function ()
    print (string.format ("left button up (x=%g, y=%g)", mouse_x, mouse_y))
  end  
  
  this.OnCursorEnterWindow = function ()
    print ("cursor in window")
  end
  
  this.OnCursorLeaveWindow = function ()
    print ("cursor out of window")
  end  

  -- Регистрация обработчика событий ввода. CreateEventHandler без параметров перенаправит все события ввода в скрипт
  
  local function EventsDispatch (command)
    -- Компиляция команды
    
    local status, value = pcall (loadstring (string.format ("return function (this) %s end", command)))
    local fn
    
    if status then
      fn = value
    else
      print (string.format ("input error: %s", value))

      return
    end

    -- Запуск команды

    status, value = pcall (fn, this)
    
    if not status then
      print (string.format ("input error: %s", value))      
    end
  end

  Engine.InputEventHandlers.Register ("game", Input.CreateEventHandler (EventsDispatch))  

  return {}
end

-- Загрузка ресурсов
local function InitResources ()
  local resource_group = Engine.ResourceManager.Group.Create ()

  resource_group:Add ("data/materials.xmtl")
  resource_group:Add ("data/sounds.snddecl")  

  local resource_binding = Engine.ResourceManager.CreateBinding (resource_group)

  resource_binding:Load ()

  return {}
end

-- Создание тестовой сцены
local function InitScene (gfx)
  -- Создание сцены и камеры

  local scene  = Scene.Scene.Create ()
  local camera = Scene.OrthoCamera.Create ()
  
  scene.Root.Scale = vec3 (1, -1, 1)
  
  camera.Left   = 0
  camera.Right  = 100
  camera.Top    = -100
  camera.Bottom = 0
  
  camera:BindToScene (scene)
  
  -- Создание слушателя

  local listener = Scene.Listener.Create () 

  listener:BindToScene (scene)   
  listener:SetPosition (0, 0, -1)
  
  -- Связывание слушателя с звуковой системой в config.xml

  Engine.Listeners.Register ("Listener1", listener)

  -- Установка активной камеры
  
  gfx.SetCamera (camera)
  
  -- Загрузка спрайтов  
  
  local function LoadSprite (node, z)
    local sprite   = Scene.Sprite.Create ()
    local position = StringToVec (node:Get ("position"), ";")
    local size     = StringToVec (node:Get ("size"), ";")

    sprite.Position = vec3 (position.x, position.y, z) 
    sprite.Scale    = vec3 (size.x, size.y, 1)
    sprite.Material = node:Get ("material")

    sprite:SetOrientation (tonumber (node:Get ("angle", "0")), 0, 0, 1)

    return sprite
  end
  
  -- Загрузка звука
  local function LoadSound (node)
    local emitter = Scene.SoundEmitter.Create (node:Get ("source"))
    
    emitter.Gain = tonumber (node:Get ("gain", "1"))    
    
    emitter:BindToScene (scene) 
    emitter:Play ()

    return emitter
  end  
  
  local xml         = Common.StringNode.LoadXml ("data/user_data.xml")
  local nodes_count = xml.ChildrenCount
  local z           = 0  
  local sprites     = {}
  local sounds      = {}

  for i=0, nodes_count-1 do
    local node = xml:Child (i)
    
    local function SafeLoad ()
      if node.Name == "sprite" then  
        local sprite = LoadSprite (node, z)
          
        sprite:BindToScene (scene)
            
        z = z + 0.001
            
        table.insert (sprites, sprite)
      elseif node.Name == "sound" then
        local sound = LoadSound (node)
        
        table.insert (sounds, sound)
      end    
    end
    
    local status, error_message = pcall (SafeLoad)
    
    if not status then
      print (string.format ("load error: %s", error_message))
    end
  end  
  
  return sprites
end

-- Обновление игрового состояния
local function Update (timer, sprites)
  timer:Update ()
  
  for _, sprite in pairs (sprites) do
    sprite.Alpha = (math.cos (timer.Time) + 1) / 2
  end
end

-- Главная функция
local function Main ()
  local gfx_system      = InitGraphics () -- ссылки хранятся, чтобы сборщик мусора не удалил вложенные объекты
  local resource_system = InitResources ()      
  local input_system    = InitInput ()  
  local sprites         = InitScene (gfx_system)
  local timer           = Common.Timer.Create (false, true)  
  
  timer.Started = true
  
  -- Главный цикл  
  
  local action_queue = Common.ActionQueue.Create ()    
  local update_connection
  
  local function SafeUpdate ()
    local status, error_message = pcall (Update, timer, sprites)

    if not status then
      print (string.format ("main loop error: %s", error_message))
      
      update_connection:Disconnect ()
    end
  end
  
  update_connection = action_queue:RegisterEventHandler (Common.ActionQueue.CreateEventHandler (SafeUpdate))
end

-- Вызов Main
Main ()
