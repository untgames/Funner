function NuBatchCalculation (nu1_start, nu1_end, nu1_step, nu2_start, nu2_end, nu2_step, nu3_start, nu3_end, nu3_step, lod, point_equal_epsilon)
  Modeler.CleanBatchCalculationList ()
  
  for nu1 = nu1_start, nu1_end, nu1_step do
	  for nu2 = nu2_start, nu2_end, nu2_step do
		  for nu3 = nu3_start, nu3_end, nu3_step do
		    print ("Adding calculation " .. nu1 .. " " .. nu2 .. " " .. nu3)
        Modeler.AddBatchCalculation (nu1, nu2, nu3)
		  end
	  end
  end
  
  Modeler.RunBatchCalculation (lod, point_equal_epsilon)
end

function LoadTrajectoryWrapper (nu1, nu2, nu3, epsilon)
  if (not (Modeler.LoadTrajectoryIfExist (nu1, nu2, nu3, epsilon))) then
    print ("Trajectory with nu1 = " .. nu1 .. " nu2 = " .. nu2 .. " nu3 = " .. nu3 .. " was not calculated")
  end
end
