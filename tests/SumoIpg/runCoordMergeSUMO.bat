:: compile TrafficLayer and CoordMerge
::msbuild ..\..\TrafficLayer\TrafficLayer.sln /p:Configuration=Release
::msbuild ..\..\ControlLayer\ControlLayer.sln /target:CoordMerge /p:Configuration=Release 

:: start SUMO
start sumo-gui -c .\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 5 --num-clients 1

REM :: start TrafficLayer
REM ::start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f config_SUMO.yaml
start cmd /c ..\..\TrafficLayer\x64\Debug\TrafficLayer.exe -f config_SUMO.yaml
REM start cmd /k ..\..\realsimreleaseford\TrafficLayer.exe -f config_SUMO.yaml

REM :: start CoordMerge
REM start cmd /k ..\..\realsimreleaseford\CoordMerge.exe -f config_SUMO.yaml -c ControlZoneDefault_SUMO.txt

:: start Simulink dummy client
::start cmd /c matlab -nodesktop -nosplash -r "configFilename = '.\config_SUMO.yaml'; runMatlabClient_SUMO; "