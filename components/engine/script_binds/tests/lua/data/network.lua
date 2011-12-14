function test ()
  local client = Network.TcpClient.Create ("google.com", 80)
  
  client:Send ("Hello")
  
  local result = client:Receive ()
  
  print (result)
end
