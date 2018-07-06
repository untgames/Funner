local function print_rational (r)
  print ("numerator = " .. r.Numerator)
  print ("denominator = " .. r.Denominator)
  print ("to number = " .. r:ToNumber ())
end

function test ()
  local half = Xtl.Rational.LongLong.Create (1, 2)
  local zero = Xtl.Rational.LongLong.Create ()
  local one  = Xtl.Rational.LongLong.Create (1)
  local two  = Xtl.Rational.LongLong.Create (2)

  --create  
  print ("half:")
  print_rational (half)
  print ("zero:")
  print_rational (zero)
  print ("one:")
  print_rational (one)
  print ("two:")
  print_rational (two)
  
  --arithmetic
  print ("half + half")
  print_rational (half + half)
  print ("one - half")
  print_rational (one - half)
  print ("two * half")
  print_rational (two * half)
  print ("one / half")
  print_rational (one / half)
  
  print ("half + half == one: " .. tostring (half + half == one))
  print ("one - half == half: " .. tostring (one - half == half))
  print ("two * half == one: " .. tostring (two * half == one))
  print ("one / half == two: " .. tostring (one / half == two))
  
  print ("1 + one == two: " .. tostring (1 + one == two))
  print ("1 - half == half: " .. tostring (1 - half == half))
  print ("2 * half == one: " .. tostring (2 * half == one))
  print ("1 / half == two: " .. tostring (1 / half == two))
  
  print ("one + 1 == two: " .. tostring (one + 1 == two))
  print ("two - 1 == one: " .. tostring (two - 1 == one))
  print ("half * 2 == one: " .. tostring (half * 2 == one))
  print ("one / 2 == half: " .. tostring (one / 2 == half))

  --comparison
  print ("half < one: " .. tostring (half < one))
  print ("one > half: " .. tostring (one > half))
  print ("half > one : " .. tostring (half > one))
  print ("one < half: " .. tostring (one < half))
  
  --sign
  local minus_half = Xtl.Rational.LongLong.Create (-1, 2)

  print ("minus half:")
  print_rational (minus_half)
  
  print ("-half == minus_half: " .. tostring (-half == minus_half))
  print ("minus_half.Abs () == half: " .. tostring (minus_half:Abs () == half))
end
