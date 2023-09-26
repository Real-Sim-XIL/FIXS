'''

script to grab .exe and .dll files to RealSimRelease folder
run script within the RealSimRelease folder

'''

import os
import shutil
import argparse

def ReleaseRealSim(args):
    #RealSimSourcePath = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), r'..\..\RealSimInterface'))
    SourcePath = args.realsim_path
    # os.makedirs(os.path.join(os.path.abspath(os.curdir), 'build'), exist_ok=True)
    ReleasePath = os.path.join(os.path.abspath(os.curdir), 'build')

    if not os.path.exists(os.path.join(SourcePath, r'CommonLib\yaml-cpp\build')):
        status=os.system('compileExternalLibraries.bat')

    # compile code
    curPath = os.path.abspath(os.curdir)
    os.chdir(os.path.join(SourcePath, r'tests'))
    status=os.system('compileCodes.bat')
    os.chdir(curPath)
    if status == -1:
        print("==============================")
        print('RealSimError: compile code failed!')
        print("==============================")
        exit()

    # compile RS dSPACE IPG library
    os.chdir(os.path.join(SourcePath, r'CommonLib'))
    import subprocess
    #status=os.system('buildRS_2021b.bat')
    p = subprocess.run(['buildRS_2021b.bat'], capture_output=True)
    os.chdir(curPath)

    # release executable
    shutil.copy(os.path.join(SourcePath, r'TrafficLayer\x64\Release\TrafficLayer.exe'), os.path.join(ReleasePath, 'TrafficLayer.exe'))
    shutil.copy(os.path.join(SourcePath, r'ControlLayer\x64\Release\CoordMerge.exe'), os.path.join(ReleasePath, 'CoordMerge.exe'))

    # release source code
    # create directory first
    os.makedirs(os.path.join(ReleasePath, 'CommonLib'), exist_ok=True)
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimCmHelper.cpp'), os.path.join(ReleasePath, r'CommonLib\RealSimCmHelper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimCmHelper.h'), os.path.join(ReleasePath, r'CommonLib\RealSimCmHelper.h'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\MsgHelper.cpp'), os.path.join(ReleasePath, r'CommonLib\MsgHelper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\MsgHelper.h'), os.path.join(ReleasePath, r'CommonLib\MsgHelper.h'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\ConfigHelper.cpp'), os.path.join(ReleasePath, r'CommonLib\ConfigHelper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\ConfigHelper.h'), os.path.join(ReleasePath, r'CommonLib\ConfigHelper.h'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\SocketHelper.cpp'), os.path.join(ReleasePath, r'CommonLib\SocketHelper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\SocketHelper.h'), os.path.join(ReleasePath, r'CommonLib\SocketHelper.h'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\VehDataMsgDefs.h'), os.path.join(ReleasePath, r'CommonLib\VehDataMsgDefs.h'))

    # release simulink files
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimPack.m'), os.path.join(ReleasePath, 'RealSimPack.m'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimDepack.m'), os.path.join(ReleasePath, 'RealSimDepack.m'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimMsgClass.m'), os.path.join(ReleasePath, 'RealSimMsgClass.m'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimInterpSpeed.m'), os.path.join(ReleasePath, 'RealSimInterpSpeed.m'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimSocket.mexw64'), os.path.join(ReleasePath, 'RealSimSocket.mexw64'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\RealSimInitSimulink.m'), os.path.join(ReleasePath, 'RealSimInitSimulink.m'))

    # release CarMaker files
    os.makedirs(os.path.join(ReleasePath, 'CarMaker', 'CM11'), exist_ok=True)
    os.makedirs(os.path.join(ReleasePath, 'CarMaker', 'CM10'), exist_ok=True)
    os.makedirs(os.path.join(ReleasePath, 'CarMaker', 'CM9'), exist_ok=True)
    shutil.copy(os.path.join(SourcePath, r'CM11_proj\src\CarMaker.win64.exe'), os.path.join(ReleasePath, 'CarMaker', 'CM11', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(SourcePath, r'CM11_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(ReleasePath, 'CarMaker', 'CM11', 'libcarmaker4sl.mexw64'))
    shutil.copy(os.path.join(SourcePath, r'CM10_proj\src\CarMaker.win64.exe'), os.path.join(ReleasePath, 'CarMaker', 'CM10', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(SourcePath, r'CM10_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(ReleasePath, 'CarMaker', 'CM10', 'libcarmaker4sl.mexw64'))
    shutil.copy(os.path.join(SourcePath, r'CM9_proj\src\CarMaker.win64.exe'), os.path.join(ReleasePath, 'CarMaker', 'CM9', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(SourcePath, r'CM9_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(ReleasePath, 'CarMaker', 'CM9', 'libcarmaker4sl.mexw64'))

    # copy CarMaker utility files
    shutil.copytree(os.path.join(SourcePath, r'CarMaker'), os.path.join(ReleasePath, r'CarMaker'), dirs_exist_ok=True)

    # release Vissim files
    shutil.copy(os.path.join(SourcePath, r'VISSIMserver\x64\Release\DriverModel_RealSim.dll'), os.path.join(ReleasePath, 'DriverModel_RealSim.dll'))
    shutil.copy(os.path.join(SourcePath, r'VISSIMserver\x64\Release\DriverModel_RealSim_v2021.dll'), os.path.join(ReleasePath, 'DriverModel_RealSim_v2021.dll'))

    # # copy Yaml cpp library
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\build\include'), os.path.join(curPath, r'CommonLib\yaml-cpp\build\include'), dirs_exist_ok=True)
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\include'), os.path.join(curPath, r'CommonLib\yaml-cpp\include'), dirs_exist_ok=True)
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\build\Release'), os.path.join(curPath, r'CommonLib\yaml-cpp\build\Release'), dirs_exist_ok=True)

    # release CM-dSPACE library
    shutil.copy(os.path.join(SourcePath, r'CommonLib\libRealSimDsLib_2021b.a'), os.path.join(ReleasePath, r'CarMaker\libRealSimDsLib_2021b.a'))

    # common virtual environment files
    shutil.copy(os.path.join(SourcePath, r'CommonLib\VirEnvHelper.cpp'), os.path.join(ReleasePath, r'CommonLib\VirEnvHelper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\VirEnvHelper.h'), os.path.join(ReleasePath, r'CommonLib\VirEnvHelper.h'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\VirEnv_Wrapper.cpp'), os.path.join(ReleasePath, r'CommonLib\VirEnv_Wrapper.cpp'))
    shutil.copy(os.path.join(SourcePath, r'CommonLib\VirEnv_Wrapper.h'), os.path.join(ReleasePath, r'CommonLib\VirEnv_Wrapper.h'))

    print("==============================")
    print('RealSim: finished release')
    print("==============================")

    pass

if __name__ == '__main__':
    argparser = argparse.ArgumentParser(description=__doc__)
    argparser.add_argument('--realsim-path', '-p', 
                        metavar = 'PATH', 
                        type=str, 
                        default=os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), r'.\\')),
                        help='RealSim source code path')
    arguments = argparser.parse_args()

    ReleaseRealSim(arguments)
