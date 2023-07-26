##
## QuantSubscribe.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how to subscribe to quantities, and also 
## how to use the quantity values in a conditional expression. 
##
## Id

# Subscribe the Quantities you want to have access to
QuantSubscribe Car.v

# Load and Start the Simulation
Log "* Load Test Run and start simulation"
LogExec {LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"}
LogExec StartSim
LogExec {WaitForStatus running}


# Print a description to the screen
set ms 9000
Log ""
Log "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
Log "We are waiting for $ms milliseconds according to the host (GUI)"
Log "system clock. After $ms milliseconds we will check the velocity"
Log "of the vehicle. If it is less than 20 m/s we will continue the"
Log "simulation for 5 more seconds and then stop the simulation."
Log ""
Log "HOWEVER, if the speed is greater than 20 m/s after $ms ms"
Log "         we stop the simulation immediately. "
Log "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"     
Sleep $ms


# Check the vehicle speed in a if-else statement
if {$Qu(Car.v) < 20} {
    Log "The speed \"$Qu(Car.v)\" m/s is less than 20 m/s"
    Log "Continuing the Simulation ..." 
    Sleep 5000
    Log "The speed is now \"$Qu(Car.v)\" m/s"   
} else {
    Log "The speed \"$Qu(Car.v)\" m/s is greater than 20 m/s"
}


Log "\n* Stop Simulation"
StopSim

