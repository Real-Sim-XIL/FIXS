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


start sumo-gui -c ..\..\tests\SumoIpg\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1
start cmd /c ..\..\TrafficLayer\x64\Debug\TrafficLayer.exe -f ..\..\tests\SumoIpg\config_SUMO.yaml