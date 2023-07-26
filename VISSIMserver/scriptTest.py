from ctypes import*

driverD11 = cdll.LoadLibrary("D:\\Dropbox (ORNL)\\2_projects\\1_1_9_1b\\2_2_RealSim\\VISSIMserver\\x64\\Debug\\DriverModel_RealSim.dll")

def SendComDataToDll():
	#logFile = open('D:\\WupingXin\\MyVissimLog.txt1a') 
    SC_number = 1 # SC = SignalController
    SG_number = 2 # SG = SignalGroup
    SignalController = Vissim.Net.SignalControllers.ItemByKey(SC_number)
    SignalGroup = SignalController.SGs.ItemByKey(SG_number)
    SigStateStr = SignalGroup.AttValue("SigState")
    if SigStateStr == "GREEN":
        SigState = 1
    elif SigStateStr == "AMBER":
        SigState = 8
    elif SigStateStr == "RED":
        SigState = 2
    else:
        SigState = 0
	driverD11.test2(0, SC_number, SG_number, 0, SigState, 0)
    
	# logFile.write('\n'+ repr(result))
	# logFile.close()
	
	return