function test_files()
  local data = File.LoadString ('data/text_file.txt')

  print ("File data: '" .. data .. "'")
end

function test ()
  test_files ()
end
