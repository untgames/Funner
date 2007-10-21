print (my_c_func("Arg passed to my_c_func"))
print (my_c_func2 (5, 2.4, 3.8, 4.019735, "Arg5 passed to my_c_func2"))
function my_lua (arg1)
  print ("Arg1 of my_lua")
  print (arg1)
  return arg1*arg1
end
function my_lua2 ()
  print ("It was printed from my_lua2 function")
end
