#!/usr/bin/env luajit
-- bisection method for solving non-linear equations

delta=1e-6	-- tolerance

function bisect(f,a,b,fa,fb)
 local c=(a+b)/2
 io.write(string.format("%d c=%.5g a=%.5g b=%.5g\n",n,c,a,b))
 if c==a or c==b or math.abs(a-b)<delta then return c,b-a end
 n=n+1
 local fc=f(c)
 if fa*fc<0 then return bisect(f,a,c,fa,fc) else return bisect(f,c,b,fc,fb) end
end

-- find root of f in the inverval [a,b]. needs f(a)*f(b)<0
function solve(f,a,b)
 n=0
 local z,e=bisect(f,a,b,f(a),f(b))
 io.write(string.format("after %d steps, root is %.7g with error %.5f, f=%.5f\n",n,z,e,f(z)))
end

-- our function
function f(x)
 return x*x*x-x-1
end

-- find zero in [1,2]
solve(f,1,2)
