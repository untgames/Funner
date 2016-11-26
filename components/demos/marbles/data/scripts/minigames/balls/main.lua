local GAME_ROOT_POSITION = vec3 (400, -300, 0) 
local GAME_ROOT_SCALE    = vec3 (710, 710, 1)

local FIELD_OFFSET = vec3 (0.233, 0, 0)

local INPUT_ATTACHMENT_NAME = "Balls"

local LEFT_PLAYER_MANA_BOTTLE_POSITION  = vec3 (-0.46, -0.295, 1)
local RIGHT_PLAYER_MANA_BOTTLE_POSITION = vec3 (0.375, -0.39, 1)

local game_paused = nil --пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ

BallsMinigameState = BallsMinigameState or {}  
UIControlScript = UIControlScript or {}

local frames_count = 0
local time = 0

function balls_minigame_idle_handler (dt)
  if game_paused then return end
  
  time = time + dt
  frames_count = frames_count + 1
  
  if (time > 2) then
    local window=WindowManager.GetWindowByName("Minigame_Balls")
    if window then
      local bottles=window.GetControlByName("FPS")
      bottles.SetCaption (string.format ("%3.0f", frames_count / time))
    end
    
    time = 0
    frames_count = 0
  end

  BallsMinigameState.right_game_field:ProcessAnimation (dt)
  BallsMinigameState.left_game_field:ProcessAnimation (dt)
  
  if (BallsMinigameState.ai) then
    BallsMinigameState.ai:Update (dt)
  end  
  
  if (BallsMinigameState.mana_bottle_animation) then
    BallsMinigameState.mana_bottle_animation:Process (dt)
  end
end

UIControlScript["RunBallsMinigame"] = function()
  Engine.InputEventHandlers.Register (INPUT_ATTACHMENT_NAME, Input.CreateEventHandler ())

  BallsMinigameState.action_queue = Common.ActionQueue.Create ()

  BallsMinigameState.idle_connection = BallsMinigameState.action_queue:RegisterEventHandler (Common.ActionQueue.CreateEventHandler ("balls_minigame_idle_handler"))  

  BallsMinigameState.left_game_field  = BallsMinigameState.GameField.Create ()
  BallsMinigameState.right_game_field = BallsMinigameState.GameField.Create ()

  BallsMinigameState.Root = Scene.Node.Create ()
  BallsMinigameState.Root.Position = GAME_ROOT_POSITION
  BallsMinigameState.Root.Scale    = GAME_ROOT_SCALE
  
  BallsMinigameState.Root:BindToParent (WindowManager.GetWindowByName ("Minigame_Balls").ForeNode)
  
  BallsMinigameState.left_player_mana_bottle          = Scene.Node.Create ()
  BallsMinigameState.left_player_mana_bottle.Name     = "LeftPlayerManaBottle"
  BallsMinigameState.left_player_mana_bottle.Position = LEFT_PLAYER_MANA_BOTTLE_POSITION
  BallsMinigameState.left_player_mana_bottle:BindToParent (BallsMinigameState.Root)

  BallsMinigameState.right_player_mana_bottle          = Scene.Node.Create ()
  BallsMinigameState.right_player_mana_bottle.Name     = "RightPlayerManaBottle"
  BallsMinigameState.right_player_mana_bottle.Position = RIGHT_PLAYER_MANA_BOTTLE_POSITION
  BallsMinigameState.right_player_mana_bottle:BindToParent (BallsMinigameState.Root)

  BallsMinigameState.left_game_field.root:BindToParent (BallsMinigameState.Root)
  BallsMinigameState.right_game_field.root:BindToParent (BallsMinigameState.Root)

  BallsMinigameState.left_game_field.root.Position  = -FIELD_OFFSET
  BallsMinigameState.right_game_field.root.Position = FIELD_OFFSET

  for i = 0, 4, 1 do
    for j = 5, 9, 1 do
      BallsMinigameState.left_game_field:GenerateBallAtCoords (i, j)
      BallsMinigameState.right_game_field:GenerateBallAtCoords (i, j)
    end
  end

  BallsMinigameState.left_game_field:CheckMatches ()
  BallsMinigameState.right_game_field:CheckMatches ()

  BallsMinigameState.left_game_field.opponent  = BallsMinigameState.right_game_field
  BallsMinigameState.right_game_field.opponent = BallsMinigameState.left_game_field
  
  BallsMinigameState.ai = BallsMinigameState.AI.Create (BallsMinigameState.left_game_field, Game.Player.ManaBottles)
  
  game_paused = false
end

UIControlScript["ShutdownBallsMinigame"] = function()
  if not BallsMinigameState.idle_connection then return end

  Engine.InputEventHandlers.Unregister (INPUT_ATTACHMENT_NAME)

  BallsMinigameState.idle_connection:Disconnect ()
  
  BallsMinigameState.action_queue     = nil
  BallsMinigameState.idle_connection  = nil
  BallsMinigameState.left_game_field  = nil
  BallsMinigameState.right_game_field = nil
  BallsMinigameState.Root             = nil
  BallsMinigameState.ai               = nil

  collectgarbage ("collect")
end

UIControlScript["BallsMinigamePause"] = function()
  game_paused = true

  Engine.InputEventHandlers.Unregister (INPUT_ATTACHMENT_NAME)
end

UIControlScript["BallsMinigameUnpause"] = function()
  print ("Unpause")
  
  game_paused = false

  Engine.InputEventHandlers.Register (INPUT_ATTACHMENT_NAME, Input.CreateEventHandler ())
end
