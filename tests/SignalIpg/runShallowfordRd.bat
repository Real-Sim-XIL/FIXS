
REM start sumo-gui -c ./sumoFiles/ShallowfordRd_RL.sumocfg --remote-port 1337 --step-length 0.1 --start --begin 39600
start sumo-gui -v t -W t -l t -c ./sumoFiles/ShallowfordRd_RL.sumocfg --remote-port 1337 --step-length 0.1 --start --begin 39600

REM start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f RS_config.yaml

pause