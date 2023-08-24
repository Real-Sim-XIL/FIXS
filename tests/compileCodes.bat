msbuild ..\TrafficLayer\TrafficLayer.sln /p:Configuration=Release
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> TrafficLayer built success>> testsResults.log 
)else (
	echo TrafficLayer built failed>> testsResults.log 
	exit -1
)

msbuild ..\ControlLayer\ControlLayer.sln /target:CoordMerge /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> ControlLayer built success>> testsResults.log 
)else (
	echo ControlLayer built failed>> testsResults.log 
	exit -1
)

msbuild ..\VISSIMserver\VISSIMserver.sln /target:DriverModel_RealSim /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> DriverModel_RealSim built success>> testsResults.log 
)else (
	echo DriverModel_RealSim built failed>> testsResults.log 
	exit -1
)

msbuild ..\VISSIMserver\VISSIMserver.sln /target:DriverModel_RealSim_v2021 /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> DriverModel_RealSim_v2021 built success>> testsResults.log 
)else (
	echo DriverModel_RealSim_v2021 built failed>> testsResults.log 
	exit -1
)

msbuild ..\VirtualEnvironment\VirtualEnvironment.sln /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> VirtualEnvironment built success>> testsResults.log 
)else (
	echo VirtualEnvironment built failed>> testsResults.log 
	exit -1
)

:: Build CarMaker 11
msbuild ..\CM11_proj\src\CarMaker.sln /target:CarMaker /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker11 built success>> testsResults.log 
)else (
	echo CarMaker11 built failed>> testsResults.log 
	exit -1
)

msbuild "..\CM11_proj\src_cm4sl\CarMaker for Simulink.sln" /target:"CarMaker for Simulink" /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker11 Simulink built success>> testsResults.log 
)else (
	echo CarMaker11 Simulink built failed>> testsResults.log 
	exit -1
)

:: Build CarMaker 10
msbuild ..\CM10_proj\src\CarMaker.sln /target:CarMaker /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker10 built success>> testsResults.log 
)else (
	echo CarMaker10 built failed>> testsResults.log 
	exit -1
)

msbuild "..\CM10_proj\src_cm4sl\CarMaker for Simulink.sln" /target:"CarMaker for Simulink" /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker10 Simulink built success>> testsResults.log 
)else (
	echo CarMaker10 Simulink built failed>> testsResults.log 
	exit -1
)

:: Build CarMaker 9
msbuild ..\CM9_proj\src\CarMaker.sln /target:CarMaker /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker9 built success>> testsResults.log 
)else (
	echo CarMaker9 built failed>> testsResults.log 
	exit -1
)

msbuild "..\CM9_proj\src_cm4sl\CarMaker for Simulink.sln" /target:"CarMaker for Simulink" /p:Configuration=Release 
set BUILD_STATUS=%ERRORLEVEL% 
if %BUILD_STATUS%==0 (
	echo ===^> CarMaker9 Simulink built success>> testsResults.log 
)else (
	echo CarMaker9 Simulink built failed>> testsResults.log 
	exit -1
)


exit 0