function animation_event_handler (track, event)
  local name = ""  
  
  if     event == Media.SpineAnimationEvent.Start     then name = "Start"
  elseif event == Media.SpineAnimationEvent.Interrupt then name = "Interrupt"
  elseif event == Media.SpineAnimationEvent.End       then name = "End"
  elseif event == Media.SpineAnimationEvent.Complete  then name = "Complete"
  elseif event == Media.SpineAnimationEvent.Dispose   then name = "Dispose"
  end  

  print ("Animation event '" .. name .. "'")
end

function user_event_handler (track, event_name, int, float, string)
  print ("User event '" .. event_name .. "': int = " .. int .. ", float = " .. float .. ", string = '" .. string .. "'")
end

function test ()
  local skeleton_data = Media.SpineSkeletonData.Create ("data/spine_3.6/goblins/export/goblins-pro.skel", "data/spine_3.6/goblins/export/goblins.atlas")

  print (string.format ("Skeleton data size is %d x %d", skeleton_data.Width, skeleton_data.Height))
  print ("Skeleton data animations count is " .. skeleton_data.AnimationsCount)

  local skeleton             = skeleton_data:CreateSkeleton ()
  local animation_state_data = skeleton_data:CreateAnimationStateData () 

  print ("Skeleton slots count is " .. skeleton.SlotsCount)
  print ("Animation state data default mix is " .. animation_state_data.DefaultMix)
  
  local animation_state         = animation_state_data:CreateAnimationState ()
  local animation_event_handler = Media.Spine.CreateAnimationEventHandler ("animation_event_handler")
  local user_event_handler      = Media.Spine.CreateUserEventHandler ("user_event_handler")
  
  animation_state:RegisterAnimationEventHandler (Media.SpineAnimationEvent.Start,     animation_event_handler)
  animation_state:RegisterAnimationEventHandler (Media.SpineAnimationEvent.Interrupt, animation_event_handler)
  animation_state:RegisterAnimationEventHandler (Media.SpineAnimationEvent.End,       animation_event_handler)
  animation_state:RegisterAnimationEventHandler (Media.SpineAnimationEvent.Complete,  animation_event_handler)
  animation_state:RegisterAnimationEventHandler (Media.SpineAnimationEvent.Dispose,   animation_event_handler)
  
  animation_state:RegisterUserEventHandler (user_event_handler)
  
  local sg_skeleton = Scene.SpineSkeleton.Create (skeleton)
  
  print ("Is SG skeleton visual structure dirty = " .. tostring (sg_skeleton.IsVisualStructureDirty))
  
  animation_state:EnqueueAnimation (0, skeleton_data:AnimationName (0), false, 0)
  
  local animation_controller        = Scene.Controllers.SpineAnimation.Create (sg_skeleton, animation_state)
  local visual_structure_controller = Scene.Controllers.SpineSkeletonVisualStructureBuilder.Create (sg_skeleton, 1, 2)
  
  sg_skeleton:Update (Xtl.Rational.LongLong.Create (1))
  
  print ("Is SG skeleton visual structure dirty = " .. tostring (sg_skeleton.IsVisualStructureDirty))
  
  sg_skeleton:Update (Xtl.Rational.LongLong.Create (100))
  
  --TODO add a lot of tests
end
