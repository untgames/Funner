function searchpaths ()
  return "data/?.lua"
end

function test ()
  print ("Print override")
  print ("Dofile override = " .. dofile ("dofile"))
  
  local function indent_test()
    assert (false)
  end
  
  print ("Assert override")  

  indent_test ()
end
