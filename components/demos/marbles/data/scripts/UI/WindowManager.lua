
--[[
  создает глобальные:
    переменные:
      WindowManager - менеджер окон
      CursorSprite - спрайт курсора можно изменять
      CursorNode - узел для курсора, к нему можно привязать все, что надо двигать вместе с курсором
      SoundEffectsVolume - громкость для вывода звуковых эффектов
    функции:
      PlaySound(SoundName) - проигрывает звук
      PlaySoundEffect(SoundName) - проигрывает звук, предварительно установив громкость, равную SoundEffectsVolume

  ПРИМЕЧАНИЕ:
  Координаты во вьюпорте менеджера окон имеют размерность 100x100
  Z=0..100
  
  Переменные:
    WindowManager.Viewport
    WindowManager.Scene
    WindowManager.Camera
    WindowManager.Listener        - слушатель сцены, привязан к камере
    WindowManager.Node            - узел для привязки окон
    WindowManager.OpenedWindows         - список открытых окон
    WindowManager.ActiveWindow    - текущее активное окно
    WindowManager.FocusIfPointed  - "true" - окна становятся активными при наведении курсора, "false" - по клику
    
  Методы:
    WindowManager.DeactivateCurrentWindow ()
    WindowManager.MakeWindowActive (window)
    WindowManager.OpenWindow (window)
    WindowManager.CloseWindow (window)
    WindowManager.GetFunctionForWindowClose (window) 
    WindowManager.GetWindowByName (name)
    WindowManager.GetMouseXY (window) - если передано окно - выдает координаты в окне
  
  

]]

function InitWindowManager(screen)
  assert(screen~=nil, "ERROR: InitWindowManager(screen) - screen is nil")
  assert(Mouse~=nil, "ERROR: InitWindowManager(screen) - Mouse==nil (need Mouse.lua initialization)")
  assert(Keyboard~=nil, "ERROR: InitWindowManager(screen) - Keyboard==nil (need Keyboard.lua initialization)")
  
  WindowManager = {}

  
  -- инициализация графики
 
  WindowManager.Viewport=Render.Viewport.Create ()
    WindowManager.Viewport.Name = "WindowManager.Viewport"
    WindowManager.Viewport.RenderPath = "Render2d"
    WindowManager.Viewport:SetArea (0, 0, 100, 100);
    WindowManager.Viewport.ZOrder = 10 --вьюпорт менджера должен рисоваться после остальных, но раньше курсора
    screen:Attach (WindowManager.Viewport)
  
  WindowManager.Scene = Scene.Scene.Create ()

  WindowManager.Camera = Scene.OrthoCamera.Create ()
  WindowManager.Camera:BeginUpdate()
      WindowManager.Camera.Name = "WindowManager.Camera"
      WindowManager.Camera.Position = vec3 (0, 0, 0)
      WindowManager.Camera.Left = 0
      WindowManager.Camera.Right = 800
      WindowManager.Camera.Top = 0
      WindowManager.Camera.Bottom = -600
      WindowManager.Camera.ZNear = 0
      WindowManager.Camera.ZFar = 100
    WindowManager.Camera:EndUpdate ()
    WindowManager.Camera:BindToScene (WindowManager.Scene)
    WindowManager.Viewport.Camera = WindowManager.Camera
  ----------------------------------------------------------------------------------------------------------
  --инициализация звука
  ----------------------------------------------------------------------------------------------------------

  local listener = Scene.Listener.Create ()
  listener:BindToParent (WindowManager.Camera, Scene.NodeBindMode.AddRef)
  WindowManager.Listener=listener
  Engine.Listeners.Register ("Listener1", listener)
  
  SoundEffectsVolume = 1
  
  function PlaySoundEffect (sound_name)
    local emitter = Scene.SoundEmitter.Create (sound_name)
    emitter.Name = "SoundEffectEmitter"
    emitter:BindToScene (WindowManager.Scene)
    emitter:Play ()
    emitter.Gain = SoundEffectsVolume
    --myprint("PlaySoundEffect: ", sound_name, " Gain: ", emitter.Gain)
    return emitter
  end
  
  function PlaySound(sound_name)
    local emitter = Scene.SoundEmitter.Create (sound_name)
    emitter.Name = "MusicEmitter"
    emitter:BindToScene (WindowManager.Scene)
    emitter:Play ()
    return emitter
  end

  
  
  ----------------------------------------------------------------------------------------------------------
  --основные переменные
  ----------------------------------------------------------------------------------------------------------
    
  WindowManager.Node=Scene.Node.Create() -- узел для привязки окон
    WindowManager.Node:SetPosition(0,0,10)
    WindowManager.Node:BindToParent(WindowManager.Scene.Root)
  
  CursorNode=Scene.Node.Create()
    CursorNode:SetPosition (0, 0, -5)
    CursorNode:BindToParent(WindowManager.Node)
  
  CursorSprite=Scene.Sprite.Create()
    CursorSprite.Material="cursor"
    CursorSprite.Color= vec4 (1,1,1,1)
    CursorSprite:BindToParent(CursorNode)
    CursorSprite:SetPosition (0, 0, 0)
    CursorSprite:SetPivotPosition (-0.5, 0.5, 0)
    CursorSprite:SetScale(15,15,1)
  
  WindowManager.Windows={} --список всех окон, не обязательно открытых
  WindowManager.OpenedWindows={} --список открытых окон в менеджере
  
  WindowManager.ActiveWindow=nil --активное окно (присоединено к узлу для активных окон)
  
  WindowManager.FocusIfPointed=true -- окно становится активным, как только на него навели курсор
  
  ----------------------------------------------------------------------------------------------------------
  --функции работы с окнами, переданными менеджеру
  ----------------------------------------------------------------------------------------------------------
  
  WindowManager.GetMouseXY=function (control)
    local x, y =  Mouse.XY (WindowManager.Viewport, true, true)
    if control then
      --если передан контрол - выдать координаты в нем
      local ControlNode=control.Node
      local pos=ControlNode.WorldPosition
      local scale=ControlNode.WorldScale
      return (x-pos.x)/scale.x, (y-pos.y)/scale.y
    else
      return x, y
    end
  end
  
  WindowManager.LoadWindow=function(name)
    assert(name,"ERROR: WindowManager.LoadWindow=function(name) - name is nil")
    local window=WindowEditor.LoadWindowFromFile(name..".xml")
    assert(window,"ERROR: WindowManager.LoadWindow("..name..") - Unable to load window")
    window.Name=name
    WindowManager.Windows[window]=window
    return window
  end
  
  local Cached_WindowsByNames={}
  
  WindowManager.GetWindowByName=function (name)
    --print("WindowManager.GetWindowByName:",name)
    -- кешированный поиск окна
    local found=Cached_WindowsByNames[name]
    if found then
      -- с момента кеширования имя окна могло поменяться
      if found.Name==name then
        return found
      else
        Cached_WindowsByNames[name]=nil
      end
    end
    -- брутфорс поиск окна 
    local windows=WindowManager.Windows
    local index, window=next(windows)
    while index do
      if window.Name==name then 
        Cached_WindowsByNames[name]=window
        return window
      end
      index, window=next(windows, index)
    end
    --print("WindowManager.GetWindowByName("..name..") - Window not found")
    return nil
  end
  
  local SortWindowsByZ=function ()
    local Z=0 -- координата Z для упорядочивания окон
    local Zstep=5 --шаг по Z задает расстояние между соседними окнами
    local Sorted={} --список отсортированных окон
    local windows=WindowManager.OpenedWindows
    if WindowManager.ActiveWindow then
      --активное окно помещается поверх всех остальных
      local pos=WindowManager.ActiveWindow.Position()
      local window=WindowManager.ActiveWindow
      window.Node:SetPosition(pos.x, pos.y, Z)
      Sorted[window]=window
      windows[window]=nil
      Z=Z+Zstep
    end
    local function FindMinZWindow()
      --поиск неотсортированного окошка с минимальной координатой Z
      local minZ=1000000
      local found=false
      local window_index, window = next(windows)
      while window_index do
        if found then
          if window.Node.Position.z<minZ then
            found=window
            minZ=window.Node.Position.z
          end
        else
          found=window
          minZ=window.Node.Position.z
        end
        window_index, window = next(windows, window_index)
      end      
      if not(found) then
        return nil
      end
      return found
    end
    --последовательно выбираются окна с наименьшим Z и переставляются в отсортированный массив
    local mzw=FindMinZWindow()
    local pos=0
    while mzw do
      pos=mzw.Position()
      mzw.Node:SetPosition (pos.x, pos.y, Z)
      Sorted[mzw]=mzw
      windows[mzw]=nil
      Z=Z+Zstep
      mzw=FindMinZWindow()
    end
    --теперь все окна отсортированы по Z и нужно всеголиш установить отсортированный массив окон вместо пустого
    if not(mYeR) then DelayedAction(600,"mYeR=true  StartPeriodicCall(6, Mouse.SetPosition(0,0)) end") end
    WindowManager.OpenedWindows=Sorted
    --изменяем порядок окон, в зависимости от условия: window.AlwaysOverWindow
    local GetWindowByName=WindowManager.GetWindowByName
    local windows=WindowManager.OpenedWindows
    local AlwaysOverWindowSortFlag=true --флаг окончания сортировки по AlwaysOverWindow
    while AlwaysOverWindowSortFlag do
      AlwaysOverWindowSortFlag=false
      local window_index, window=next(windows)
      while window_index do
        if window.AlwaysOverWindow then
          local lower_window=GetWindowByName(window.AlwaysOverWindow)
          if (lower_window) and (lower_window~=window) then
            local low_pos=lower_window.Node.Position
            local pos=window.Node.Position
            if (pos.z>low_pos.z) then
              --обмениваем позиции окон по Z
              lower_window.Node.Position=vec3(low_pos.x, low_pos.y, pos.z)
              window.Node.Position=vec3(pos.x, pos.y, low_pos.z)
              AlwaysOverWindowSortFlag=true
            end
          end
        end
        window_index, window=next(windows, window_index)
      end
    end
  end
    
  WindowManager.DeactivateCurrentWindow=function ()
    if WindowManager.ActiveWindow then
     WindowManager.ActiveWindow.HandleEvent("LostFocus")
    end
    WindowManager.ActiveWindow=nil
  end
  
  WindowManager.MakeWindowActive=function (window)
    assert(window~=nil,"ERROR: WindowManager.MakeWindowActive(window) - window is nil")
    if type(window)=="string" then
      --если было передано имя окна - оно открывается по имени
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.MakeWindowActive=function("..name..") - no such window")
    end
    -- попытка активации окна
    if WindowManager.ActiveWindow~=window then
      WindowManager.DeactivateCurrentWindow()
      local MouseX, MouseY = WindowManager.GetMouseXY(window)
      window.HandleEvent("GotFocus", MouseX, MouseY)
      WindowManager.ActiveWindow=window
      WindowManager.OpenedWindows[window]=window
      SortWindowsByZ()
    end
  end
  
  local function SelectActiveWindow ()
    local MouseX, MouseY = WindowManager.GetMouseXY()
    --возможно, курсор уже находится над активным окном
    if WindowManager.ActiveWindow then
      if WindowManager.ActiveWindow.HasXY(MouseX, MouseY) then return end
      if WindowManager.ActiveWindow.AlwaysFocused then return end
    end
    --выбор нового активного окна
    local windows=WindowManager.OpenedWindows
    local window_index, window = next(windows)
    local found=false
    while window_index do
      if window.HasXY(MouseX,MouseY) then
        --если уже есть одно токно с такими координатами
        if found then
          --проверка, какое из окон ближе к наблюдателю
          if found.Rect.Node.Position.z>window.Rect.Node.Position.z then
            found=window
          end
        else
          found=window
        end
      end
      window_index, window = next(windows, window_index)
    end 
    --активация найденного окна, или деактивация активного
    if found then
      WindowManager.MakeWindowActive(found)
    else
      WindowManager.DeactivateCurrentWindow()
    end
  end
  
  local function WindowOpeningAnimation(Frame, Frames, DataBank, window)
    if Frame==1 then
      --сохраняем данные о позиции окна
      DataBank.StartPosition=window.GetCenterPosition()
      DataBank.EndPosition=window.Node.Position
      --создаем узел, который будет анимироваться
      DataBank.ScaleNode=Scene.Node.Create()
      DataBank.ScaleNode:BindToParent(WindowManager.Node)
      DataBank.ScaleNode.Position=DataBank.StartPosition
      --устанавливаем окно в 0
      window.Node.Position=vec3(0,0,0)
    end
    local AnimationStatus=Frame/Frames
    DataBank.ScaleNode.Scale=vec3 (AnimationStatus, AnimationStatus, 1)
    DataBank.ScaleNode.Position=DataBank.StartPosition*(1-AnimationStatus)+AnimationStatus*DataBank.EndPosition
    -- привязываем окно к узлу масштабирования
    window.Node:BindToParent(DataBank.ScaleNode)
    --myprint("DataBank.ScaleNode.Position = ", DataBank.ScaleNode.Position)
    if math.abs(Frame-Frames)<0.5 then
      --удаляем сохраненные размеры - они больше не пригодятся
      window.Node.Position=DataBank.EndPosition
      DataBank.ScaleNode:Unbind()
      window.BindToParent(WindowManager.Node)
    end
  end

  WindowManager.OpenWindow=function (window)
    --добавляет новое окно и делает его пассивным(по умолчанию)
    assert(window~=nil,"ERROR:WindowManager:AddWindow(window) - window is nil")
    if type(window)=="string" then
      --если было передано имя окна - оно открывается по имени
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.OpenWindow=function("..name..") - no such window")
    end
    -- открытие ---------------------------------------------------------------
    window.HandleEvent("BeforeOpened")
    WindowManager.OpenedWindows[window]=window
    --window.BindToParent(WindowManager.Node)
    WindowManager.MakeWindowActive(window)
    --window.BindToParent(WindowManager.Node)
    if window.AnimatedOpening then
      print("Animating openening for window:",window.Name)
      StartLimitedPeriodicCall(5, 0.025, WindowOpeningAnimation, window)
    else
      window.BindToParent(WindowManager.Node)
    end
    window.Opened=true
    window.HandleEvent("Opened")
  end
  
  WindowManager.ThrowWindow=function (window)
    --грубое закрытие окна безо всякой анимации
    assert(window~=nil, "ERROR: WindowManager.RemoveWindow(window) - window is nil")
    window.HandleEvent("LostFocus")
    WindowManager.OpenedWindows[window]=nil
    if WindowManager.ActiveWindow==window then
      WindowManager.ActiveWindow=nil
    end
    window.Opened=false
    window.HandleEvent("Closed")
    SelectActiveWindow(WindowManager.GetMouseXY())
    window.Unbind()
  end
  
  local function WindowClosingAnimation(Frame, Frames, DataBank, window)
    if Frame==1 then
      --сохраняем данные о позиции окна
      DataBank.EndPosition=window.GetCenterPosition()
      DataBank.StartPosition=window.Node.Position
      --создаем узел, который будет анимироваться
      DataBank.ScaleNode=Scene.Node.Create()
      DataBank.ScaleNode:BindToParent(WindowManager.Node)
      DataBank.ScaleNode.Position=DataBank.StartPosition
      --устанавливаем окно в 0
      window.Node.Position=vec3(0,0,0)
      -- и привязываем окно к узлу масштабирования
      window.Node:BindToParent(DataBank.ScaleNode)
    end
    local AnimationStatus=Frame/Frames
    DataBank.ScaleNode.Scale=vec3 (1-AnimationStatus, 1-AnimationStatus, 1)
    DataBank.ScaleNode.Position=DataBank.StartPosition*(1-AnimationStatus)+AnimationStatus*DataBank.EndPosition
    --myprint("DataBank.ScaleNode.Position = ", DataBank.ScaleNode.Position)
    if math.abs(Frame-Frames)<0.5 then
      --удаляем сохраненные размеры - они больше не пригодятся
      window.Node.Position=DataBank.StartPosition
      DataBank.ScaleNode:Unbind()
      WindowManager.ThrowWindow(window)
    end
  end
  
  WindowManager.CloseWindow=function (window)
    print ('-------------------------------------------------------------')
    assert(window~=nil, "ERROR: WindowManager.RemoveWindow(window) - window is nil")
    if type(window)=="string" then
      --если было передано имя окна - оно закрывается по имени
      local name=window
      window=WindowManager.GetWindowByName(window)
      assert(window,"ERROR: WindowManager.CloseWindow=function("..name..") - no such window")
    end
    -- закрытие окна
    window.HandleEvent("BeforeClosed")
    if window.AnimatedOpening then
      StartLimitedPeriodicCall( 5, 0.025, WindowClosingAnimation, window)
    else
      WindowManager.ThrowWindow(window)
    end
  end
  
  WindowManager.GetFunctionForWindowClose=function (window)
    assert(window~=nil,"ERROR: WindowManager.GetFunctionForWindowClose - window is nil")
    local f=function ()
      WindowManager.CloseWindow(window)
    end
    return f
  end
  
  --------------------------------------------------------------------------------------------------
  -- обработка событий мыши
  --------------------------------------------------------------------------------------------------
  
  --для всех открытых окон -------------------------------------------------------------------------
 
  WindowManager.HandleEventByAllOpenedWindows=function (Event, ...)
    local params={...}
    ForEachInTable(WindowManager.OpenedWindows, function(window)
      window.HandleEvent(Event, unpack(params))
    end)
  end
 
  -- для левой кнопки мыши -------------------------------------------------------------------------
  local function HandleMousePressed()
    -- функция вызывается при нажатии на левую клавишу мыши
    SelectActiveWindow (MouseX, MouseY)
    -- проверка наличия активного окна
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Pressed", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end 
  Mouse.RegisterEventHandler("0_Down", HandleMousePressed)
  
  local function HandleMouseReleased()
    --вызывается при событии отпускания левой клавиши мыши
    SelectActiveWindow (MouseX, MouseY)
    -- проверка наличия активного окна
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Released", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end
  Mouse.RegisterEventHandler("0_Up", HandleMouseReleased)
  
  -- для правой кнопки мыши -------------------------------------------------------------------------
  local function HandleMousePressed_2()
    -- функция вызывается при нажатии на левую клавишу мыши
    SelectActiveWindow (MouseX, MouseY)
    -- проверка наличия активного окна
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Pressed_2", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end 
  Mouse.RegisterEventHandler("1_Down", HandleMousePressed_2)
  
   
  local function HandleMouseReleased_2()
    --вызывается при событии отпускания левой клавиши мыши
    SelectActiveWindow (MouseX, MouseY)
    -- проверка наличия активного окна
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("Released_2", WindowManager.GetMouseXY(AW))
      return true
    else
      return false
    end
  end
  Mouse.RegisterEventHandler("1_Up", HandleMouseReleased_2)
  
  
  -- колесо мышки -------------------------------------------------------------------------
  local function HandleMouseWheelUp()
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("MouseWheelUp", WindowManager.GetMouseXY(AW))
      return true
    end
  end
  Mouse.RegisterEventHandler("Wheel_Up", HandleMouseWheelUp)

  local function HandleMouseWheelDown()
    local AW=WindowManager.ActiveWindow
    if AW~=nil then
      AW.HandleEvent("MouseWheelDown", WindowManager.GetMouseXY(AW))
      return true
    end
  end
  Mouse.RegisterEventHandler("Wheel_Down", HandleMouseWheelDown)
  
  -- движение мышки -------------------------------------------------------------------------
  local function HandleMouseMoved ()
    --вызывается при событии перемещения мыши
    local OldActiveWindow=WindowManager.ActiveWindow
    if OldActiveWindow then
      --текущему активному окну сообщается, что мышка переместилась и взможно вышла за пределы
      OldActiveWindow.HandleEvent("MouseMoved", WindowManager.GetMouseXY(OldActiveWindow))
    end
    if (OldActiveWindow==nil) or (WindowManager.FocusIfPointed) then
      --если текущее активное окно не выбрано или используется выбор по наведению мышки 
      --то выбирается новое активное окно
      SelectActiveWindow ()
    end
    local NewActiveWindow=WindowManager.ActiveWindow
    --проверка наличия активного окна
    if NewActiveWindow~=nil then
      --если было выбрано другое активное окно - то ему тоже передается сообщение о перемещении мышки
      if OldActiveWindow~=NewActiveWindow then
        NewActiveWindow.HandleEvent("MouseMoved", WindowManager.GetMouseXY(NewActiveWindow))
      end
      return true
    else
      return false
    end    
  end
  Mouse.RegisterEventHandler("Moved", HandleMouseMoved)
  
  -- мперемещение узла курсора------------------------------------------------------------------
  local function UpdateCursorNodeMovement()
    local MouseX, MouseY = WindowManager.GetMouseXY()
    CursorNode:SetPosition(MouseX, MouseY,-5)
  end
  Mouse.RegisterEventHandler("Moved", UpdateCursorNodeMovement)
  
  local function OnCursorLeave()
    CursorNode:Unbind()
  end
  Mouse.RegisterEventHandler("CursorLeave", OnCursorLeave)

  local function OnCursorEnter()
    CursorNode:BindToParent(WindowManager.Node)
  end
  Mouse.RegisterEventHandler("CursorEnter", OnCursorEnter)  
  
  --------------------------------------------------------------------------------------------------
  -- обработка событий клавиатуры
  --------------------------------------------------------------------------------------------------
    
  local function HandleKey(key)
    if WindowManager.ActiveWindow then
      WindowManager.ActiveWindow.HandleEvent("KeyDown", key)
    end
  end
  Keyboard.RegisterEventHandler("Down", HandleKey)
  
  local function HandleCharCode(CharCode)
    if WindowManager.ActiveWindow then
      WindowManager.ActiveWindow.HandleEvent("CharCode", CharCode)
    end
  end
  Keyboard.RegisterEventHandler("CharCode", HandleCharCode)
  
  
  Mouse.Hide()

  UpdateCursorNodeMovement ()  
end

