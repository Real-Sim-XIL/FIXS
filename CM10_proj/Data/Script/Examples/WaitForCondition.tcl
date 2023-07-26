##
## WaitForCondition.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## This Example shows how to use the WaitForCondition function.               
## It is important to note that any variables that are passed in the          
## conditional statement must be global variables. In this example            
## the Qu array, declared with global scope, is used. WaitForCondition
## will wait for the velocity of the car to exceed 30 m/s. Once the velocity  
## is greater than 30 m/s, the speed is printed and the simulation is stopped.
## 
## Id


### Subscribe all needed quantities
QuantSubscribe Car.v

## Load the test run
LoadTestRun "Examples/VehicleDynamics/Handling/LapTimeOptimization"

## Make sure the previous simulation is idle then start sim 
WaitForStatus idle
StartSim
WaitForStatus running

WaitForCondition {$Qu(Car.v) > 10}
Log "The Velocity is $Qu(Car.v) m/s"

WaitForCondition {$Qu(Car.v) > 20}
Log "The Velocity is $Qu(Car.v) m/s"

WaitForCondition {$Qu(Car.v) > 30}
## Print the Velocity and stop the simulation. 
Log "The Velocity is $Qu(Car.v) m/s"
StopSim

WaitForStatus idle

