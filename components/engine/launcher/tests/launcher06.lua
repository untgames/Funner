-- Инициализация графики
local function InitGraphics ()
  -- Создание экрана
  
  local screen = Scene.Screen.Create ()
  
  screen.BackgroundColor = vec4 (0, 1, 0, 1)
  
  -- Привязка экрана к окну в config.xml

  Engine.Screens.Register ("Screen1", screen)
  
  -- Добавление на экран области вывода

  local viewport = Scene.Viewport.Create ()
  
  viewport.Technique = "Render2d"
  viewport.ZOrder    = 0

  viewport:SetArea (0, 0, 100, 100)

  screen:Attach (viewport)
  
  return {} 
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
    
    local z = undefined_table.some_field -- string with error
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

-- Создание тестовой сцены


-- Главная функция
local function Main ()
  local gfx_system   = InitGraphics () -- ссылки хранятся, чтобы сборщик мусора не удалил вложенные объекты
  local input_system = InitInput ()
  
end

-- Вызов Main
Main ()
