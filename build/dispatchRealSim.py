'''

script to grab .exe and .dll files to RealSimRelease folder
run script within the RealSimRelease folder

'''

import os
import shutil
import argparse

def ReleaseRealSim(args):
    #RealSimSourcePath = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), r'..\..\RealSimInterface'))
    RealSimSourcePath = args.realsim_path

    # compile code
    curPath = os.path.abspath(os.curdir)
    os.chdir(os.path.join(RealSimSourcePath, r'tests'))
    status=os.system('compileCodes.bat')
    os.chdir(curPath)
    if status == -1:
        print("==============================")
        print('RealSimError: compile code failed!')
        print("==============================")
        exit()

    # release executable
    shutil.copy(os.path.join(RealSimSourcePath, r'TrafficLayer\x64\Release\TrafficLayer.exe'), 'TrafficLayer.exe')
    shutil.copy(os.path.join(RealSimSourcePath, r'ControlLayer\x64\Release\CoordMerge.exe'), 'CoordMerge.exe')

    # release source code
    # create directory first
    os.makedirs(os.path.join(curPath, 'CommonLib'), exist_ok=True)
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimCmHelper.cpp'), os.path.join(curPath, r'CommonLib\RealSimCmHelper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimCmHelper.h'), os.path.join(curPath, r'CommonLib\RealSimCmHelper.h'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\MsgHelper.cpp'), os.path.join(curPath, r'CommonLib\MsgHelper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\MsgHelper.h'), os.path.join(curPath, r'CommonLib\MsgHelper.h'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\ConfigHelper.cpp'), os.path.join(curPath, r'CommonLib\ConfigHelper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\ConfigHelper.h'), os.path.join(curPath, r'CommonLib\ConfigHelper.h'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\SocketHelper.cpp'), os.path.join(curPath, r'CommonLib\SocketHelper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\SocketHelper.h'), os.path.join(curPath, r'CommonLib\SocketHelper.h'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\VehDataMsgDefs.h'), os.path.join(curPath, r'CommonLib\VehDataMsgDefs.h'))

    # release simulink files
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimPack.m'), 'RealSimPack.m')
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimDepack.m'), 'RealSimDepack.m')
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimMsgClass.m'), 'RealSimMsgClass.m')
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimInterpSpeed.m'), 'RealSimInterpSpeed.m')
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimSocket.mexw64'), 'RealSimSocket.mexw64')
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\RealSimInitSimulink.m'), 'RealSimInitSimulink.m')

    # release CarMaker files
    os.makedirs(os.path.join(curPath, 'CarMaker', 'CM11'), exist_ok=True)
    os.makedirs(os.path.join(curPath, 'CarMaker', 'CM10'), exist_ok=True)
    os.makedirs(os.path.join(curPath, 'CarMaker', 'CM9'), exist_ok=True)
    shutil.copy(os.path.join(RealSimSourcePath, r'CM11_proj\src\CarMaker.win64.exe'), os.path.join(curPath, 'CarMaker', 'CM11', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CM11_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(curPath, 'CarMaker', 'CM11', 'libcarmaker4sl.mexw64'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CM10_proj\src\CarMaker.win64.exe'), os.path.join(curPath, 'CarMaker', 'CM10', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CM10_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(curPath, 'CarMaker', 'CM10', 'libcarmaker4sl.mexw64'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CM9_proj\src\CarMaker.win64.exe'), os.path.join(curPath, 'CarMaker', 'CM9', 'CarMaker.win64.exe'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CM9_proj\src_cm4sl\libcarmaker4sl.mexw64'), os.path.join(curPath, 'CarMaker', 'CM9', 'libcarmaker4sl.mexw64'))

    # release Vissim files
    shutil.copy(os.path.join(RealSimSourcePath, r'VISSIMserver\x64\Release\DriverModel_RealSim.dll'), 'DriverModel_RealSim.dll')
    shutil.copy(os.path.join(RealSimSourcePath, r'VISSIMserver\x64\Release\DriverModel_RealSim_v2021.dll'), 'DriverModel_RealSim_v2021.dll')

    # # copy Yaml cpp library
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\build\include'), os.path.join(curPath, r'CommonLib\yaml-cpp\build\include'), dirs_exist_ok=True)
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\include'), os.path.join(curPath, r'CommonLib\yaml-cpp\include'), dirs_exist_ok=True)
    # shutil.copytree(os.path.join(RealSimSourcePath, r'CommonLib\yaml-cpp\build\Release'), os.path.join(curPath, r'CommonLib\yaml-cpp\build\Release'), dirs_exist_ok=True)

    # common virtual environment files
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\VirEnvHelper.cpp'), os.path.join(curPath, r'CommonLib\VirEnvHelper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\VirEnvHelper.h'), os.path.join(curPath, r'CommonLib\VirEnvHelper.h'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\VirEnv_Wrapper.cpp'), os.path.join(curPath, r'CommonLib\VirEnv_Wrapper.cpp'))
    shutil.copy(os.path.join(RealSimSourcePath, r'CommonLib\VirEnv_Wrapper.h'), os.path.join(curPath, r'CommonLib\VirEnv_Wrapper.h'))


    print("==============================")
    print('RealSim: finished release')
    print("==============================")

    pass

if __name__ == '__main__':
    argparser = argparse.ArgumentParser(description=__doc__)
    argparser.add_argument('--realsim-path', '-p', 
                        metavar = 'PATH', 
                        type=str, 
                        default=os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), r'..\..\RealSimInterface')),
                        help='RealSim source code path')
    arguments = argparser.parse_args()

    ReleaseRealSim(arguments)
