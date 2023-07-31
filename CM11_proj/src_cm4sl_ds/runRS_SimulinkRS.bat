set RealSimPath=..\..\
set RealSimAppPath=..\RealSimApps\SumoIpg
set configFilename=config_RS_SimulinkRS.yaml

start sumo-gui -c %RealSimAppPath%\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 5 --num-clients 1
start cmd /c %RealSimPath%\TrafficLayer.exe -f %RealSimAppPath%\%configFilename%

REM use a python conda environment
REM you can directly can the default system python enviornment as well 
REM requires pyyaml, argparse 
call conda activate realsimdev
python %RealSimPath%\CarMaker\RealSimSetCarMakerConfig.py --configFile %RealSimAppPath%\%configFilename%
call conda deactivate

REM pause