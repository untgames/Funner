rem MakeCert -n "CN=leny" -r -h 0 -eku "1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13" -e 03/24/2014 -sv MyKey.pvk MyKey.cer
rem rem Pvk2Pfx -pvk MyKey.pvk -pi 123456 -spc MyKey.cer -pfx MyKey.pfx
rem Pvk2Pfx -pvk MyKey.pvk -spc MyKey.cer -pfx MyKey.pfx

makecert.exe mycert.cer -r -n "CN=leny" -$ individual  -sv private.pkv -pe -cy end
pvk2pfx -pvk private.pkv -spc mycert.cer -pfx mytemporarykey.pfx