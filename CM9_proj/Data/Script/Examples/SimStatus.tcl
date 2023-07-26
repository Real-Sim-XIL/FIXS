##
## SimStatus.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
## 
## This example illustrates the use of the SimStatus command.
##
## In Verbose mode, you can see the output in the CarMaker log file.
##
## The Sleep() function are used to slow down execution and make it 
## more viewable. They are normally not necessary. 
##
## Id

LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"

set status [SimStatus]
Log "Simulation status: $status\n"

Log "Start simulation"
StartSim

Log "Wait until the simulation runs"
WaitForStatus running
set status [SimStatus]
Log "Simulation status: $status\n"

Log "Simulate 10 seconds"
Sleep 10000

Log "Stop the simulation"
StopSim

Log "Wait until the simulation stops"
WaitForStatus idle
set status [SimStatus]
Log "Simulation status: $status\n"

