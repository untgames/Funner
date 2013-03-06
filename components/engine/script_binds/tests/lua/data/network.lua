function testSync ()
  print ("Sync test")

  local client = Network.TcpClient.Create ("google.com", 80)
  
  client:Send ("Hello")
  
  local result = client:Receive ()
  
  print (result)
end

function testASync ()
  print ("ASync test")

  local client = Network.TcpClient.Create ("google.com", 80)

  client:SwitchToAsyncSending ()
  client:SwitchToAsyncReceiving ()

  local function OnDataReceiving (data)
--    print (data)
    print ("Response has been received")
    System.Application.Exit (0)
  end

  local callback = Network.TcpClient.CreateAsyncReceivingCallbackHandler (OnDataReceiving)

  client:RegisterAsyncReceivingEventHandler (callback)

  print ("Sending data...")
  
  client:Send ("Hello\n")

  print ("Waiting for response...")

  apprun ()
end

function test ()
--  testSync ()
  testASync ()
end
