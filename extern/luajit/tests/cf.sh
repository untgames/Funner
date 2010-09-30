#!/usr/bin/env luajit
-- temperature conversion table (celsius to farenheit)

for c0=-20,50-1,10 do
	io.write("C ")
	for c=c0,c0+10-1 do
		io.write(string.format("%3.1f ",c))
	end
	io.write("\n")
	
	io.write("F ")
	for c=c0,c0+10-1 do
		f=(9/5)*c+32
		io.write(string.format("%3.1f ",f))
	end
	io.write("\n\n")
end
