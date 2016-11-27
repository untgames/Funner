local function universal_unpack (variable)
  if (type (variable) == "table") then
    return unpack (variable, 0, #variable)
  else
    return variable
  end      
end

local function copy_animation_handlers (source)
  if not source then return {} end
  
  local result = {}
  
  for i = 1, #source, 1 do
    result [i] = {}
    
    result [i].handler = source [i].handler
    result [i].args    = source [i].args
  end
  
  return result
end

LinearAnimation = {}
LinearAnimation.__index = LinearAnimation

function LinearAnimation.Create (animation_config)
  local animation = {}
  
  setmetatable (animation, LinearAnimation)
  
  animation.object                       = animation_config.object                                              --анимируемый узел
  animation.start_value                  = animation_config.start_value                                         --начальное значение свойства объекта
  animation.animation_direction          = animation_config.target_value - animation_config.start_value         --направление изменения значения свойства объекта
  animation.delay                        = animation_config.delay                                               --задержка перед началом анимации
  animation.animation_time               = animation_config.animation_time                                      --время анимации
  animation.ellapsed_time                = 0                                                                    --прошедшее время
  animation.property_set_function        = animation_config.property_set_function                               --функция установки свойства объекта
  animation.animation_begin_handlers     = copy_animation_handlers (animation_config.animation_begin_handlers)  --обработчики, вызываемый при первом срабатывании
  animation.animation_end_handlers       = copy_animation_handlers (animation_config.animation_end_handlers)    --обработчики окончания анимации
  animation.playing                      = true                                                                 --проигрывается ли анимация

  if not animation.delay then
    animation.delay = 0
  end
    
  return animation
end

function LinearAnimation:Process (dt)
   if not self.playing then
     return
   end

   if (self.delay > 0.0001) then
     if (self.delay > dt) then
       self.delay = self.delay - dt
       return
     else
       dt = dt - self.delay
       self.delay = 0       
     end
   end

   for i = 1, #self.animation_begin_handlers, 1 do
     local current_begin_handler = self.animation_begin_handlers [i]
   
     current_begin_handler.handler (self, universal_unpack (current_begin_handler.args))
     
     if (i == #self.animation_begin_handlers) then
       self.animation_begin_handlers = {}
     end
   end

   self.ellapsed_time = self.ellapsed_time + dt
   
   if (self.ellapsed_time > (self.animation_time)) then
     self.ellapsed_time = self.animation_time
     self.playing       = false
   end  
   
   self.property_set_function (self.object, self.start_value + self.animation_direction * (self.ellapsed_time / self.animation_time))

   if not self.playing then
     for i = 1, #self.animation_end_handlers, 1 do
       local current_end_handler = self.animation_end_handlers [i]
   
       current_end_handler.handler (self, universal_unpack (current_end_handler.args))
     
       if (i == #self.animation_end_handlers) then
         self.animation_end_handlers = {}
       end
     end
   end  
end
