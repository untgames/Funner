--[[
создает глобальный объект "Mouse"
в файле конфигурации нужно установить имя курсора "cursor"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Доступные методы:

Mouse.RegisterEventHandler(event, handler) - регистрация обработчика событий мышки

Mouse.UnregisterEventHandler(event, handler) - отмена регистрации

Mouse.XY (ViewPort, UseCameraView) 
  получение координат мыши. по умолчанию возвращаются координаты в окне.
  если передан ViewPort - координаты во вьюпорте в диапазоне 0..100. если курсор за вьюпортом - координаты вне его
  если UseCameraVieW==true - возвращаются координаты с учетом параметров ортографической камеры порта
  
Mouse.SetXY (x, y, ViewPort, UseCameraView)
  установка координат мыши.
  если передан ViewPort - устанавливается в порте
  если UseCameraView==true - устанавливается в координатах ортографической камеры этого порта
  
]]

--[[
  --тест для проверки отключения обработки мышью событий от устройств ввода
  --в таком режиме мышь может управляться только программно
  
  function ChangeMouseDriving()
    if Mouse.DrivedByInput then
      Mouse.DrivedByInput=false
      print("-- Mouse: OFF")
    else
      Mouse.DrivedByInput=true
      print("-- Mouse: ON")
    end
    DelayedAction(2, ChangeMouseDriving)
  end
  
  ChangeMouseDriving()
]]


function InitMouse()
  Mouse={}
    
  --------------------------------------------------------------------------------------------------
  -- обработка событий
  --------------------------------------------------------------------------------------------------
  
  local handler=Input.CreateEventHandler()
  Engine.InputEventHandlers.Register("Input", handler)

  assert(AddEventManager~=nil, "ERROR: InitMouse() - AddEventManager() is nil")
  AddEventManager(Mouse)
  
  local MovementTimer=Common.Timer.Create() --этот таймер нужен, чтобы события о пермещении не обрабатывались слишком часто
        MovementTimer:Reset()
  
  Mouse.DrivedByInput=true --мышь может управляется извне программы, сообщениями от устройства ввода
  
  Mouse.HandleInputEvent=function ( Event, ...)
    if (Event=="Moved") then
      if (MovementTimer.SecondsEllapsed<0.01) then
        return
      else
        MovementTimer:Reset()
      end
    end
    if Mouse.DrivedByInput then
      Mouse.HandleEvent( Event, ...)
    end
  end
  
  
  --------------------------------------------------------------------------------------------------
  -- основные функции
  --------------------------------------------------------------------------------------------------
  
  local cursor=Input.Cursor.Create()
  Engine.Cursors.Register("cursor", cursor)  
  
  Mouse.XY=function (ViewPort, UseCameraView, InWorldCoordinates)
    local Pos=cursor.Position
    if ViewPort==nil then
      --выдача экранных координат мыши
      return Pos.x, Pos.y
    else
      --расчет координат мыши внутри вьюпорта
      local x, y = Pos.x*100, Pos.y*100
      local area=ViewPort.Area
      x=x-area.Left
      y=y-area.Top
      x=x/area.Width
      y=y/area.Height
      if UseCameraView then
        --расчет координат мыши внутри камеры
        if ViewPort.Camera==nil then 
          print("ViewPort.Camera==nil")
          return
        end
        local camera=ViewPort.Camera
        x=x*(camera.Right-camera.Left)+camera.Left
        y=y*(camera.Bottom-camera.Top)+camera.Top
        if InWorldCoordinates then
          --расчет мировых координат мыши
          x=x+camera.WorldPosition.x
          y=y+camera.WorldPosition.y
        end
      end
      return x, y
    end
  end

  
  
  Mouse.SetXY=function (x, y, ViewPort, UseCameraView)
    if ViewPort==nil then
      --установка экранных координат мыши
      cursor:SetPosition (x, y)
    else
      if UseCameraView then
        --расчет целевых координат с учетом параметров камеры въюпорта
        assert(ViewPort.Camera~=nil,"Mouse.SetXY (x, y, ViewPort, UseCameraView) - no Camera to use it")
        x=(x-ViewPort.Camera.Left)/(ViewPort.Camera.Right-ViewPort.Camera.Left)
        y=(y-ViewPort.Camera.Top)/(ViewPort.Camera.Bottom-ViewPort.Camera.Top)
      end
      --расчет целевых координат на экране с учетом положения въюпорта на нем
      x=(x*ViewPort.Area.Width+ViewPort.Area.Left)*0.01
      y=(y*ViewPort.Area.Height+ViewPort.Area.Top)*0.01
      cursor:SetPosition (x, y)
    end
    Mouse.HandleEvent('Moved')
  end

  
  
  Mouse.Hide=function ()
    cursor:Hide()
  end
  
  Mouse.Show=function ()
    cursor:Show()
  end
  
  --------------------------------------------------------------------------------------------------
  -- дополнительные фичи
  --------------------------------------------------------------------------------------------------
  
  Mouse.MoveToPosition=function (x,y, MovementTime)
    --перемещает курсор мышы в заданную позицию за азданное время
    assert(MovementTime, "ERROR: Mouse.MoveToPosition(x,y,MovementTime) - Delay is nil")
    local startX,startY=Mouse.XY()
    local Movement={}
    local MovementTimer=Common.Timer.Create()
    local function MoveMouse()
      local t=MovementTimer.SecondsEllapsed/MovementTime
      if t>=1 then
        Mouse.SetXY(x,y)
        return false
      end
      Mouse.SetXY(startX*(1-t)+x*t, startY*(1-t)+y*t)
    end
    Movement=StartPeriodicCall(0.04, MoveMouse)
  end
  
end

