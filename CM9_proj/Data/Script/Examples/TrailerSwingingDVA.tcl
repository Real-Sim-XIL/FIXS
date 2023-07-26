##
## TrailerSwingingDVA.tcl
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This example shows how to set the value of a quantity using DVA.
## The functions introduced are:
##
## DVAWrite <Name> <Value> <Duration> <Mode> ...
##     <Name>     Quantity name
##     <Value>    New quantity value
##     <Duration> Duration in milliseconds 
##     <Mode>     One of Abs, Off, Fac, AbsRamp, ...; default Abs(olute Value)
##     DVAWrite sets the value of the specified quantity using DVA
##
## DVAReleaseQuants
##     Releases all quantities from DVA control. A quantity won't be reset,
##     so unless it is modified by some internal function it will keep the
##     value last given by DVA.
##

# Declare variables and get access to some predefined globals
QuantSubscribe DM.Steer.Ang   ;# implicitly includes Time

set duration 2

Log screen "\n         *** DVA Example Start ***\n" 

# Load the testrun
LoadTestRun "Examples/BasicFunctions/TestAutomation/ScriptControl/Straight_TrailerSwingingDVA"

# Start the simulation
StartSim

# Wait until the simulation is running
WaitForStatus running

set t 10
WaitForCondition {$Qu(Time)>=$t || [SimStatus]<0}

for {set i 0} {[SimStatus] >= 0} {incr i} {
    DVAWrite DM.Steer.Ang $i $duration Abs
    Log screen "Wheel turned to Angle $i rad.\n"

    incr t 10
    WaitForCondition {$Qu(Time)>=$t || [SimStatus]<0}
}

Log screen "
       +++++++++++++++++++++++++++++++++++++++++
       Car went off the road at: 
               
       Time                 =  $Qu(Time) seconds
       Steering Wheel Angle =  [expr $i-1] rad
       Duration             =  $duration ms
       +++++++++++++++++++++++++++++++++++++++++"

DVAReleaseQuants

Log screen "\n         *** DVA Example End ***\n\n" 

