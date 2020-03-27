function test_async_post()
  print "Async post test"
  
  local function OnLoad (data)
    if data ~= "" then print ("Data received") end
    
    System.Application.Exit (0)
  end
  
  local callback = Common.File.CreateAsyncLoadHandler (OnLoad)

  local FILE_NAME = "http://untgames.com"
  local result    = Common.File.AsyncPostString (FILE_NAME, "post_string", callback)

  print ("Test completed")
end

function test ()
  test_async_post ()
end
