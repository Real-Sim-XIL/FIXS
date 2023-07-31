REM set RealSimPath=..\..\
REM set RealSimAppPath=..\RealSimApps\SumoIpg
REM set configFilename=config_RS_noSimulinkRS.yaml

REM start sumo-gui -c %RealSimAppPath%\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 5 --num-clients 1
REM start cmd /c %RealSimPath%\TrafficLayer.exe -f %RealSimAppPath%\%configFilename%

REM use a python conda environment
REM you can directly can the default system python enviornment as well 
REM requires pyyaml, argparse 
REM call conda activate realsimdev
REM python %RealSimPath%\CarMaker\RealSimSetCarMakerConfig.py --configFile %RealSimAppPath%\%configFilename%
REM call conda deactivate

REM pause

REM set RealSimAppPath=..\..\tests\SumoIpg
REM start sumo-gui -c %RealSimAppPath%\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1
REM set RealSimAppPath=..\..\tests\TurnSignal
REM start sumo-gui -c %RealSimAppPath%\turnSignal.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1
REM set RealSimAppPath=..\..\tests\Elevation
REM start sumo-gui -c %RealSimAppPath%\elevation.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1
start sumo-gui -c ..\..\tests\ShallowfordRdSCIL\sumoFiles\ShallowfordRd_RL.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1 --begin 39600
start cmd /c ..\..\TrafficLayer\x64\Debug\TrafficLayer.exe -f ..\..\tests\Elevation\config_SUMO.yaml