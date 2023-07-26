:: start SUMO
start sumo-gui -c .\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 10 --num-clients 1

:: start TrafficLayer
start cmd /k ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f config_SUMOD.yaml

:: start CoordMerge
start cmd /k ..\..\ControlLayer\x64\Release\CoordMerge.exe -f config_SUMOD.yaml -c ControlZone_SUMO.txt
