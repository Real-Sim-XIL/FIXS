##
## Print.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## Id

# Files with no directory component will go to SimOutput/ScriptLog/<date>
OpenSLog PrintExample1.slog

Log         "This is going to screen, and (if one was opened) to file"
Log file    "This is going in the file (file MUST be opened)"
Log screen  "This is going to the screen"
Log file+   "This is going to file and screen (file MUST be opened)"
Log screen+ "This is going to screen and file (file MUST be opened)"

LogExec         {LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"}
LogExec screen   StartSim
LogExec screen  {Sleep 10000}
LogExec screen   StopSim
LogExec screen  {Sleep 5000}

CloseSLog


# Files with no directory component will go to SimOutput/<hostname>/ScriptLog/<date>
OpenSLog PrintExample2.slog

WaitForStatus idle
Log ""
LogExec file    {LoadTestRun "Examples/VehicleDynamics/Braking/Braking"}
LogExec screen+ {Sleep 1000}
LogExec file     StartSim
LogExec file    {Sleep 10000}
LogExec file     StopSim
LogExec file    {Sleep 5000}

WaitForStatus idle
Log ""
LogExec screen+ {LoadTestRun "Examples/BasicFunctions/Driver/HandlingCourse"}
LogExec screen+ {Sleep 1000}
LogExec screen+  StartSim
LogExec screen+ {Sleep 10000}
LogExec screen+  StopSim
LogExec file    {Sleep 5000}

WaitForStatus idle
Log ""
LogExec file+   {LoadTestRun "Examples/BasicFunctions/Road/Surface/Bumps"}
LogExec screen+ {Sleep 1000}
LogExec file+    StartSim
LogExec file+   {Sleep 10000}
LogExec file+    StopSim

CloseSLog

