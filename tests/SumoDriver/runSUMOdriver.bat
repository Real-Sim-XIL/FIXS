
:: start SUMO
start sumo-gui -c .\osm.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump osm_out.xml --netstate-dump.precision 5 --num-clients 1
REM start sumo-gui -c .\osm.sumocfg --step-length 0.1 --start --netstate-dump osm_out.xml --netstate-dump.precision 5 --num-clients 1

:: start TrafficLayer
::start cmd /k ..\..\realsimreleaseford\TrafficLayer.exe -f config_SUMOdriver.yaml

:: start Simulink dummy client
::start cmd /c matlab -nodesktop -nosplash -r "configFilename = '.\config_SUMO.yaml'; runMatlabClient_SUMO; "