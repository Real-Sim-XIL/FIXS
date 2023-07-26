:: start SUMO
start sumo-gui -c .\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 10 --num-clients 1

:: start TrafficLayer
start cmd /c ..\..\TrafficLayer\x64\Debug\TrafficLayer.exe -f config_SUMO.yaml
