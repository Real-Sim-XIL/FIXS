##
## TestManager.tcl
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how to automatically create and run a test series with
## the help of the new TestManager ScriptControl API.
##

# Start a new test series
TestMgr new

# Add a TestRun
TestMgr additem TestRun "Examples/BasicFunctions/TestAutomation/TestManager/Runs/Braking" \
    -param {
	{Brake NValue}
     }

# Add a Characteristic Value to the TestRun
TestMgr additem Characteristic "Brake Distance Calculation" \
    -desc "A new quantity \"BrakeDist\" records the distance travelled by the test car after the braking maneuver has started." \
    -ident "BrakeDist" -unit "m" \
    -param {
	{RTexpr {(first () ? Qu::BrakeDist=0); DM.ManNo>0 ? BrakeDist=Delta2Ev(Car.Road.sRoad, change(DM.Brake), Car.v <=0.01)}}
     }

# Add a Criterion for evalualtion of the TestRun
TestMgr additem Criterion "Brake Distance" \
    -desc "TestRun evaluation is carried out on the basis of Brake Distance. TestRun is considered good if Brake Distance < 14m." \
    -good {[get BrakeDist] <  27.0} \
    -warn {[get BrakeDist] >= 27.0 && [get BrakeDist] < 32.0} \
    -bad  {[get BrakeDist] >= 32.0}

# Add a Diagram to the TestRun
TestMgr additem Diagram "Brake Distance vs Time" \
    -mode "Quantity vs Quantity" -type "Line Diagram" -grid "Both" -allvars 1 \
    -axisdata {
	{Manual 40 80 Time}
	{Auto "" "" Brake Distance}
     } \
    -contentdata {
	{Time "" "" ""}
	{BrakeDist "" "" ""}
     }

# Add three variations
TestMgr additem Variation "Variation 0" -param {0.5}
TestMgr additem Variation "Variation 1" -param {0.7}
TestMgr additem Variation "Variation 2" -param {0.9}

# Start the test series
TestMgr start -async
