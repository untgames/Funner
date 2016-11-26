------------------------------------------------------------------------------------------------------------------------------------
-- вспомогательные функции для создания контрола
------------------------------------------------------------------------------------------------------------------------------------
local function AddStyleChangingHandler(control, Event, StyleName)
  assert(control, "ERROR:  AddStyleChangingHandler(control, Event, StyleName) - control is nil")
  assert(Event, "ERROR: AddStyleChangingHandler(control, Event, StyleName) - Event is nil")
  local function ChangeStyleByEvent()
    control.SetStyle(StyleName)
  end
  control.RegisterEventHandler(Event, ChangeStyleByEvent)
end

------------------------------------------------------------------------------------------------------------------------------------
-- создание контрола
------------------------------------------------------------------------------------------------------------------------------------


function CreateControl ()
  local control={}
  
  control.Name=name
  
  --- графические примитивы контрола -------------------------------------------------------
  
  control.Rect=CreateWinRect(10, 10)
  
  control.Node=control.Rect.Node
  
  control.ForeNode=Scene.Node.Create() --используется для привязки чуть выше уровня контрола
    control.ForeNode:BindToParent(control.Rect.Node)
    control.ForeNode:SetPosition(0,0,-1)
    control.ForeNode:SetScale(1,1, (1/128))
    
  --- внешний вид контрола ------------------------------------------------------------------
  
  control.StyleSetName="Control"
  
  control.Styles={}
  
  control.Style=nil
    
  --- поведение контрола --------------------------------------------------------------------
    
  control.ControlType="Control"
  
  control.ArrowSelectable=true
  
  control.ParentFocused=false
  
  control.Focused=false
  
  --- дополнительные данные -----------------------------------------------------------------
  
  control.Positions={}--массив специальных позиций
  
  control.Sprites={}  --массив специальных спрайтов
  
  --------------------------------------------------------------------------------------------------
  -- диспетчер событий
  --------------------------------------------------------------------------------------------------
  
  assert(AddEventManager~=nil, "ERROR: CreateControl () - AddEventManager() is nil")
  AddEventManager(control)
 
  --------------------------------------------------------------------------------------------------
  -- основные функции
  --------------------------------------------------------------------------------------------------
            
  control.SetWidthAndHeight =control.Rect.SetWidthAndHeight
  control.Position          =control.Rect.Position
  control.GetCenterPosition    =control.Rect.GetCenterPosition
  control.WorldPosition     =control.Rect.WorldPosition
  control.BindToParent =control.Rect.BindToParent
  control.Unbind       =control.Rect.Unbind
  control.SetScale     =control.Rect.SetScale
  control.HasXY        =control.Rect.HasXY
  control.HasWorldXY   =control.Rect.HasWorldXY

  
  control.SetStyle=CreateMethodForObject(UI.Control_SetStyle, control)
  
  control.SetStyles=CreateMethodForObject(UI.Control_SetStyles, control)
  
  control.SetSpecialMaterial=CreateMethodForObject(UI.Control_SetSpecialMaterial, control)
  
  control.ShowSprite=CreateMethodForObject(UI.Control_ShowSprite, control)
  
  control.HideSprite=CreateMethodForObject(UI.Control_HideSprite, control)
  
  --------------------------------------------------------------------------------------------------
  -- добавление надписи
  --------------------------------------------------------------------------------------------------
  
  control.Redraw=CreateMethodForObject(UI.Control_Redraw, control)
  
  control.SetCaption=CreateMethodForObject(UI.Control_SetCaption, control)
  
  --------------------------------------------------------------------------------------------------
  -- работа с XML (сохранение и загрузка)
  --------------------------------------------------------------------------------------------------  
  
  control.SetControlByXML=CreateMethodForObject(UI.Control_SetByXML, control)
  
  --------------------------------------------------------------------------------------------------
  -- обработчики событий
  --------------------------------------------------------------------------------------------------
  
  -- изменение стиля по событиям -----------------------------------------------------------
  AddStyleChangingHandler(control, "GotFocus", "Focused")
  AddStyleChangingHandler(control, "LostFocus", "Default")
  AddStyleChangingHandler(control, "ParentGotFocus", "Default")
  AddStyleChangingHandler(control, "ParentLostFocus","Default")
  AddStyleChangingHandler(control, "Pressed", "Pressed")
  AddStyleChangingHandler(control, "Released", "Focused")
  
  -- включение и отключение анимации вложенных спрайтов -------------------------------------
  control.RegisterEventHandler("GotFocus", function () 
    control.Focused=true 
    UI.Control_AnimateSprites(control, true)
  end)
  
  control.RegisterEventHandler("LostFocus",function () 
    control.Focused=false 
    UI.Control_AnimateSprites(control, false)
  end)
  
  control.RegisterEventHandler("ParentGotFocus", function () 
    control.ParentFocused=true 
    UI.Control_AnimateSprites(control, true)
  end)
  
  control.RegisterEventHandler("ParentLostFocus",function () 
    control.ParentFocused=false 
    UI.Control_AnimateSprites(control, false)
  end)
  
  -- автоматическая загрузка и выгрузка ресурсов ---------------------------------------------
  control.RegisterEventHandler("BeforeOpened",UI.Control_LoadResources, control)
  control.RegisterEventHandler("Closed", UI.Control_UnloadResources, control )
  
  return control
end

