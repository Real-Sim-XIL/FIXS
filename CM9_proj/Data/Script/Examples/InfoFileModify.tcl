##
## InfoFileModify.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how to modify parameter files (InfoFile keys).
##
## IFileModify <file> <key> <value>
##     <file>  used to determine which infofile should be modified.
##             Values like "Vehicle", "TestRun" or "Brake" can be used,
##             for a full list of supported values see Programmer's Guide
##             "Infofile Parameter Access".
##     <key>   Infofile key name
##     <value> new value of the InfoFile key
##     IFileModify sets the value of the specified string key.
##  
## The Test Run must be loaded prior to calling the function.
##
## Id

Log "* The test run is loaded"
LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"

Log "* Create a working copy of the loaded TestRun"
SaveTestRun "MyOwnTestRun"

Log ""
Log "* Modify parameters (vehicle load 0)"
set mass0 [IFileRead TestRun "VehicleLoad.0.mass"]
set pos0  [IFileRead TestRun "VehicleLoad.0.pos"]

IFileModify TestRun "VehicleLoad.0.mass" 500 
IFileModify TestRun "VehicleLoad.0.pos" "1.5 0 1.5" 

Log "* Flush modifications and run simulation..."
IFileFlush

StartSim
WaitForStatus running
Sleep 10000
StopSim
WaitForStatus idle 10000

Log ""
Log "* Reset parameters to the original values."
IFileModify TestRun "VehicleLoad.0.mass" $mass0 
IFileModify TestRun "VehicleLoad.0.pos"  $pos0 

Log "* Flush modifications and run simulation..."
IFileFlush

StartSim
WaitForStatus running
Sleep 10000
StopSim

