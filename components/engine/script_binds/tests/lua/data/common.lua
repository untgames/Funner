local EXISTING_FILE     = "data/text_file.txt" 
local NOT_EXISTING_FILE = "data/not_existing_file.txt" 
local CREATE_DIR        = "create"

function test_files()
  local data = Common.File.LoadString ("data/text_file.txt")

  print ("File data: '" .. data .. "'")

  data = Common.File.LoadStringFilterOut ("data/text_file1.txt", "\n\r")

  print ("Filtered file data: '" .. data .. "'")
  
  if Common.File.IsFileExist (EXISTING_FILE) then
    print ("File '" .. EXISTING_FILE .. "' exist")
  else
    print ("File '" .. EXISTING_FILE .. "' not exist")
  end
  
  if Common.File.IsFileExist (NOT_EXISTING_FILE) then
    print ("File '" .. NOT_EXISTING_FILE .. "' exist")
  else
    print ("File '" .. NOT_EXISTING_FILE .. "' not exist")
  end
  
  print ("File hash = " .. Common.File.FileHash ("data/text_file.txt"))
  print ("File part hash = " .. Common.File.FileHash ("data/text_file.txt", 4))
  print ("File full part hash = " .. Common.File.FileHash ("data/text_file.txt", 1024))
  
  if Common.File.IsFileExist (CREATE_DIR) then
    print ("File '" .. CREATE_DIR .. "' exist")
  else
    print ("File '" .. CREATE_DIR .. "' not exist")
  end
  
  Common.File.Mkdir (CREATE_DIR)
  
  if Common.File.IsFileExist (CREATE_DIR) then
    print ("File '" .. CREATE_DIR .. "' exist")
  else
    print ("File '" .. CREATE_DIR .. "' not exist")
  end
  
  Common.File.Remove (CREATE_DIR)
  
  if Common.File.IsFileExist (CREATE_DIR) then
    print ("File '" .. CREATE_DIR .. "' exist")
  else
    print ("File '" .. CREATE_DIR .. "' not exist")
  end
end

function test_post()
  print "Post test"

  local FILE_NAME = "http://www.untgames.com" 

  local data = Common.File.PostString (FILE_NAME, "post_string")

--  print ("File data: '" .. data .. "'")
  if data ~= "" then print ("Data has received") end
end

function test ()
  test_files ()
  test_post ()
end
