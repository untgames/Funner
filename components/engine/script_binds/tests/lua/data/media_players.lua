function dump_playlist (name, playlist)
  local playlist_content = ""
  
  for i = 0, playlist.Size - 1, 1 do
    if (i > 0) then
      playlist_content = playlist_content .. ",  "
    end
    
    playlist_content = playlist_content .. playlist:Item (i)
  end 
  
  print (name .. " = {" .. playlist_content .. "}")
end

function test_playlist () 
  print ("Playlist test")
  
  local list1 = Media.Playlist.Create ()
  
  dump_playlist ("list1_default", list1)

  print ("list1 empty: " .. tostring (list1.Empty))
    
  list1:AddSource ("source1")
  local remove_index = list1:AddSource ("source2")
  list1:AddSource ("source3")
    
  dump_playlist ("list1_after_add", list1)
  print ("list1 empty: " .. tostring (list1.Empty))    
    
  list1:RemoveSource (remove_index)
    
  dump_playlist ("list1_after_remove", list1)
    
  local list2 = list1
  local list3 = list1:Clone ()
    
  dump_playlist ("list2_copy", list2)
    
  list1:RemoveSource (0)
    
  dump_playlist ("list2_after_list1_remove", list2)
  dump_playlist ("list3_after_list1_remove", list3)
    
  list1:AddSource ("source4")
  list1:AddSource ("source4")
  list1:AddSource ("source5")    
  list1:AddSource ("source4")    
    
  dump_playlist ("list1_with_duplicates", list1)
    
  list1:RemoveSource ("source4")
    
  dump_playlist ("list1_after_remove_duplicates", list1)
end

function get_event_name (event)
  if (event == Media.MediaPlayerEvent.OnChangeName)       then return "Media.MediaPlayerEvent.OnChangeName"       end
  if (event == Media.MediaPlayerEvent.OnChangeTarget)     then return "Media.MediaPlayerEvent.OnChangeTarget"     end
  if (event == Media.MediaPlayerEvent.OnChangePlaylist)   then return "Media.MediaPlayerEvent.OnChangePlaylist"   end
  if (event == Media.MediaPlayerEvent.OnChangeTrack)      then return "Media.MediaPlayerEvent.OnChangeTrack"      end
  if (event == Media.MediaPlayerEvent.OnChangePlayback)   then return "Media.MediaPlayerEvent.OnChangePlayback"   end
  if (event == Media.MediaPlayerEvent.OnChangeVolume)     then return "Media.MediaPlayerEvent.OnChangeVolume"     end
  if (event == Media.MediaPlayerEvent.OnChangeRepeatMode) then return "Media.MediaPlayerEvent.OnChangeRepeatMode" end
end

function media_player_event_handler (event)
  print ("New event: " .. get_event_name (event))
end

function test_media_player () 
  print ("MediaPlayer test")
  
  local player = Media.MediaPlayer.Create ()
  
  local handler = Media.MediaPlayer.CreateEventHandler ("media_player_event_handler") 
    
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangeName,       handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangeTarget,     handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangePlaylist,   handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangeTrack,      handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangePlayback,   handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangeVolume,     handler)
  player:RegisterEventHandler (Media.MediaPlayerEvent.OnChangeRepeatMode, handler)

  player.Name = "player"
    
  print ("Tracks count: " .. player.TracksCount)
    
  player.RepeatMode = MediaPlayerRepeatMode_Last
    
  player.RepeatMode = MediaPlayerRepeatMode_All
    
  print ("Change mute")
    
  player.Mute = true
    
  print ("volume=" .. player.Volume .. " mute=" .. tostring (player.Mute))
    
  player.Mute = false
    
  print ("volume=" .. player.Volume .. " mute=" .. tostring (player.Mute))
    
  print ("Dynamic change volume")
    
  for i = 0, 4, 1 do
    player.Volume = 1 / 5 * i
  end
end

function test () 
  test_playlist ()
  test_media_player ()
end
