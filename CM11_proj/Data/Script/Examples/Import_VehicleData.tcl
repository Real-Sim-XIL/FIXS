##
## Import Vehicle Data
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## Syntax: Car::Import <FileName> <DataList>
##
## Known Options for DataList:
##      Additional
##      Assembly
##      Assembly.Body
##      Assembly.Chassis
##      Assembly.Configuration
##      Assembly.ElasticMount
##      Assembly.Powertrain
##      Assembly.TrimLoad
##      Body
##      Body.Aerodynamics
##      Body.OuterShell
##      Body.Structure
##      Brake
##      Buffer.Front
##      Buffer.Rear
##      Clutch
##      ControlUnit
##      ControlUnit.BCU
##      ControlUnit.ECU
##      ControlUnit.MCU
##      ControlUnit.PTControl
##      ControlUnit.TCU
##      Damper.Front
##      Damper.Rear
##      Driveline
##      Driveline.DiffCenter
##      Driveline.DiffCenterFront
##      Driveline.DiffCenterRear
##      Driveline.DiffFront
##      Driveline.DiffFront2
##      Driveline.DiffHangOn
##      Driveline.DiffRear
##      Driveline.DiffRear2
##      ElectricMotor
##      Engine
##      GearBox
##      Kinematics.Front
##      Kinematics.Rear
##      MCDriveline
##      PlanetGear
##      PowerSupply
##      Powertrain
##      Retarder
##      SecSpring.Front
##      SecSpring.Rear
##      Sensor.Camera
##      Sensor.Collision
##      Sensor.FSpace
##      Sensor.GNav
##      Sensor.Inertial
##      Sensor.LidarRSI
##      Sensor.Line
##      Sensor.ObjByLane
##      Sensor.Object
##      Sensor.Radar
##      Sensor.RadarRSI
##      Sensor.RadarRSILeg
##      Sensor.Road
##      Sensor.SAngle
##      Sensor.TSign
##      Sensor.USonicRSI
##      Sensors
##      Spring.Front
##      Spring.Rear
##      Stabilizer.Front
##      Stabilizer.Rear
##      StarterMotor
##      Steering
##      Suspensions
##      Tires
##      VehicleControl
##      WhlBearing.Front
##      WhlBearing.Rear
##
## Id

set lst {Steering Brake Additional Spring.Front Pushrod}

set FName Examples/Demo_Ford_Focus
set ierr [Car::Import $FName $lst]
Log "# 1: $FName -> $ierr\n"

set FName Examples/Demo_Jaguar_XType
set ierr [Car::Import $FName $lst]
Log "# 2: $FName -> $ierr\n"

set lst {Steering Brake Additional Spring.Front}

set FName Examples/Demo_Lexus_NX300h
set ierr [Car::Import $FName $lst]
Log "# 3: $FName -> $ierr\n"

set lst {Steering Brake Body.Aerodynamics Additional Spring.Front Damper.Rear}

set FName Examples/Demo_Porsche_911
set ierr [Car::Import $FName $lst]
Log "# 4: $FName -> $ierr\n"

set FName Examples/DemoCar
set ierr [Car::Import $FName]
Log "# 5: $FName -> $ierr\n"

