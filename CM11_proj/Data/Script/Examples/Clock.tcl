##
## Clock.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## An example showing how to create and use a clock. 
## The clock functions work using the simulation time T and are only
## relevant when a simulation is running. When no simulation is running, 
## the clock functions will not give predictable results. 
## The functions are:
##
## ClockCreate <clock_name> <condition>
##     <clock_name> is a user specified clock name.
##     <condition>  is a boolean condition. All variable used 
##                  must have global scope.
##     ClockCreate creates the clock.
##  
## ClockStart <clock_name>
##     <clock_name> is the name used in ClockCreate
##     ClockStart starts the clock.
##  
## ClockStop  <clock_name>
##     <clock_name> is the name used in ClockCreate
##     ClockStop stops the clock.
##  
## ClockGetTime <clock_name>
##     <clock_name> is the name used in ClockCreate
##     ClockGetTime returns the elapsed clock time. The time can be 
##     read any time after ClockStart is called.
##  
## ClockReset <clock_name>
##     <clock_name> is the name used in ClockCreate
##     ClockReset resets the clock without stopping it.
##  
## Id

# Subscribe all needed Quantities
QuantSubscribe { DM.Steer.Ang Time }

# create the clock
ClockCreate MyClock {$Qu(DM.Steer.Ang) > 0}

# load the test run
LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"

# start the simulation
Log "* Starting Simulation ..."
StartSim

# Wait until the simulation starts 
WaitForStatus running

# start the clock
Log "* Clock Timer started"
ClockStart MyClock

# Print a description to the screen
Log ""
Log "  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
Log "  Now that the timer has started, we will wait until the"
Log "  Simulation Time is greater than 30. When the time reaches"
Log "  30, the clock is stopped and the total time that the "
Log "  Steering Wheel Angle is greater than 0 is displayed. "
Log "  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"


# wait until CarMaker Simulation Time is greater than 30
WaitForCondition {$Qu(Time) > 30}

# stop the clock
ClockStop MyClock
Log "* Clock Timer stopped"

# Get the total time that the steering wheel angle was greater than 0
set TotalTime [ClockGetTime MyClock]

# print the elapsed time
# [ format ... ] is used to round the result 
Log "\nThe SWheel Angle was > 0 for [format %.2f $TotalTime] seconds"

# stop the simulation
StopSim

# don't forget to free resources after use
ClockDelete MyClock

