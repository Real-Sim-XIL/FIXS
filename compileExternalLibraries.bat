cd .\CommonLib\libevent
md build && cd build
cmake -G "Visual Studio 16 2019" -DEVENT__DISABLE_MBEDTLS=ON ..  
cmake --build . --config Release 
cmake --build . --config Debug

pause
REM cd .\CommonLib\yaml-cpp
REM md build && cd build
REM cmake -G "Visual Studio 16 2019" ..  
REM cmake --build . --config Release
REM cmake --build . --config Debug