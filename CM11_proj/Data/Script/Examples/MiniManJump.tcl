##
## MiniManJump.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how to jump from one mini manuever to the next.
##
## ManJump <man_no>
##     <man_no> number of minimaneuver step (0 ist first mini-maneuver)
##
## Id

Log "* Load Test Run and start simulation"
LoadTestRun "Examples/VehicleDynamics/Braking/Braking"

StartSim
WaitForStatus running
Sleep 10000

# jump to mini maneuver 1
Log "* Jump to mini maneuver 1"
ManJump 1

Sleep 5000

# jump to mini maneuver 0
Log "* Jump back to mini maneuver 0"
ManJump 0

Sleep 5000

# jump to mini maneuver 1
Log "* Jump to mini maneuver 1"
ManJump 1

Sleep 2000 

# jump to mini maneuver 0
Log "* Jump back to mini maneuver 0"
ManJump 0

# wait for the simulation to stop
WaitForStatus idle

