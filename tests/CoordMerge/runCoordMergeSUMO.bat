:: start SUMO
start sumo -c .\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 10 --num-clients 1

:: start TrafficLayer
start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f config_SUMO.yaml

:: start CoordMerge
start cmd /c ..\..\ControlLayer\x64\Release\CoordMerge.exe -f config_SUMO.yaml -c ControlZone_SUMO.txt
