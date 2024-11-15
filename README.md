# Flexible Interface for XIL Simulation (FIXS)

## FIXS Contacts
**Primary Maintainer**: University of Georgia\
realsimxil@gmail.com

## About
This interface is part of the **Real-Sim** project funded by Department of Energy (DOE)-Vehicle Technology Office (VTO)-Energy Efficient Mobility Systems (EEMS) program. It is part of the core tools of EEMS.

Real-Sim develops a multi-resolution X-in-the-loop (XIL) simulation framework to support inclusive testing and evaluation of emerging technologies such as connected and automated vehicles. The Real-Sim framework connects different simulation components and integrates various traffic and vehicle simulation tools, virtual environments, and XIL systems through a Flexible Interface for XIL Simulation (FIXS). Connections among different tools are transparent to users and handled by the interface behind the scenes. Co-simulation of various vehicle and traffic simulation tools with different XIL systems can be easily achieved and become a transparent "plug-and-play" process to users. 

**Real-Sim Project Lead**: Oak Ridge National Lab\
**Real-Sim Project Team**: University of Georgia, University of Texas-Dallas

**Links to project presentation**\
https://www1.eere.energy.gov/vehiclesandfuels/downloads/2024_AMR/EEMS101_Chen_2024_o.pdf \
https://www1.eere.energy.gov/vehiclesandfuels/downloads/2023_AMR/eems101_Shao_2023_o_v2%20-%20Yunli%20Shao.pdf_ADW373C.tmp.pdf

Publication list is upcoming...

# Other links
This README contains general information of the interface. For specific documentation of different simulators, check the following links:
* ### [VISSIM Interface](doc/VISSIMdoc.md)
* ### [SUMO Interface](doc/SUMOdoc.md)
* ### [CarMaker](doc/CarMakerDoc.md)

# Table of Contents
* [Preface](#preface)
* [Build](#build)
   * [Prerequisite](#prerequisite)
   * [Dispatch a Release](#dispatch-a-release)
* [General Setups](#general-setups)
* [Compatibility to Previous Versions](#compatibility-to-previous-versions)
    * [RealSimDepack Output](#realsimdepack-output)
* [VISSIM and SUMO comparison](#vissim-and-sumo-comparison)
* [Troubleshooting](#troubleshooting)
* [Appendix](#appendix)
    * [Vehicle Message Field Specifications](#vehicle-message-field-specifications)

## Preface
Before testing the Real-Sim interface, please 
- read through this README
- check comments in config.yaml which specifies how to properly write/modify the configuration file
- check annotations in the Simulink template which specifies how to properly use each block
- RealSimPack.m and RealSimDepack.m files are currently open-source. check the comments before modifying.  

## General Setups
The interface runs the connections to different software, simulators by itself to provide plug-in-and-play experience for the users. A config.yaml file is critical to setup the interface parameters and configure different scenarios. 

**NOTE: below is still experimental features and only applies to SUMO for now.**
There are different mode of synchronization and opeartion of the Real-Sim interface. The **SimulationMode** parameter is an integer with each bit as the followings:
- bit 2, bit 1, bit 0: 
    - **binary 000, integer 0**: Application/Xil will connect and sync with Simulator (SUMO/VISSIM) at the beginning of the simulation
    - **binary 001, integer 1**: Application/Xil will connect with the Simulator and in wait mode until the ego vehicle enters the network then sync. 

        This is suitable for simulation environment where users want to run the SUMO simulation at fast pace without running a Simulink model until the ego vehicle enters. 
<!--    - **binary 010, integer 2**: Application/Xil will connect with the Simulator and running steps but NOT sync with the Simulator until the ego vehicle enters the network

        This is suitable for XIL environment where users want to run the VISSIM/SUMO simulation at fast pace before the ego vehicle enters the network, but the Simulink model needs to also starts at beginning. Before ego vehicle enters, Simulink and VISSIM/SUMO are both running without any message sharing. After ego vehicle enters, the ego vehicle in Simulink will then sync with the same one in VISSIM/SUMO. 
-->
    - **binary 100, integer 4**: Application/Xil will connect with the Simulator and in wait mode until the specified initial simulation seconds (parameter **SimulationModeParamter**) then sync

        This is suitable for simulation environment where users want to run the SUMO simulation at fast pace without running a Simulink model until the a specific seconds later.
        You could add ramp up and ramp down in the Simulink model and manipulate the SimulationModeParamter settings so that Simulink and SUMO sychronized only after the ego vehicle in Simulink reaches initial speed. 

<!--   - **binary 101, integer 5**: Application/Xil will connect with the Simulator and running steps but NOT sync with the Simulator until the specified initial simulation seconds (parameter **SimulationModeParamter**) then sync

        This is suitable for XIL environment where users want to run the VISSIM/SUMO simulation at fast pace before a specific second, but the Simulink model needs to also starts at beginning. Before the specific second, Simulink and VISSIM/SUMO are both running without any message sharing. After the specific second, the ego vehicle in Simulink will then sync with the same one in VISSIM/SUMO. 
-->

SimulationModeParamter is a double variable that currently only used for mode binary 100, integer 4 and binary 101, integer 5

## Build
This section is about how to build the source code, and then how to dispatch a released executable version.

### Prerequisite
Several libraries need to be compiled first. You can use the ```compileExternalLibraries.bat``` to do the following steps automatically or manually execute the following steps.

Build libevent 
https://github.com/libevent/libevent/blob/master/Documentation/Building.md#building-on-windows

```
cd .\CommonLib\libevent
md build && cd build
cmake -G "Visual Studio 16 2019" -DEVENT__DISABLE_MBEDTLS=ON ..   # Or use any generator you want to use. Run cmake --help for a list
cmake --build . --config Release # Or "start libevent.sln" and build with menu in Visual Studio.
```

Note: build in Release version if also compiling Release version of TrafficLayer.exe, CoordMerge.exe, etc. build in Debug if compiling Debug version of TrafficLayer.exe, CoordMerge.exe, etc.

Build yaml-cpp
https://github.com/jbeder/yaml-cpp
```
cd .\CommonLib\yaml-cpp
md build && cd build
cmake -G "Visual Studio 16 2019" ..   # Or use any generator you want to use. Run cmake --help for a list
cmake --build . --config Release # Or "start libevent.sln" and build with menu in Visual Studio.
```

### Dispatch a release
The source code uses ```msbuild``` as the default compiler, command ```msbuild``` must be known in the environmental variable before dispatching. The path to be added is ```%ProgramFiles(x86)%\Microsoft Visual Studio\2019\<YOUR_VS_EDITION>\MSBuild\Current\Bin```.

Additionally, python >= 3.8 is required. It is recommended to create a dedicated conda environment and name it as ```realsimdev```.

With the above, run ```dispatch.bat``` will dispatch released executables inside a ```build``` folder.

## Compatibility to Previous Versions
Here are potential compatibility issues with different versions

### RealSimDepack Output
Instead of multiple outputs, now RealSimDepack will output one single bus signal,

## VISSIM and SUMO comparison
To be filled...

## Troubleshooting
- If in any case an issue occurs when using the interface, first try to restart VISSIM and MATLAB and try again.

- If issue persists, turn on the EnableVerboseLog in the config.yaml, enable log of the RealSimSocket s-function, repeat the simulation that causes the issue. Then locate the following logs and error reports and check them (some of the logs will be overwritten when a new simulation starts): 
    - TrafficLayer.err in the directory of TrafficLayer.exe
    - Copy/Screenshot the outputs displaying from the cmd window running the TrafficLayer.exe
    - DriverModelError.txt and DriverModelLog.txt in the directory of the VISSIM network file
    - Copy/Screenshot outputs from the 'Diagnostic Viewer' of the Simulink. 

## Appendix
### Vehicle Message Field Specifications
This is a full list of vehicle message field
```cpp
std::string id; // VISSIM id is integer and will be converted to string, e.g., (int) 8 ==> (string) "8"
std::string type; // VISSIM type is integer and will be converted to string, e.g., (int) 100 ==> (string) "100"
std::string vehicleClass; // vehicle class, such as car, truck in VISSIM; private, passenger, truck in SUMO
float speed; // m/s
float acceleration; // m/s^2
float positionX; // m, position based on the coordinates defined in VISSIM/SUMO
float positionY; // m
float positionZ; // m
float heading; // heading in degree, north is 0 degree, then increasing clockwise. i.e., east is 90 degree.
uint32_t color;  // combined bits in order of r,g,b,a, each is 8-bit, i.e., leftmost 8-bit is r
std::string linkId; // VISSIM linkId is integer and will be converted to string, e.g., (int) 10001 ==> (string) "10001"
int32_t laneId; // lane id convention is consistent to VISSIM, with rightmost lane to be 1
float distanceTravel; // m, cumulative driving distance of each vehicle since entering VISSIM network
float speedDesired; // m/s
float accelerationDesired; // m/s^2
int8_t hasPrecedingVehicle; // 1: has preceding vehicle, 0: no preceding vehicle
std::string precedingVehicleId; // 
float precedingVehicleDistance; // m, distance to preceding vehicle (front pump of ego to rear pump of front), -1 if no preceding vehicle
float precedingVehicleSpeed; // m/s, absolute speed of preceding vehicle, -1 if no preceding vehicle
string signalLightId; // id of signal controller
int32_t signalLightHeadId; // index of signal head
float signalLightDistance; // m, distance to next signal light, no nearby signal light = -1;
int8_t signalLightColor; // color of next signal light, red = 1, amber = 2, green = 3, red / amber = 4, amber flashing = 5, off = 6, other = 0, no nearby signal light = -1;
float speedLimit; // m/s, VISSIM speed limit is defined in terms of reduced speed area, desired speed decision. If neither exists, this value will be -1. Similarly for speedLimitNext.
float speedLimitNext; // m/s, -1 if no speed limit ahead. 
float speedLimitChangeDistance; // m, -1 if no speed limit ahead.
std::string linkIdNext; // 
float grade; // angle in radians, positive climbing up hill
int8_t activeLaneChange; // 1 to the left, -1 to the right, 0 stay on the lane, 
```

<!--
<img src="docs/images/Flowchart.PNG" align="middle" width="500"/>
--[]([url](url))>
