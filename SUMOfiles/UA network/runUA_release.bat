::sumo-gui -c ./coordMerge_v1.sumocfg --remote-port 1337 --step-length 0.1 --start
::sumo-gui -c ./UAdemoRealSim_Final.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump UAdemoRealSim_Final.xml
::sumo-gui -c ./UAdemoRealSim_Final.sumocfg --step-length 0.1 --start --netstate-dump UAdemoRealSim_Final.xml


REM start sumo-gui -c ./UAdemoRealSim_Final.sumocfg --step-length 0.1 --start --netstate-dump UAdemoRealSim_Orig.xml --netstate-dump.precision 10 --end 70


:: compile TrafficLayer and CoordMerge
::msbuild ..\..\TrafficLayer\TrafficLayer.sln /p:Configuration=Release

:: start SUMO
start sumo-gui -c ./UAdemoRealSim_Final.sumocfg --remote-port 1337 --step-length 0.1 --start --netstate-dump UAdemoRealSim_Final.xml --netstate-dump.precision 10 --num-clients 1

:: start TrafficLayer
start cmd /c ..\..\RealSimRelease\TrafficLayer.exe -f test.yaml

:: start Simulink dummy client
:: start cmd /c matlab -nodesktop -nosplash -r "configFilename = '.\test.yaml'; mainUA; "



