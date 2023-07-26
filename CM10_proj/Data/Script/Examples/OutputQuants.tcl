##
## OutputQuants.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how the functions that modify the OutputQuantities file
## are used, and details how saving the simulation data of the selected 
## quantities is automated.  
## The functions are:
##
## OutQuantsAdd <Quantity_List>
## OutQuantsAdd <Q1> <Q2> ...
##     where <Quantity_List> is the list of quantities and and <Qi>
##     is the name of a single quantity to be added
##
##     OutQuantsAdd adds the specified quantities to the OutputQuantities file.
##
##
## OutQuantsDel <Quantity_List>
## OutQuantsDel <Q1> <Q2> ...
##     where <Quantity_List> is the list of quantities and and <Qi>
##     is the name of a single quantity that should be deleted from the
##     OutputQuantities file
##
##     OutQuantsDel removes the specified quantities from the OutputQuantities
##     file.
##
## OutQuantsDelAll 
##     OutQuantsDelAll deletes all quantities from the OutputQuantities file.
##
## OutQuantsRestoreDefs
##     OutQuantsRestoreDefs restores the default quantities defined in
##     the OutputQuantities.default file. The default file is created the
##     first time any change is made to OutputQuantities using one of these 
##     functions, and can be modified manually at any point after it was created.
##     

# subscribe to quantities
QuantSubscribe { Car.v Car.ax }

# start with an empty output quantity list
OutQuantsDelAll

# add new quantities 
OutQuantsAdd Car.v DM.Clutch DM.Gas Time

# load the test run
LoadTestRun "Examples/VehicleDynamics/Handling/LapTimeOptimization"

# set storage mode
SaveMode hist_10s

# start of the simulation
StartSim
WaitForStatus running

# Check the speed every second until the simulation is 
# no longer running. If the speed is greater than 30 m/s
# save the data. Otherwise, stop saving the data until 
# the speed reaches 30 m/s.

# $$ If the code and the comments disagree, then both are probably wrong...
# $$        -- Norm Schryer

set flag "SaveNotStarted"
while {[SimStatus] == 0 && $flag == "SaveNotStarted"} {
    
    if {$Qu(Car.v) > 20.0} {
        if {$Qu(Time) > 30.0} {
	    set flag "SaveStarted"
	}
	SaveStart
	Log "Data being saved..."
	Sleep 1000
    } else {
       Log "Speed is: $Qu(Car.v) m/s"
    }
    Sleep 1000
}

StopSim

# restore the default output quantity list
OutQuantsRestoreDefs    

