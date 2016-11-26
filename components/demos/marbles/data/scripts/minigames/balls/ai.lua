BallsMinigameState = BallsMinigameState or {}

BallsMinigameState.AI = {}
BallsMinigameState.AI.__index = BallsMinigameState.AI

local MAXIMUM_SPEED       = 0.15
local MINIMUM_SPEED       = 0.6
local MAXIMUM_LEVEL_RANGE = 0.6
local MINIMUM_LEVEL_RANGE = 0.15
local MAXIMUM_DIFFICULTY  = 10

local BAD_MOVE_PROBABILITY = 10

local function have_not_we_do_bad_move ()
  return 0 ~= math.random (1, BAD_MOVE_PROBABILITY)
end

local POSSIBLE_ACTIONS = {
  function (game_field)
    game_field:MoveSelection (1)
    
    return true
  end,

  function (game_field)
    game_field:MoveSelection (-1)
    
    return true
  end,

  function (game_field)
    if not game_field:CanCycleUpColumn (game_field.selection_position) and have_not_we_do_bad_move () then return false end
  
    game_field:UserCycleUpColumn ()
    
    return true
  end,

  function (game_field)
    if not game_field:CanCycleDownColumn (game_field.selection_position) and have_not_we_do_bad_move () then return false end
  
    game_field:UserCycleDownColumn ()
    
    return true
  end
}

local function get_next_step_time (speed, speed_variation)
  return speed + math.random () * speed_variation
end

local function cycle_middle_row (game_field)
  local left_ball_type = game_field.field [0][BallsMinigameState.MIDDLE_ROW].type

  if (game_field.field [1][BallsMinigameState.MIDDLE_ROW].type == left_ball_type) and
     (game_field.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].type == left_ball_type) then
    game_field:MoveSelection (1)
    return true
  end

  if (game_field.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 2][BallsMinigameState.MIDDLE_ROW].type == left_ball_type) and
     (game_field.field [BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1][BallsMinigameState.MIDDLE_ROW].type == left_ball_type) then
    game_field:MoveSelection (-1)
    return true
  end

  return false
end

local function check_sides_balls (game_field, ball_type)
  if (game_field.selection_position == 0) or (game_field.selection_position == BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 1) then return false end

  if (game_field.field [game_field.selection_position - 1][BallsMinigameState.MIDDLE_ROW].type == ball_type) and
     (game_field.field [game_field.selection_position + 1][BallsMinigameState.MIDDLE_ROW].type == ball_type) then
    return true
  end
  
  return false
end

local function check_one_side_balls (game_field, ball_type)
  if (game_field.selection_position > 1) then 
    if (game_field.field [game_field.selection_position - 1][BallsMinigameState.MIDDLE_ROW].type == ball_type) and
      (game_field.field [game_field.selection_position - 2][BallsMinigameState.MIDDLE_ROW].type == ball_type) then
      return true
    end
  end
  
  if (game_field.selection_position < (BallsMinigameState.GAME_FIELD_CELLS_IN_ROW - 2)) then 
    if (game_field.field [game_field.selection_position + 1][BallsMinigameState.MIDDLE_ROW].type == ball_type) and
      (game_field.field [game_field.selection_position + 2][BallsMinigameState.MIDDLE_ROW].type == ball_type) then
      return true
    end
  end
  
  return false
end

local function cycle_column (game_field)
  local top_ball_type = game_field.field [game_field.selection_position][BallsMinigameState.MIDDLE_ROW + 1].type
  local top_ball2_type = game_field.field [game_field.selection_position][BallsMinigameState.MIDDLE_ROW + 2].type

  if game_field:CanCycleDownColumn (game_field.selection_position) and (check_sides_balls (game_field, top_ball_type) or check_sides_balls (game_field, top_ball2_type) or
     check_one_side_balls (game_field, top_ball_type) or check_one_side_balls (game_field, top_ball2_type)) then
    game_field:UserCycleDownColumn ()
    return true
  end

  local bottom_ball_type = game_field.field [game_field.selection_position][BallsMinigameState.MIDDLE_ROW - 1].type
  local bottom_ball2_type = game_field.field [game_field.selection_position][BallsMinigameState.MIDDLE_ROW - 2].type

  if game_field:CanCycleUpColumn (game_field.selection_position) and (check_sides_balls (game_field, bottom_ball_type) or check_sides_balls (game_field, bottom_ball2_type) or 
     check_one_side_balls (game_field, bottom_ball_type) or check_one_side_balls (game_field, bottom_ball2_type)) then
    game_field:UserCycleUpColumn ()
    return true
  end
  
  return false
end

function BallsMinigameState.AI.Create (game_field, ai_level)
  local ai = {}
  
  setmetatable (ai, BallsMinigameState.AI)
  
  ai.game_field         = game_field   --поле для игры
  ai.idle_time          = 0            --текущее время "обдумывания следующего хода"
  
  local difficulty = math.min (ai_level, MAXIMUM_DIFFICULTY) / MAXIMUM_DIFFICULTY
  
  ai.speed_variation = MAXIMUM_LEVEL_RANGE - (MAXIMUM_LEVEL_RANGE - MINIMUM_LEVEL_RANGE) * difficulty  --вариативность скорости
  ai.speed           = MINIMUM_SPEED - (MINIMUM_SPEED - MAXIMUM_SPEED) * difficulty                    --базовая скорость
  ai.next_step_time  = get_next_step_time (ai.speed, ai.speed_variation)   --время следующего хода

  return ai
end

function BallsMinigameState.AI:Update (dt)
  self.idle_time = self.idle_time + dt
  
  if self.idle_time < self.next_step_time then return end
  
  self.idle_time      = 0
  self.next_step_time = get_next_step_time (self.speed, self.speed_variation)
  
  if (cycle_middle_row (self.game_field)) then return end
  if (cycle_column (self.game_field)) then return end
  
  local move_type = nil
  
  repeat
    move_type = math.random (1, #POSSIBLE_ACTIONS)
  until POSSIBLE_ACTIONS [move_type] (self.game_field)
end
