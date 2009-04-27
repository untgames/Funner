function test ()
  print ("Print override")
  print ("Dofile override = " .. dofile ("data/dofile.lua"))
  
  local function indent_test()
    assert (false)
  end
  
  print ("Assert override")  

  indent_test ()
end