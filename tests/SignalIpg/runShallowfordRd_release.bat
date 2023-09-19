REM start sumo-gui -c .\sumoFiles\ShallowfordRd_RL.sumocfg --remote-port 1337 --step-length 0.1 --start --begin 39600

REM start cmd /c ..\..\build\TrafficLayer.exe -f RS_config_release.yaml

REM pause

set RealSimPath=..\..\build
set RealSimAppPath=.\sumoFiles
set configFilename=RS_config.yaml
set CmProjPath=..\..\CM11_proj\src_cm4sl_ds

start sumo-gui -c %RealSimAppPath%\ShallowfordRd_RL.sumocfg --remote-port 1337 --step-length 0.1 --start --begin 39600

start cmd /c %RealSimPath%\TrafficLayer.exe -f %configFilename%

call conda activate realsimdev
python %RealSimPath%\CarMaker\RealSimSetCarMakerConfig.py --cm-project-path %CmProjPath% --configFile %configFilename%
call conda deactivate

pause
