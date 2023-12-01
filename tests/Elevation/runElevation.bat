
set RealSimPath=..\..\
set configFilename=RS_config.yaml
set CmProjPath=..\..\CM11_proj

start sumo-gui -c .\elevation.sumocfg --remote-port 1337 --step-length 0.1 --start 

start cmd /c %RealSimPath%\TrafficLayer.exe -f %configFilename%

call conda activate realsimdev
python %RealSimPath%\CarMaker\RealSimSetCarMakerConfig.py --cm-project-path %CmProjPath% --configFile %configFilename%
call conda deactivate

pause