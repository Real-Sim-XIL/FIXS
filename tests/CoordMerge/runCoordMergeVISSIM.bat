
:: start TrafficLayer
start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f config_VISSIM.yaml

:: start CoordMerge
start cmd /c ..\..\ControlLayer\x64\Release\CoordMerge.exe -f config_VISSIM.yaml -c ControlZone_VISSIM.txt
