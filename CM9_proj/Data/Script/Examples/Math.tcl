##
## Math.tcl  --  built-in math functions
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## Example illustrates the use of the built-in math functions
##
## Id

# load the test run
LoadTestRun "Examples/VehicleDynamics/Handling/LapTimeOptimization"

# subscribe all needed quantities
QuantSubscribe { Time Car.v Sensor.SAngle.SL00.Ang }

# start the test run
StartSim
WaitForStatus running
Log "* Simulation started..."

# wait until Time is ~13 seconds
WaitForCondition {$Qu(Time) > 13}
Log "* After ~13 seconds the values are:\n"
 
# after ~13 seconds set the new variables 
set Velocity      $Qu(Car.v)
set InstSlipAng   $Qu(Sensor.SAngle.SL00.Ang)

# print the formatted values to the screen
Log [format "  The Side Slip Angle is   : %7.3f rad" $InstSlipAng]
Log [format "  The Velocity is          : %7.3f m/s" $Velocity]

# stop the simulation
Log "\n* Stop the simulation\n"
StopSim


# get the Arc-cosine  
set SlipAcos [expr acos($InstSlipAng)]
Log [format "Arc-cosine of Side Slip  : %7.3f" $SlipAcos]

# get the Arc-sine
set SlipAsin [expr asin($InstSlipAng)]
Log [format "Arc-sine of Side Slip    : %7.3f" $SlipAsin]

# get the Arc-tangent
set SlipAtan [expr atan($InstSlipAng)]
Log [format "Arc-tangent of Side Slip : %7.3f" $SlipAtan]

# get the Cosine
set SlipCos [expr cos($InstSlipAng)]
Log [format "Cosine of Side Slip      : %7.3f" $SlipCos]

# get the Hyperbolic cosine
set SlipHcos [expr cosh($InstSlipAng)]
Log [format "The Hyperbolic cosine is : %7.3f" $SlipHcos]

# get the exponential, e^x
set SlipExp [expr exp($InstSlipAng)]
Log [format "The exponential is       : %7.3f" $SlipExp]

# get the natural log
if {$InstSlipAng > 0} {
    set SlipLog [expr log10($InstSlipAng)]
    Log [format "The base 10 log is       : %7.3f" $SlipLog]
} else {
    Log         "The natural log is       : n/a"
}

# get the base 10 log
if {$InstSlipAng > 0} {
    set SlipLn [expr log($InstSlipAng)]
    Log [format "The natural log is       : %7.3f" $SlipLn]
} else {
    Log         "The base 10 log is       : n/a"
}

# get the sine
set SlipSin [expr sin($InstSlipAng)]
Log [format "The sine is              : %7.3f" $SlipSin]

# get the hyperbolic sine
set SlipHsin [expr sinh($InstSlipAng)]
Log [format "The hyperbolic sine is   : %7.3f" $SlipHsin]

# get the square root

set SlipSqRt [expr sqrt($InstSlipAng)]
Log [format "The square root is       : %7.3f" $SlipSqRt]

# get the tangent
set SlipTan [expr tan($InstSlipAng)]
Log [format "The tangent is           : %7.3f" $SlipTan]

# get the hyperbolic tangent
set SlipHtan [expr tanh($InstSlipAng)]
Log [format "The hyperbolic tangent is: %7.3f" $SlipHtan]

# get the absolute value
set SlipAbs [expr abs($InstSlipAng)]
Log [format "The absolute value is    : %7.3f" $SlipAbs]

# get the least integral value greater than or equal to InstSlipAng
set SlipCeil [expr ceil($InstSlipAng)]
Log [format "Ceiling of Side Slip     : %7.3f" $SlipCeil]

# get the floor
set SlipFloor [expr floor($InstSlipAng)]
Log [format "The floor is             : %7.3f" $SlipFloor]

# truncate to integer
set SlipInt [expr int($InstSlipAng)]
Log [format "The truncated integer is : %7.3f" $SlipInt]

# round to an integer
set SlipRound [expr round($InstSlipAng)]
Log [format "The rounded integer is   : %7.3f" $SlipRound]


# calculate and print the value of some random formula
set x [expr $SlipLn * sqrt($Velocity)]
Log [format "\nThe natural log of the side slip angle multiplied
by the square root of the velocity is: %7.3f" $x]

