function test ()
  local var_registry = VarRegistry.Create ()

  print ("Branch name = " .. var_registry.BranchName)
  print ("IsOpened = " .. tostring (var_registry:IsOpened ()))
  print ("Has a = " .. tostring (var_registry:HasVariable ("a")))

  var_registry:Open ("branch")

  print ("Branch name = " .. var_registry.BranchName)
  print ("IsOpened = " .. tostring (var_registry:IsOpened ()))

  var_registry:SetValue ("a", "1")

  print ("Has a = " .. tostring (var_registry:HasVariable ("a")))
  print ("a = " .. var_registry:GetValue ("a"))

  var_registry:SetValue ("a", "1")
  
  print ("Has a = " .. tostring (var_registry:HasVariable ("a")))

  var_registry:Close ()

  print ("Has a = " .. tostring (var_registry:HasVariable ("a")))
end
