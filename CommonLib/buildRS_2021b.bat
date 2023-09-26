start cmd /d /e:on /k ""C:\Program Files\dSPACE RCPHIL 2021-B\Exe\dSPACE_vars.bat " & ""dsmake -f DsBuildLibrary_2021b.mk output_filename=RealSimDsLib_2021b source_files="SocketHelper.cpp MsgHelper.cpp VirEnvHelper.cpp VirEnv_Wrapper.cpp" custom_cpp_options="-std=c++11 -IC:\IPG\carmaker\win64-11.0.1\include -DDSRTLX -DRS_DSPACE -DRS_CAVE -DRS_DEBUG" target=Dsx86_32"" " 

REM start cmd /d /e:on /k ""C:\Program Files\dSPACE RCPHIL 2019-B\Exe\dSPACE_vars.bat " & "dsmake -f DsBuildLibrary_2019b.mk output_filename=RealSimDsLib source_files="SocketHelper.cpp MsgHelper.cpp VirEnvHelper.cpp VirEnv_Wrapper.cpp" custom_cpp_options="-std=c++11 -IC:\IPG\carmaker\win64-11.0.1\include -DDSRT -DRS_DSPACE -DRS_CAVE -DRS_DEBUG" target=Dsx86_32" " 

REM pause
