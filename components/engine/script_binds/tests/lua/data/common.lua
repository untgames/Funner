function test_files()
  local data = File.LoadString ("data/text_file.txt")

  print ("File data: '" .. data .. "'")

  data = File.LoadStringFilterOut ("data/text_file1.txt", "\n\r")

  print ("Filtered file data: '" .. data .. "'")
end

function test ()
  test_files ()
end
