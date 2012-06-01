BallsMinigameState = BallsMinigameState or {}

-- Параметры поля
BallsMinigameState.GAME_FIELD_CELLS_IN_ROW    = 5
BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN = 15

local WORLD_WIDTH  = 800
local WORLD_HEIGHT = 600
local BALL_WORLD_X_RADIUS = 12
local BALL_WORLD_Y_RADIUS = BALL_WORLD_X_RADIUS

local CELL_WIDTH                 = 0.04
local CELL_HEIGHT                = CELL_WIDTH
local CELL_HORIZONTAL_INDENT     = 0.01
local CELL_VERTICAL_INDENT       = 0

local SELECTION_WIDTH  = 0.053
local SELECTION_HEIGHT = 0.053
local SELECTION_OFFSET = vec3 (-0.002, 0.002, 0)

local GAME_FIELD_DEPTH           = 5
local BALL_DEPTH_OFFSET          = -1
local SELECTION_DEPTH_OFFSET     = -110
local PARTICLE_NODE_DEPTH_OFFSET = -3

--Фаза обработки движения мыши
local MOUSE_INPUT_PHASE_IDLE                   = 0                                              --мышь не обрабатывается
local MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE = MOUSE_INPUT_PHASE_IDLE + 1                     --ожидание движения по одной из осей
local MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT     = MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE + 1   --перерыв для определения по какой из осей производить движение
local MOUSE_INPUT_PHASE_MOVE                   = MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT + 1       --ввод непосредственно передвигает шарики 

local MOUSE_INPUT_DIRECTION_DETECTION_TIMEOUT = 0.05
local MOUSE_INPUT_X_SCALE = 600
local MOUSE_INPUT_Y_SCALE = MOUSE_INPUT_X_SCALE

-- Типы шаров
local NO_BALL 	   = 0
local RED_BALL     = NO_BALL + 1
local GREEN_BALL   = RED_BALL + 1
local BLUE_BALL    = GREEN_BALL + 1
local YELLOW_BALL  = BLUE_BALL + 1
local BLACK_BALL   = YELLOW_BALL + 1
local COLORS_COUNT = BLACK_BALL

local BALLS_DEPTH_WINDOW = 1

--Информация о типах шаров
local BallInfo = 
{
  [RED_BALL] = 
  {
    Material = "Minigame_Balls_FireBall",
    Offset   = -(BALLS_DEPTH_WINDOW / COLORS_COUNT) * RED_BALL
  },
  
  [GREEN_BALL] = 
  {
    Material = "Minigame_Balls_NatureBall",
    Offset   = -(BALLS_DEPTH_WINDOW / COLORS_COUNT) * GREEN_BALL
  },
  
  [BLUE_BALL] =
  {
    Material = "Minigame_Balls_MoonBall",
    Offset   = -(BALLS_DEPTH_WINDOW / COLORS_COUNT) * BLUE_BALL
  },
  
  [YELLOW_BALL] =
  {
    Material = "Minigame_Balls_AnimalBall",
    Offset   = -(BALLS_DEPTH_WINDOW / COLORS_COUNT) * YELLOW_BALL
  },
  
  [BLACK_BALL] =
  {
    Material = "Minigame_Balls_StarBall",
    Offset   = -(BALLS_DEPTH_WINDOW / COLORS_COUNT) * BLACK_BALL
  }
}

--Звуки
local MOVE_SOUND              = "Move"
local CANT_MOVE_SOUND         = "Can't Move"
local MATCH_THREE_BALLS_SOUND = "Match 3 Balls"

--Параметры анимации
local BALL_STABILIZATION_ANIMATION_DURATION = 0.05
local BALL_MOVE_ANIMATION_DURATION          = 0.1
local BALL_MOVE_ANIMATION_X_RANGE           = 1 + CELL_HORIZONTAL_INDENT / CELL_WIDTH
local BALL_MOVE_ANIMATION_Y_RANGE           = 1 + CELL_VERTICAL_INDENT / CELL_HEIGHT
local BALL_REMOVE_ANIMATION_DURATION        = 0.15
local BALL_REMOVE_ANIMATION_DELAY           = 0.1

local EPSILON = 0.001

local AXIS_DEAD_ZONE = 0.25

local DEFAULT_MINIMUM_BALLS_IN_COLUMN = 5

local MIDDLE_COLUMN           = 2
BallsMinigameState.MIDDLE_ROW = 7
local FIELD_WIDTH   = (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1) * (CELL_WIDTH + CELL_HORIZONTAL_INDENT) + CELL_WIDTH
local FIELD_HEIGHT  = (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1) * (CELL_HEIGHT + CELL_VERTICAL_INDENT) + CELL_HEIGHT  

local BALL_OFFSET = vec3 (0.1, -0.07, 0)

local function get_cell_position (x, y)
  return vec2 (-FIELD_WIDTH / 2 + x * (CELL_WIDTH + CELL_HORIZONTAL_INDENT) + CELL_WIDTH / 2, -FIELD_HEIGHT / 2 + y * (CELL_HEIGHT + CELL_VERTICAL_INDENT) + CELL_HEIGHT / 2, 0)
end

local function swap_cells (cell1, cell2)
  cell1.sprite, cell2.sprite = cell2.sprite, cell1.sprite
  cell1.type,   cell2.type   = cell2.type,   cell1.type

  if (cell1.sprite) then
    cell1.sprite:BindToParent (cell1.root)
  end
  
  if (cell2.sprite) then
    cell2.sprite:BindToParent (cell2.root)
  end
end

local GameFieldCell = {}
GameFieldCell.__index = GameFieldCell

function GameFieldCell.Create ()
  local game_field_cell = {}
  
  setmetatable (game_field_cell, GameFieldCell)
  
  game_field_cell.type   = NO_BALL
  
  game_field_cell.root          = Scene.Node.Create ()
  game_field_cell.root.Name     = "GameFieldCellRoot"
  game_field_cell.root.Position = vec3 (0, 0, BALL_DEPTH_OFFSET)
  game_field_cell.root.Scale    = vec3 (CELL_WIDTH, CELL_HEIGHT, 1)
  
  game_field_cell.particle_node          = Scene.Node.Create ()
  game_field_cell.particle_node.Name     = "GameFieldCellParticleNode"
  game_field_cell.particle_node.Position = vec3 (0, 0, PARTICLE_NODE_DEPTH_OFFSET)
  game_field_cell.particle_node:BindToParent (game_field_cell.root)  

  game_field_cell.sprite = nil
  
  return game_field_cell
end

function GameFieldCell:SetBallType (new_ball_type)
  if (self.type == new_ball_type) then 
    return
  end
  
  if (self.sprite) then
    self.sprite:Unbind ()
    self.sprite = nil
  end

  self.type = new_ball_type
    
  if (new_ball_type == NO_BALL) then
    return 
  end
    
  self.sprite               = Scene.Sprite.Create ()
  self.sprite.Name          = "BallSprite"
  self.sprite.PivotPosition = -BALL_OFFSET
  self.sprite.Material      = BallInfo [new_ball_type].Material
  self.sprite.Color         = vec4 (1, 1, 1, 1)
  self.sprite.Position      = vec3 (0, 0, BallInfo [new_ball_type].Offset)
  self.sprite:BindToParent (self.root) 
end

BallsMinigameState.GameField = {}
BallsMinigameState.GameField.__index = BallsMinigameState.GameField

local function GameFieldColumnAnimationEndHandler (game_field, dummy, column_index)
  if game_field.collumns_move_animations_lists [column_index].size ~= 0 then
    return
  end

  game_field.animated_columns [column_index] = game_field.animated_columns [column_index] - 1
  
  for i = 1, game_field.generate_balls_in_column_after_animation [column_index], 1 do
    game_field:GenerateBallInColumn (column_index)
  end
  
  game_field.generate_balls_in_column_after_animation [column_index] = 0
end

function BallsMinigameState.GameField.Create ()
  local game_field = {}
  
  setmetatable (game_field, BallsMinigameState.GameField)
  
  game_field.root          = Scene.Node.Create ()
  game_field.root.Name     = "GameFieldRoot"
  game_field.root.Position = vec3 (0, 0, GAME_FIELD_DEPTH)

  game_field.opponent = nil
  
  game_field.minimum_balls_in_column = DEFAULT_MINIMUM_BALLS_IN_COLUMN

  game_field.vertical_axis_value   = 0
  game_field.horizontal_axis_value = 0

  game_field.mouse_input_phase         = MOUSE_INPUT_PHASE_IDLE
  game_field.mouse_middle_row_selected = false
  game_field.mouse_column_selected     = nil                      --Номер выделенной колонки
  game_field.previous_cursor_x         = 0                        --Последнее обработанное положение мыши
  game_field.previous_cursor_y         = 0  
  game_field.cursor_x                  = 0                        --Положение мыши
  game_field.cursor_y                  = 0

  game_field.field_change_blocked = 0                     --количество блокирований перемещения шаров пользователем (на время анимации)
  game_field.removed_balls_count  = 0                     --количество сокращенных на последнем ходу шаров

  game_field.helper_ball               = Scene.Sprite.Create ()    --вспомогательный шарик, используемый при анимации прокрутки центральной линии
  game_field.helper_ball.Name          = "HelperBall"
  game_field.helper_ball.PivotPosition = -BALL_OFFSET
  game_field.helper_ball.Color         = vec4 (1, 1, 1, 1)   

  game_field.animated_columns = {}                             --если по индексу x значение не nil, значит колонка х в данный момент анимируется
  game_field.generate_balls_in_column_after_animation = {}     --количество шаров, которое необходимо сгенерировать в колонке по окончании анимации

  game_field.middle_row_move_animations_list = List.Create ()  --список анимаций перемещающихся шаров в средней линии
  game_field.remove_animations_list          = List.Create ()  --список анимаций сокращения шаров
  game_field.collumns_move_animations_lists  = {}              --массив списков анимаций перемещающихся шаров в колонке

  game_field.field = {}
  
  for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
    game_field.collumns_move_animations_lists [i]           = List.Create ()
    game_field.animated_columns [i]                         = 0
    game_field.generate_balls_in_column_after_animation [i] = 0
    
    game_field.field [i] = {}
    
    for j = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1), 1 do
      local new_cell      = GameFieldCell.Create ()
      local cell_position = get_cell_position (i, j)
      
      new_cell.root.Position = vec3 (cell_position.x, cell_position.y, new_cell.root.Position.z)
      
      new_cell.root:BindToParent (game_field.root)
      
      game_field.field [i][j] = new_cell      
    end
  end

  game_field.selection_position      = MIDDLE_COLUMN  --private
  game_field.selection               = Scene.Sprite.Create ()
  game_field.selection.Name          = "SelectionSprite"
  game_field.selection.Material      = "Minigame_Balls_Selection"
  game_field.selection.Color         = vec4 (1, 1, 1, 1)
  game_field.selection.PivotPosition = SELECTION_OFFSET
  game_field.selection.Scale         = vec3 (SELECTION_WIDTH, SELECTION_HEIGHT, 1)
  game_field:RepositionSelection ()
  game_field.selection:BindToParent (game_field.root)

  game_field.ColumnAnimationEndHandler = CreateMethodForObject (GameFieldColumnAnimationEndHandler, game_field)
  
  game_field.ColumnStabilizationAnimationEnd = function (ended_animation, column_index)
    game_field.collumns_move_animations_lists [column_index]:RemoveByValue (ended_animation)
    
    if game_field.collumns_move_animations_lists [column_index].size ~= 0 then
      return
    end

    game_field:CheckMatches ()
     
    game_field.field_change_blocked = game_field.field_change_blocked - 1  
  end
  
  game_field.MiddleRowStabilizationAnimationEnd = function (ended_animation)
    game_field.middle_row_move_animations_list:RemoveByValue (ended_animation)
    
    if game_field.middle_row_move_animations_list.size ~= 0 then
      return
    end

    game_field.helper_ball:Unbind ()

    game_field:CheckMatches ()
          
    game_field.field_change_blocked = game_field.field_change_blocked - 1  
  end
  
  game_field.CycleUpColumnAnimationEnd = function (ended_animation, column_index)
    game_field:ProcessColumnAnimationEnd (ended_animation, column_index, game_field.CycleUpColumn)
  end
  
  game_field.CycleDownColumnAnimationEnd = function (ended_animation, column_index)
    game_field:ProcessColumnAnimationEnd (ended_animation, column_index, game_field.CycleDownColumn)
  end
  
  game_field.CycleLeftMiddleRowAnimationEnd = function (ended_animation)
    game_field:ProcessRowAnimationEnd (ended_animation, game_field.ProcessCycleLeftMiddleRow)
  end  
  
  game_field.CycleRightMiddleRowAnimationEnd = function (ended_animation)
    game_field:ProcessRowAnimationEnd (ended_animation, game_field.ProcessCycleRightMiddleRow)
  end  
  
  game_field.CycleUpColumn = function (column_index)
    for i = (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1), 1, -1 do
      swap_cells (game_field.field [column_index][i], game_field.field [column_index][i - 1])
    end

    PlaySoundEffect (MOVE_SOUND)
  end

  game_field.CycleDownColumn = function (column_index)
    for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 2), 1 do
      swap_cells (game_field.field [column_index][i], game_field.field [column_index][i + 1])
    end  

    PlaySoundEffect (MOVE_SOUND)
  end
  
  game_field.ProcessCycleLeftMiddleRow = function ()
    for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 2), 1 do
      swap_cells (game_field.field [i][BallsMinigameState.MIDDLE_ROW], game_field.field [i + 1][BallsMinigameState.MIDDLE_ROW])
    end  

    PlaySoundEffect (MOVE_SOUND)
  end

  game_field.ProcessCycleRightMiddleRow = function ()
    for i = (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1, -1 do
      swap_cells (game_field.field [i][BallsMinigameState.MIDDLE_ROW], game_field.field [i - 1][BallsMinigameState.MIDDLE_ROW])
    end  

    PlaySoundEffect (MOVE_SOUND)
  end

  game_field.RemoveAnimationEndHandler = function (ended_animation)
    game_field.remove_animations_list:RemoveByValue (ended_animation)
    
    if game_field.remove_animations_list.size ~= 0 then
      return
    end

    game_field.field_change_blocked = game_field.field_change_blocked - 1

    for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 3), 1 do
      local ball_type = game_field.field [i][BallsMinigameState.MIDDLE_ROW].type 
  
      if ((ball_type == game_field.field [i + 1][BallsMinigameState.MIDDLE_ROW].type) and (ball_type == game_field.field [i + 2][BallsMinigameState.MIDDLE_ROW].type)) then
        game_field.removed_balls_count = 0
      
        for j = i, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
          if (game_field.field [j][BallsMinigameState.MIDDLE_ROW].type == ball_type) then
            game_field:RemoveBall (j)
            game_field.removed_balls_count = game_field.removed_balls_count + 1
          else
            break
          end
        end

        break
      end 
    end   
  end
  
  game_field.FieldStackingAnimationEnd = function (ended_animation, column_index)
    ended_animation.object.Position = ended_animation.start_value
    game_field.collumns_move_animations_lists [column_index]:RemoveByValue (ended_animation)
    
    if game_field.collumns_move_animations_lists [column_index].size ~= 0 then
      return
    end

    if (game_field.field [column_index][BallsMinigameState.MIDDLE_ROW + 1].type == NO_BALL) then
      game_field.CycleUpColumn (column_index)
    else
      for i = BallsMinigameState.MIDDLE_ROW, (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 2), 1 do
        swap_cells (game_field.field [column_index][i], game_field.field [column_index][i + 1])
      end  
    end
    
    game_field.field_change_blocked = game_field.field_change_blocked - 1
    
    for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
      if game_field.collumns_move_animations_lists [i].size ~= 0 then
        return
      end  
    end    
      
    game_field:GenerateMissingBalls ()
    game_field.opponent:GenerateBalls (1, game_field.removed_balls_count)
    game_field:CheckMatches ()  
  end

  game_field.DetermineMouseMoveDirection = function ()
    if (game_field.mouse_input_phase ~= MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT) then
      return
    end
  
    if (math.abs (game_field.cursor_x - game_field.previous_cursor_x) > math.abs (game_field.cursor_y - game_field.previous_cursor_y)) then
      game_field.mouse_column_selected = false
    else
      game_field.mouse_middle_row_selected = false
    end
    
    game_field.mouse_input_phase = MOUSE_INPUT_PHASE_MOVE
  end
  
  return game_field
end

function BallsMinigameState.GameField:ProcessRowAnimationEnd (ended_animation, move_handler) --private
  ended_animation.object.Position = ended_animation.start_value
  self.middle_row_move_animations_list:RemoveByValue (ended_animation)
    
  if self.middle_row_move_animations_list.size ~= 0 then
    return
  end

  self.helper_ball:Unbind ()

  move_handler ()
  self:CheckMatches ()
          
  self.field_change_blocked = self.field_change_blocked - 1  
end

function BallsMinigameState.GameField:ProcessColumnAnimationEnd (ended_animation, column_index, move_handler)   --private
  ended_animation.object.Position = ended_animation.start_value
  self.collumns_move_animations_lists [column_index]:RemoveByValue (ended_animation)
    
  if self.collumns_move_animations_lists [column_index].size ~= 0 then
    return
  end

  move_handler (column_index)
  self:CheckMatches ()
     
  self.field_change_blocked = self.field_change_blocked - 1  
end

function BallsMinigameState.GameField:RemoveBall (ball_column)  --private
  self.field [ball_column][BallsMinigameState.MIDDLE_ROW]:SetBallType (NO_BALL)
  
  if (self.field [ball_column][BallsMinigameState.MIDDLE_ROW + 1].type == NO_BALL) then
    self:AnimateCycleColumn (ball_column, vec3 (0, BALL_MOVE_ANIMATION_Y_RANGE, 0), self.FieldStackingAnimationEnd)
  else
    self:AnimateCycleColumn (ball_column, vec3 (0, -BALL_MOVE_ANIMATION_Y_RANGE, 0), self.FieldStackingAnimationEnd, BallsMinigameState.MIDDLE_ROW)
  end
end

function BallsMinigameState.GameField:GenerateMissingBalls () --private
  for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
    local balls_in_column = 0
    
    for j = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1), 1 do
      if (self.field [i][j].type ~= NO_BALL) then
        balls_in_column = balls_in_column + 1
      end
    end

    if (balls_in_column < self.minimum_balls_in_column) then
      self:GenerateBallInColumn (i)
    end
  end
end

function BallsMinigameState.GameField:CheckMatches ()  --private
  for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 3), 1 do
    local ball_type = self.field [i][BallsMinigameState.MIDDLE_ROW].type 
  
    if ((ball_type == self.field [i + 1][BallsMinigameState.MIDDLE_ROW].type) and (ball_type == self.field [i + 2][BallsMinigameState.MIDDLE_ROW].type)) then
      local animation_config = {}
      
      animation_config.start_value                        = 1
      animation_config.target_value                       = 0
      animation_config.delay                              = 0
      animation_config.animation_time                     = BALL_REMOVE_ANIMATION_DURATION
      animation_config.property_set_function              = Scene.Sprite.set_Alpha
      animation_config.animation_end_handlers             = {}
      animation_config.animation_end_handlers [1]         = {}
      animation_config.animation_end_handlers [1].handler = self.RemoveAnimationEndHandler

      for j = i, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
        if (self.field [j][BallsMinigameState.MIDDLE_ROW].type == ball_type) then
          animation_config.object = self.field [j][BallsMinigameState.MIDDLE_ROW].sprite 
                            
          self.remove_animations_list:PushFront (LinearAnimation.Create (animation_config))          
          
          animation_config.delay = animation_config.delay + BALL_REMOVE_ANIMATION_DELAY
        else
          break
        end
      end
      
      self.field_change_blocked = self.field_change_blocked + 1
      PlaySoundEffect (MATCH_THREE_BALLS_SOUND)
      return
    end
  end
end

function BallsMinigameState.GameField:GenerateBallInColumn (column)  --private
  local can_add_ball_to_bottom = self.field [column][0].type == NO_BALL 
  local can_add_ball_to_top    = self.field [column][BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1].type == NO_BALL
  
  if (can_add_ball_to_bottom and (self.field [column][1].type ~= NO_BALL) and 
      ((self.field [column][1].sprite.WorldPosition.y - self.field [column][1].root.WorldPosition.y) < -EPSILON)) then
    can_add_ball_to_bottom = false
  end
  
  if (can_add_ball_to_top and (self.field [column][BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 2].type ~= NO_BALL) and 
      ((self.field [column][BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 2].sprite.WorldPosition.y - self.field [column][BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 2].root.WorldPosition.y) > EPSILON)) then
    can_add_ball_to_top = false
  end
  
  if (not (can_add_ball_to_bottom or can_add_ball_to_top)) then 
    return false
  end

  if (self.animated_columns [column] > 0) then
    self.generate_balls_in_column_after_animation [column] = self.generate_balls_in_column_after_animation [column] + 1 
    return true
  end
   
  local add_new_ball_to_bottom = math.random (0, 1)
	  
  if (add_new_ball_to_bottom == 1) then
    if (not (can_add_ball_to_bottom)) then
      add_new_ball_to_bottom = 0
    else
      for i = 0, BallsMinigameState.MIDDLE_ROW, 1 do
        if ((self.field [column][i].type ~= NO_BALL) or (i == BallsMinigameState.MIDDLE_ROW)) then
          self:GenerateBallAtCoords (column, i - 1)
          return true
        end
      end      
    end
  end
	  
  if ((add_new_ball_to_bottom == 0) and (can_add_ball_to_top)) then
    for i = (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1), BallsMinigameState.MIDDLE_ROW, -1 do
      if ((self.field [column][i].type ~= NO_BALL) or (i == BallsMinigameState.MIDDLE_ROW)) then
        self:GenerateBallAtCoords (column, i + 1)
        return true
      end
    end
  end
  
  return false
end

function BallsMinigameState.GameField:GenerateBalls (min_balls, max_balls)
  local new_balls_count = math.random (min_balls, max_balls)

  for i = 0, (new_balls_count - 1), 1 do
    local column_to_place = math.random (0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1)
    
    local place_tries_count = 0
    
    while (place_tries_count < BallsMinigameState.GAME_FIELD_CELLS_IN_ROW) do
      if (self:GenerateBallInColumn ((column_to_place + place_tries_count) % BallsMinigameState.GAME_FIELD_CELLS_IN_ROW)) then break end      
      place_tries_count = place_tries_count + 1
    end
    
    if (place_tries_count == BallsMinigameState.GAME_FIELD_CELLS_IN_ROW) then
      balls_minigame_game_over (self)
    end
  end
end

function BallsMinigameState.GameField:GenerateBallAtCoords (x, y)
  local new_ball_type = math.random (COLORS_COUNT)
  
  self.field [x][y]:SetBallType (new_ball_type)

  local correction_sprite_row = nil
  
  if (y > BallsMinigameState.MIDDLE_ROW) then
    if (self.field [x][y - 1].sprite) then
      correction_sprite_row = y - 1
    end
  else
    if (self.field [x][y + 1].sprite) then
      correction_sprite_row = y + 1
    end  
  end

  if (correction_sprite_row) then
    self.field [x][y].sprite.Position = vec3 (self.field [x][y].sprite.Position.x, self.field [x][correction_sprite_row].sprite.Position.y, self.field [x][y].sprite.Position.z)
  end
end

function BallsMinigameState.GameField:RepositionSelection ()
  local cell_position = get_cell_position (self.selection_position, BallsMinigameState.MIDDLE_ROW)

  self.selection.Position  = vec3 (cell_position.x, cell_position.y, SELECTION_DEPTH_OFFSET)  
end

function BallsMinigameState.GameField:MoveSelection (x_offset)
  if ((self.selection_position == 0) and (x_offset < 0)) then
    self:CycleLeftMiddleRow ()
    return
  end
  
  if ((self.selection_position == (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1)) and (x_offset > 0)) then
    self:CycleRightMiddleRow ()
    return
  end
  
  self.selection_position = self.selection_position + x_offset
  
  self:RepositionSelection ()
end

function BallsMinigameState.GameField:AnimateMiddleRow (direction, duration, animation_end_handler)
  local animation_config = {}
      
  animation_config.target_value                       = direction
  animation_config.animation_time                     = duration
  animation_config.property_set_function              = Scene.Node.set_Position
  animation_config.animation_end_handlers             = {}
  animation_config.animation_end_handlers [1]         = {}
  animation_config.animation_end_handlers [1].handler = animation_end_handler

  for i = 0, (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1), 1 do
    animation_config.object      = self.field [i][BallsMinigameState.MIDDLE_ROW].sprite
    animation_config.start_value = self.field [i][BallsMinigameState.MIDDLE_ROW].sprite.Position
      
    self.middle_row_move_animations_list:PushFront (LinearAnimation.Create (animation_config))
  end  

  self.field_change_blocked = self.field_change_blocked + 1  
end

function BallsMinigameState.GameField:AnimateCycleMiddleRow (direction, animation_end_handler)
  self:AnimateMiddleRow (direction, BALL_MOVE_ANIMATION_DURATION, animation_end_handler)
end

function BallsMinigameState.GameField:CycleLeftMiddleRow ()
  if (self.field_change_blocked > 0) then
    return
  end

  self.helper_ball.Material = self.field [0][BallsMinigameState.MIDDLE_ROW].sprite.Material
  self.helper_ball.Position = vec3 (BALL_MOVE_ANIMATION_X_RANGE, 0, 0)
  self.helper_ball:BindToParent (self.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].sprite)

  self:AnimateCycleMiddleRow (vec3 (-BALL_MOVE_ANIMATION_X_RANGE, 0, 0), self.CycleLeftMiddleRowAnimationEnd)
end

function BallsMinigameState.GameField:CycleRightMiddleRow ()
  if (self.field_change_blocked > 0) then
    return
  end

  self.helper_ball.Material = self.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].sprite.Material
  self.helper_ball.Position = vec3 (-BALL_MOVE_ANIMATION_X_RANGE, 0, 0)
  self.helper_ball:BindToParent (self.field [0][BallsMinigameState.MIDDLE_ROW].sprite)

  self:AnimateCycleMiddleRow (vec3 (BALL_MOVE_ANIMATION_X_RANGE, 0, 0), self.CycleRightMiddleRowAnimationEnd)
end

function BallsMinigameState.GameField:AnimateColumn (column_index, direction, animation_end_handler, duration, bottom_index, top_index)  --private
  local animation_config = {}
      
  self.animated_columns [column_index] = self.animated_columns [column_index] + 1 
      
  animation_config.target_value                       = direction
  animation_config.animation_time                     = duration
  animation_config.property_set_function              = Scene.Node.set_Position
  animation_config.animation_end_handlers             = {}
  animation_config.animation_end_handlers [1]         = {}
  animation_config.animation_end_handlers [1].handler = animation_end_handler
  animation_config.animation_end_handlers [1].args    = column_index
  animation_config.animation_end_handlers [2]         = {}
  animation_config.animation_end_handlers [2].handler = self.ColumnAnimationEndHandler
  animation_config.animation_end_handlers [2].args    = column_index

  if not bottom_index then bottom_index = 0 end
  if not top_index    then top_index    = (BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1) end

  for i = bottom_index, top_index, 1 do
    if self.field [column_index][i].type ~= NO_BALL then
      animation_config.object      = self.field [column_index][i].sprite
      animation_config.start_value = self.field [column_index][i].sprite.Position
      
      self.collumns_move_animations_lists [column_index]:PushFront (LinearAnimation.Create (animation_config))
    end
  end  
  
  if (self.collumns_move_animations_lists [column_index].size) then
    self.field_change_blocked = self.field_change_blocked + 1
  end
end

function BallsMinigameState.GameField:AnimateCycleColumn (column_index, direction, animation_end_handler, bottom_index, top_index) --private
  self:AnimateColumn (column_index, direction, animation_end_handler, BALL_MOVE_ANIMATION_DURATION, bottom_index, top_index)
end

function BallsMinigameState.GameField:CanCycleUpColumn (column)
  return not ((self.field [column][BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1].type ~= NO_BALL) or (self.field [column][BallsMinigameState.MIDDLE_ROW - 1].type == NO_BALL))
end

function BallsMinigameState.GameField:UserCycleUpColumn ()
  if (self.field_change_blocked > 0) then
    return
  end

  if not self:CanCycleUpColumn (self.selection_position) then
    PlaySoundEffect (CANT_MOVE_SOUND)
    return
  end

  self:AnimateCycleColumn (self.selection_position, vec3 (0, BALL_MOVE_ANIMATION_Y_RANGE, 0), self.CycleUpColumnAnimationEnd)
end

function BallsMinigameState.GameField:CanCycleDownColumn (column)
  return not ((self.field [column][0].type ~= NO_BALL) or (self.field [column][BallsMinigameState.MIDDLE_ROW + 1].type == NO_BALL))
end

function BallsMinigameState.GameField:UserCycleDownColumn ()
  if (self.field_change_blocked > 0) then
    return
  end

  if not self:CanCycleDownColumn (self.selection_position) then
    PlaySoundEffect (CANT_MOVE_SOUND)
    return
  end

  self:AnimateCycleColumn (self.selection_position, vec3 (0, -BALL_MOVE_ANIMATION_Y_RANGE, 0), self.CycleDownColumnAnimationEnd)
end

function BallsMinigameState.GameField:ProcessVerticalAxisInput (axis_value)
  if (axis_value > AXIS_DEAD_ZONE) then
    do
      if (self.vertical_axis_value <= AXIS_DEAD_ZONE) then
        self:UserCycleUpColumn ()
      end
    end
  elseif (axis_value < -AXIS_DEAD_ZONE) then
    do
      if (self.vertical_axis_value >= -AXIS_DEAD_ZONE) then
        self:UserCycleDownColumn ()
      end
    end
  end
  
  self.vertical_axis_value = axis_value
end

function BallsMinigameState.GameField:ProcessHorizontalAxisInput (axis_value)
  if (axis_value > AXIS_DEAD_ZONE) then
    do
      if (self.horizontal_axis_value <= AXIS_DEAD_ZONE) then
        self:MoveSelection (1)
      end
    end
  elseif (axis_value < -AXIS_DEAD_ZONE) then
    do
      if (self.horizontal_axis_value >= -AXIS_DEAD_ZONE) then
        self:MoveSelection (-1)
      end
    end
  end
  
  self.horizontal_axis_value = axis_value
end

local function update_animations_list (animations_list, dt)
  local current_animation = animations_list.first

  while current_animation do
    current_animation.value:Process (dt) 
  
    current_animation = current_animation.next
  end   
end

function BallsMinigameState.GameField:ProcessAnimation (dt)
  update_animations_list (self.middle_row_move_animations_list, dt)
  update_animations_list (self.remove_animations_list, dt)

  for i = 0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1, 1 do
    update_animations_list (self.collumns_move_animations_lists [i], dt)
  end    
end

function BallsMinigameState.GameField:GetBottomBallRow (column) --private
  for i = 0, BallsMinigameState.MIDDLE_ROW, 1 do
    if (self.field [column][i].type ~= NO_BALL) then
      return i
    end
  end
end

function BallsMinigameState.GameField:GetTopBallRow (column) --private
  for i = BallsMinigameState.GAME_FIELD_CELLS_IN_COLUMN - 1, BallsMinigameState.MIDDLE_ROW, -1 do
    if (self.field [column][i].type ~= NO_BALL) then
      return i
    end
  end
end

-- Обработка ввода с мыши
function BallsMinigameState.GameField:MouseDown ()
  if (self.field_change_blocked > 0) then
    return
  end

  self.previous_cursor_x = self.cursor_x
  self.previous_cursor_y = self.cursor_y

  local x = self.cursor_x * WORLD_WIDTH
  local y = -self.cursor_y * WORLD_HEIGHT

  if ((x > (self.field [0][0].root.WorldPosition.x - BALL_WORLD_X_RADIUS)) and 
      (x < (self.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][0].root.WorldPosition.x + BALL_WORLD_X_RADIUS)) and
      (y > (self.field [0][BallsMinigameState.MIDDLE_ROW].root.WorldPosition.y - BALL_WORLD_Y_RADIUS)) and
      (y < (self.field [0][BallsMinigameState.MIDDLE_ROW].root.WorldPosition.y + BALL_WORLD_Y_RADIUS))) then
    self.mouse_middle_row_selected = true
    self.mouse_input_phase         = MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE
  end
  
  for i = 0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1, 1 do
    if ((x > (self.field [i][0].root.WorldPosition.x - BALL_WORLD_X_RADIUS)) and 
        (x < (self.field [i][0].root.WorldPosition.x + BALL_WORLD_X_RADIUS)) and
        (y > (self.field [i][self:GetBottomBallRow (i)].root.WorldPosition.y - BALL_WORLD_Y_RADIUS)) and
        (y < (self.field [i][self:GetTopBallRow (i)].root.WorldPosition.y + BALL_WORLD_Y_RADIUS))) then
      self.mouse_column_selected = i
      self.mouse_input_phase     = MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE
      break
    end
  end
  
  if (self.mouse_middle_row_selected or self.mouse_column_selected) then
    self.field_change_blocked = self.field_change_blocked + 1
  end
end

function BallsMinigameState.GameField:ProcessMouseFirstMove ()
  if (self.mouse_column_selected and (not self.mouse_middle_row_selected)) then
    self.mouse_moving_vertical = true
    self.mouse_input_phase     = MOUSE_INPUT_PHASE_MOVE
    return 
  end

  DelayedAction (MOUSE_INPUT_DIRECTION_DETECTION_TIMEOUT, self.DetermineMouseMoveDirection)
  self.mouse_input_phase = MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT
end

function BallsMinigameState.GameField:MouseUp ()
  if (self.mouse_middle_row_selected or self.mouse_column_selected) then
    self.field_change_blocked = self.field_change_blocked - 1
  end

  if (self.mouse_column_selected) then
    self:AnimateColumn (self.mouse_column_selected, vec3 (0, 0, 0), self.ColumnStabilizationAnimationEnd, BALL_STABILIZATION_ANIMATION_DURATION)
  end

  if (self.mouse_middle_row_selected) then
    self:AnimateMiddleRow (vec3 (0, 0, 0), BALL_STABILIZATION_ANIMATION_DURATION, self.MiddleRowStabilizationAnimationEnd)
    self.helper_ball:Unbind ()
  end

  self.mouse_input_phase = MOUSE_INPUT_PHASE_IDLE
  self.mouse_middle_row_selected = false
  self.mouse_column_selected     = nil
end

function BallsMinigameState.GameField:MoveMiddleRow (offset)
  local column_interval                   = (self.field [1][0].root.WorldPosition.x - self.field [0][0].root.WorldPosition.x) / 2 
  local maximum_offset_without_reposition = self.field [0][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition.x - self.field [0][BallsMinigameState.MIDDLE_ROW].root.WorldPosition.x

  if (offset > 0) then
    maximum_offset_without_reposition = column_interval - maximum_offset_without_reposition
  else
    maximum_offset_without_reposition = column_interval + maximum_offset_without_reposition
  end

  if (maximum_offset_without_reposition > math.abs (offset)) then  
    for i = 0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1, 1 do
      self.field [i][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition = self.field [i][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition + vec3 (offset, 0, 0) 
    end
  elseif (offset < 0) then
    for i = 0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1, 1 do
      self.field [i][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition = self.field [i][BallsMinigameState.MIDDLE_ROW].root.WorldPosition + vec3 (column_interval, 0, 0) 
    end

    self.ProcessCycleLeftMiddleRow ()
    offset = offset + maximum_offset_without_reposition
    self:MoveMiddleRow (offset)
    return
  else
    for i = 0, BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1, 1 do
      self.field [i][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition = self.field [i][BallsMinigameState.MIDDLE_ROW].root.WorldPosition - vec3 (column_interval, 0, 0) 
    end

    self.ProcessCycleRightMiddleRow ()
    offset = offset - maximum_offset_without_reposition
    self:MoveMiddleRow (offset)
    return
  end

  if ((self.field [0][BallsMinigameState.MIDDLE_ROW].sprite.WorldPosition.x - self.field [0][BallsMinigameState.MIDDLE_ROW].root.WorldPosition.x) < 0) then
    self.helper_ball.Material = self.field [0][BallsMinigameState.MIDDLE_ROW].sprite.Material
    self.helper_ball.Position = vec3 (BALL_MOVE_ANIMATION_X_RANGE, 0, 0)
    self.helper_ball:BindToParent (self.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].sprite)
  else
    self.helper_ball.Material = self.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].sprite.Material
    self.helper_ball.Position = vec3 (-BALL_MOVE_ANIMATION_X_RANGE, 0, 0)
    self.helper_ball:BindToParent (self.field [0][BallsMinigameState.MIDDLE_ROW].sprite)
  end
end

function BallsMinigameState.GameField:MouseXMove (x)
  self.cursor_x = x
  
  if (self.mouse_input_phase == MOUSE_INPUT_PHASE_MOVE) then
    if (not self.mouse_middle_row_selected) then
      return
    end

    self:MoveMiddleRow ((x - self.previous_cursor_x) * MOUSE_INPUT_X_SCALE)
    
    self.previous_cursor_x = x
  end
  
  if ((self.mouse_input_phase == MOUSE_INPUT_PHASE_IDLE) or
      (self.mouse_input_phase == MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT)) then
    return
  end
  
  if (self.mouse_input_phase == MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE) then
    self:ProcessMouseFirstMove ()
    return
  end
end

function BallsMinigameState.GameField:MoveColumn (column, offset)
  local row_interval                      = (self.field [0][1].root.WorldPosition.y - self.field [0][0].root.WorldPosition.y) / 2 
  local bottom_ball                       = self:GetBottomBallRow (column)
  local top_ball                          = self:GetTopBallRow (column)
  local maximum_offset_without_reposition = self.field [column][bottom_ball].sprite.WorldPosition.y - self.field [column][bottom_ball].root.WorldPosition.y

  if ((offset > 0) and (self:CanCycleDownColumn (column))) then
    maximum_offset_without_reposition = maximum_offset_without_reposition + row_interval
  elseif ((offset < 0) and (self:CanCycleUpColumn (column))) then
    maximum_offset_without_reposition = maximum_offset_without_reposition - row_interval
  end

  if (math.abs (maximum_offset_without_reposition) > math.abs (offset)) then  
    for i = bottom_ball, top_ball, 1 do
      self.field [column][i].sprite.WorldPosition = self.field [column][i].sprite.WorldPosition - vec3 (0, offset, 0) 
    end
  elseif ((offset < 0) and (self:CanCycleUpColumn (column))) then
    for i = bottom_ball, top_ball, 1 do
      self.field [column][i].sprite.WorldPosition = self.field [column][i].root.WorldPosition - vec3 (0, row_interval, 0) 
    end

    self.CycleUpColumn (column)
    offset = offset - maximum_offset_without_reposition
    self:MoveColumn (column, offset)
  elseif ((offset > 0) and (self:CanCycleDownColumn (column))) then
    for i = bottom_ball, top_ball, 1 do
      self.field [column][i].sprite.WorldPosition = self.field [column][i].root.WorldPosition + vec3 (0, row_interval, 0)
    end

    self.CycleDownColumn (column)
    offset = offset - maximum_offset_without_reposition
    self:MoveColumn (column, offset)
  else
    for i = bottom_ball, top_ball, 1 do
      self.field [column][i].sprite.WorldPosition = self.field [column][i].root.WorldPosition
    end
  end
end

function BallsMinigameState.GameField:MouseYMove (y)
  self.cursor_y = y
  
  if (self.mouse_input_phase == MOUSE_INPUT_PHASE_MOVE) then
    if (not self.mouse_column_selected) then
      return
    end
    
    self:MoveColumn (self.mouse_column_selected, (y - self.previous_cursor_y) * MOUSE_INPUT_Y_SCALE)
    
    self.previous_cursor_y = y
  end

  if ((self.mouse_input_phase == MOUSE_INPUT_PHASE_IDLE) or
      (self.mouse_input_phase == MOUSE_INPUT_PHASE_FIRST_MOVE_TIMEOUT)) then
    return
  end
  
  if (self.mouse_input_phase == MOUSE_INPUT_PHASE_WAITING_FOR_FIRST_MOVE) then
    self:ProcessMouseFirstMove ()
    return
  end  
end

function BallsMinigameState.GameField:TouchBegan (x, y)
  WindowManager.Node:SetPosition(0,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,0)

  self:MouseXMove (x)
  self:MouseYMove (y)
  
  self:MouseDown ()

  WindowManager.Node:SetPosition(600,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,-90)
end

function BallsMinigameState.GameField:TouchEnded (x, y)
  WindowManager.Node:SetPosition(0,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,0)

  self:MouseXMove (x)
  self:MouseYMove (y)
  
  self:MouseUp ()

  WindowManager.Node:SetPosition(600,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,-90)
end

function BallsMinigameState.GameField:TouchMoved (x, y)
  WindowManager.Node:SetPosition(0,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,0)

  self:MouseXMove (x)
  self:MouseYMove (y)

  WindowManager.Node:SetPosition(600,0,10)
  WindowManager.Node:SetEulerOrientation(0,0,-90)
end

function BallsMinigameState.GameField:Debug ()
  self:GenerateBallInColumn (2)
end
