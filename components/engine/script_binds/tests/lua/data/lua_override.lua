function test ()
  print ("Print override")
  print ("Dofile override = " .. dofile ("data/dofile.lua"))
end