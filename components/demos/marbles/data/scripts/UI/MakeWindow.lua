
--[[
Методы:

window.Update ()
  нужно вызывать после любых изменений стиля, размера или надписи на окне
  
window.UpdateState (MouseX, MouseY, click)
  обновляет состояние кнопок и других элементов окна, а также отрабатывает нажатие на них

]]



function UI.MakeWindow(control)
  assert(control, "ERROR: MakeWindow(control) - control is nil")

  local window=control
  
  window.ControlType="Window"

  window.Controls={}       --массив кнопок окна
  
  
  window.ActiveControl=nil --активная кнопка окна(та, над которой в данный момент мышка)
  
  if not(window.CaptionAlignment) then 
    window.CaptionAlignment=vec3( 0, 0, 0)
  end
  
  window.Moving=false -- включается, когда окно перетаскивают
  window.Movable=true -- если установить в false то окно будет невозможно перетащить
  
  --------------------------------------------------------------------------------------------------
  -- основные функции
  --------------------------------------------------------------------------------------------------
  
  local MouseX, MouseY = 0, 0 --хранят координаты мыши над окном
  
  local IdCounter=0 --счетчик id для добавляемых элементов
  
  --------------------------------------------------------------------------------------------------
  -- добавление и удаление контролов
  --------------------------------------------------------------------------------------------------
  
  window.AddControl=function (control)
    --используется для добавления кнопок, полей ввода, слайдеров и т.д.
    assert(control~=nil,"ERROR: window.AddControl(control) - control is nil")
    window.Controls[control]=control
    control.id=IdCounter
    IdCounter=IdCounter+1
    control.BindToParent(window.ForeNode)
    control.Parent=window
  end
  
  window.RemoveControl=function (control)
    assert(control~=nil, "ERROR: window.RemoveControl(control) - button is nil")
    if window.Controls[control]==nil then return end
    window.Controls[control]=nil
    control.Unbind()
    if window.ActiveControl==control then
      window.ActiveControl=nil
    end
  end
  
  window.RemoveControlByName=function (name)
    window.RemoveControl(window.GetControlByName(name))
  end
  
  window.RemoveAllControls=function ()
    local controls=window.Controls
    local index, control=next(controls)
    while index do
      window.RemoveControl(control)
      index, control = next(controls, index)
    end
    window.Controls={}
  end
  
  window.AddControls=function (controls)
    local index, control=next(controls)
    while index do
      window.AddControl(control)
      index, control = next(controls, index)
    end
  end
  
  window.HideControl=function(control)
    if control then
      assert(window.Controls[control],"ERROR: "..window.Name..".HideControl() - control is not from this window")
      control.Hidden=true
      control.Unbind()
      if window.ActiveControl==control then
        window.ActiveControl=nil
      end
    end
  end
  
  window.HideControlByName=function (name)
    local control=window.GetControlByName(name)
    if control then
      window.HideControl(control)
    end
  end
  
  window.ShowControl=function(control)
    if control then
      assert(window.Controls[control],"ERROR: "..window.Name..".ShowControl() - control is not from this window")
      control.Hidden=false
      control.BindToParent(window.ForeNode)
    end
  end
  
  window.ShowControlByName=function (name)
    local control=window.GetControlByName(name)
    if control then
      window.ShowControl(control)
    end
  end
  
  --------------------------------------------------------------------------------------------------
  -- выделение контролов
  --------------------------------------------------------------------------------------------------

  local GetMousePositionForControl=WindowManager.GetMouseXY
  
  window.SetControlAsActive=function (control)
    if control then
      if window.ActiveControl~=control then
        if window.ActiveControl then
          window.ActiveControl.HandleEvent("LostFocus", GetMousePositionForControl (window.ActiveControl))
        end
        window.ActiveControl=control
        control.HandleEvent("GotFocus", GetMousePositionForControl (control))
      end
    end
  end
  
  window.SelectActiveControlByMouse=function ()
    -- проверка, возможно уже выбрана активная кнопка
    if window.ActiveControl~=nil then
      if window.ActiveControl.HasXY(MouseX, MouseY) then 
        return 
      else
        window.ActiveControl.HandleEvent("LostFocus", GetMousePositionForControl (window.ActiveControl))
        window.ActiveControl=nil
      end
    end
    -- обновляет состояние всех кнопок, ищет новую активную
    local controls=window.Controls
    local control_index, control = next(controls)
    while control_index do
      if control.HasXY(MouseX, MouseY) then
        if not(control.Hidden) then
          window.SetControlAsActive(control)
          return
        end
      end
      control_index, control = next(controls, control_index)
    end
  end
  
  window.GetControlByName=function (name)
    local controls=window.Controls
    local control_index, control = next(controls)
    while control_index do
      if control.Name==name then
        return control
      end
      control_index, control = next(controls, control_index)
    end
    return nil
  end
  
  window.SelectActiveControlByName=function (name)
    local control=window.GetControlByName(name)
    if control then
      window.SetControlAsActive(control, 0, 0)
    end
  end
  
  window.GetRelativeControl=function(X, Y, DirectionPriority, start_control)
    --[[выбирает ближайший контрол относительно заданного контрола start_control или относительно (0,0)
    X и Y задают направдение поиска соседнего контрола, например:
    (-1,0) - поиск контрола слева, (1,1) -поиск контрола справа внизу
    ]]
      
    local found=false --найденный контрол
      
    local function GetRelative()
      local start=vec3(0,0,0) -- стартовая позиция
        if start_control then start=start_control.GetCenterPosition() end
      local controls=window.Controls
      local control_index, control = next(controls)
      local dpos=vec3(0,0,0)
      local MinF=false --при нахождени контрола будет происходить минимизация функции отклонения от стартового
      local F=0
      while control_index do
        if (control~=start_control) and (control.ArrowSelectable) then
          F=0
          dpos=(control.GetCenterPosition()) - start
          F=(dpos.x*X)+(dpos.y*Y)
          if F>0 then
            --myprint("DirectionBonus= ", ((-F)*DirectionPriority), "  DistancePenalty= ", (dpos.length*(1-DirectionPriority)))
            dpos=dpos:abs()
            F=((-F)*DirectionPriority)+((dpos.x+dpos.y)*(1-DirectionPriority))
            --myprint(control.Name, " F= ", F)
            if MinF then
              if F<MinF then
                found=control
                MinF=F
              end
            else
              found=control
              MinF=F
            end
          end
        end
        control_index, control = next(controls, control_index)
      end
    end
    
    GetRelative()
    if not(found) then
      --если далее в этом направлении нет контролов, 
      --ищется крайний контрол в противоположном направлении
      X, Y, DirectionPriority = -X, -Y, 1
      GetRelative()
    end
    return found
  end
  
  window.RedrawAllControls=function ()
    local controls=window.Controls
    local control_index, control = next(controls)
    while control_index do
      control.Redraw()
      control_index, control = next(controls, control_index)
    end
  end
  
  --------------------------------------------------------------------------------------------------
  -- обработка событий
  --------------------------------------------------------------------------------------------------
  local OnOpened=function ()
    window.RedrawAllControls()
    window.Redraw()
  end
  window.RegisterEventHandler ("Opened", OnOpened)
  
  local OnGotFocus=function (Mouse_X, Mouse_Y)
    MouseX, MouseY = Mouse_X, Mouse_Y
    --сообщение всем контролам, что получен фокус
    local controls=window.Controls
    local control_index, control = next(controls)
    while control_index do
      control.HandleEvent("ParentGotFocus", GetMousePositionForControl (control))
      control_index, control = next(controls, control_index)
    end
    --поиск новой активной кнопки
    window.SelectActiveControlByMouse (MouseX, MouseY)
  end
  window.RegisterEventHandler ("GotFocus", OnGotFocus)
  
  
  local OnLostFocus=function ()
    -- все кнопки сновятся пассивными
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("LostFocus", GetMousePositionForControl (control))
      window.ActiveControl=nil
    end
    local controls=window.Controls
    local control_index, control = next(controls)
    while control_index do
      control.HandleEvent("ParentLostFocus", GetMousePositionForControl (control))
      control_index, control = next(controls, control_index)
    end
    -- режим перетаскивания отключается
    window.Moving=false
  end
  window.RegisterEventHandler ("LostFocus", OnLostFocus)
  
  --------------------------------------------------------------------------------------------------
  -- обработка событий мышки
  --------------------------------------------------------------------------------------------------
  
  -- левая кнопка мыши -----------------------------------------------------------------------------
  local OnPressed=function ()
    if window.ActiveControl==nil then
      window.SelectActiveControlByMouse (MouseX, MouseY)
    end
    if window.ActiveControl then
      --если нажата активная кнопка окна
      window.ActiveControl.HandleEvent("Pressed", GetMousePositionForControl (window.ActiveControl))
    else
      --если ни одна кнопка не была нажата, пытаемся перетащить окно
      if window.Movable then
        local pos=window.Rect.Position()
        window.Moving={}
        window.Moving.MouseX=MouseX
        window.Moving.MouseY=MouseY
      end
    end
  end
  window.RegisterEventHandler ("Pressed", OnPressed)
  
  
  local OnReleased=function ()
    window.Moving=nil
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("Released", GetMousePositionForControl (window.ActiveControl))
    end
  end
  window.RegisterEventHandler ("Released", OnReleased)
  
  
  -- правая кнопка мыши -----------------------------------------------------------------------------
  local OnPressed_2=function ()
    if window.ActiveControl==nil then
      window.SelectActiveControlByMouse (MouseX, MouseY)
    end
    if window.ActiveControl then
      --если нажата активная кнопка окна
      window.ActiveControl.HandleEvent("Pressed_2", GetMousePositionForControl (window.ActiveControl))
    end
  end
  window.RegisterEventHandler ("Pressed_2", OnPressed_2)
  
  
  local OnReleased_2=function ()
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("Released_2", GetMousePositionForControl (window.ActiveControl))
    end
  end
  window.RegisterEventHandler ("Released_2", OnReleased_2)
  
  -- колесо мыши ----------------------------------------------------------------------------------
  local OnMouseWheelUp=function ()
    window.Moving=nil
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("MouseWheelUp", GetMousePositionForControl (window.ActiveControl))
    end
  end
  window.RegisterEventHandler ("MouseWheelUp", OnMouseWheelUp)
  
  local OnMouseWheelDown=function ()
    window.Moving=nil
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("MouseWheelDown", GetMousePositionForControl (window.ActiveControl))
    end
  end
  window.RegisterEventHandler ("MouseWheelDown", OnMouseWheelDown)
  
  local OnMouseMoved=function (Mouse_X, Mouse_Y)
    MouseX, MouseY = Mouse_X, Mouse_Y
    -- если идет перетаскивание окна
    if window.Moving then
      local pos=window.Position()
      window.Node:SetPosition (pos.x+(MouseX-window.Moving.MouseX), pos.y+(MouseY-window.Moving.MouseY), 0)
    else
      --определение активной кнопки окна
      window.SelectActiveControlByMouse (MouseX, MouseY)
      if window.ActiveControl then
        window.ActiveControl.HandleEvent("MouseMoved", GetMousePositionForControl (window.ActiveControl))
      end
    end
  end
  window.RegisterEventHandler ("MouseMoved", OnMouseMoved)
  
  --------------------------------------------------------------------------------------------------
  -- обработка событий клавиатуры
  --------------------------------------------------------------------------------------------------
  
  local OnKeyDown=function(key)
    -- если нажата одна из стрелок на клавиатуре
    if window.SelectControlsByKeys then
      local FoundControl=false
      if key=="ArrowUp"    then FoundControl=window.GetRelativeControl( 0, 1, 0.4, window.ActiveControl)  end
      if key=="ArrowDown"  then FoundControl=window.GetRelativeControl( 0, -1, 0.4, window.ActiveControl) end
      if key=="ArrowLeft"  then FoundControl=window.GetRelativeControl( -1, 0, 0.4, window.ActiveControl) end
      if key=="ArrowRight" then FoundControl=window.GetRelativeControl( 1, 0, 0.4, window.ActiveControl)  end
      if FoundControl then 
        window.SetControlAsActive(FoundControl) 
        return
      end
    end
    -- для всех остальных кнопок клавиатуры
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("KeyDown", key)
    end
  end
  window.RegisterEventHandler("KeyDown", OnKeyDown)
  
  local OnChar=function(CharCode)
    if window.ActiveControl then
      window.ActiveControl.HandleEvent("CharCode", CharCode)
    end
  end
  window.RegisterEventHandler("CharCode", OnChar)
  
  
  return window
end

