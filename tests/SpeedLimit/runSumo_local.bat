REM start sumo-gui -c .\speedLimit.sumocfg --remote-port 1337 --step-length 0.1 --start --num-clients 1

REM REM start sumo-gui -c .\speedLimit.sumocfg --step-length 0.1 --start --netstate-dump speedLimit.xml 


set RealSimPath=..\..\
set configFilename=config_sumo_local.yaml
set CmProjPath=..\..\CM11_proj

start sumo-gui -c .\speedLimit.sumocfg --remote-port 1337 --step-length 0.1 --start

start cmd /c %RealSimPath%\TrafficLayer.exe -f %configFilename%

call conda activate realsimdev
python %RealSimPath%\CarMaker\RealSimSetCarMakerConfig.py --cm-project-path %CmProjPath% --configFile %configFilename% --signal-table-path %CmProjPath%\Data\Road\speedLimit_RSsignalTable.csv
call conda deactivate

pause