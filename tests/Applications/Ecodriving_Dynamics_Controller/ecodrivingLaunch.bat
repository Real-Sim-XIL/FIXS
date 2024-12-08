
:: start SUMO
start sumo-gui -c .\chatt.sumocfg --remote-port 1337 --step-length 1 --netstate-dump chatt.xml --netstate-dump.precision 10 --num-clients 2  --begin 28985 --end 33000

:: start TrafficLayer
::start cmd /k ..\..\realsimreleaseford\TrafficLayer.exe -f config_SUMOdriver.yaml

:: start Simulink dummy client
::start cmd /c matlab -nodesktop -nosplash -r "configFilename = '.\config_SUMO.yaml'; runMatlabClient_SUMO; "