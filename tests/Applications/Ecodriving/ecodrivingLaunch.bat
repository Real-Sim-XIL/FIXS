
:: start SUMO
start sumo-gui -c .\coordMerge.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump coordMerge.xml --netstate-dump.precision 10 --num-clients 1

:: start TrafficLayer
::start cmd /k ..\..\realsimreleaseford\TrafficLayer.exe -f config_SUMOdriver.yaml

:: start Simulink dummy client
::start cmd /c matlab -nodesktop -nosplash -r "configFilename = '.\config_SUMO.yaml'; runMatlabClient_SUMO; "