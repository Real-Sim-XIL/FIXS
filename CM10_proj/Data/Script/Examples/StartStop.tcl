##
## StartStop.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
## 
## Load, start and stop test runs
## 
## Id

Log "* Run 15 seconds of Hockenheim"
LoadTestRun "Examples/VehicleDynamics/Handling/Racetrack_Hockenheim"
StartSim
WaitForStatus running
WaitForTime 15

StopSim
WaitForStatus idle 10000

Log "* Run 15 seconds of LaneChangeISO"
LoadTestRun "Examples/VehicleDynamics/Handling/LaneChange_ISO"
StartSim
WaitForStatus running
WaitForTime 15

StopSim

