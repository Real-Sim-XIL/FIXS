import os
import fileinput

def addCode(fullFilePath, searchStr, addStr, addMode='prefix'):
    newCppStr = ''
    with open(fullFilePath, 'r') as cppFile:

        iLine = 0
        iSearchStrLine = 0

        # Read all lines in the file one by one
        for line in cppFile:
            # For each line, check if line contains the string
            iLine += 1
            if searchStr in line:
                # If yes, then return
                iSearchStrLine = iLine
                # add new Str
                if addMode == 'prefix':
                    newCppStr = newCppStr + line.replace(line, addStr+'\n'+line)
                elif addMode == 'ConfigHelper':
                    newCppStr = newCppStr + line.replace("};", addStr+"};")
            else:
                newCppStr = newCppStr + line
        
        if iSearchStrLine == 0:
            print('reach end of file, did not find the pattern!')
            exit()

    # update the file
    with open(fullFilePath, 'w') as cppFile:
        cppFile.write(newCppStr)


### begin adding code automatically

RealSimPath = os.path.abspath(os.getcwd())

vehicleMessageName = 'lightIndicators'
vehicleMessageDataType = 'uint16_t'

#############################################################################
######################    ADD to CPP
#
# will change MsgHelper.cpp and ConfigHelper.cpp
#
#############################################################################

fullFilePath = os.path.join(RealSimPath, 'CommonLib\MsgHelper.cpp')

# 1)
searchStr = '// printVehData: add new vehicle message field here'
addStr = '\tif (VehicleMessageField_set.find(\"{0}\") != VehicleMessageField_set.end()) {{\n\t\tprintf(\"\\t {0}: %d\\n\", VehData.{0});\n\t}}\n'.format(vehicleMessageName,vehicleMessageDataType)

addCode(fullFilePath, searchStr, addStr)

# # 2)
# searchStr = '// VehFullDataToSubData: add new vehicle message field here'
# addStr = '\tif (VehicleMessageField_set.find(\"{0}\") != VehicleMessageField_set.end()) {{\n\t\tVehData[\"{0}\"] = VehFullData.{0};\n\t}}\n'.format(vehicleMessageName)

# addCode(fullFilePath, searchStr, addStr)

# 3)
searchStr = '// packVehData: add new vehicle message field here'
addStr = '\tnumericVehDataToBuffer<{1}>(VehData.{0}, \"{0}\", buffer, iByte);\n'.format(vehicleMessageName, vehicleMessageDataType)

addCode(fullFilePath, searchStr, addStr)

# 4)
searchStr = '// depackVehData: add new vehicle message field here'
if (vehicleMessageDataType == 'float'):
    pat = 'tempFloat'
if (vehicleMessageDataType == 'uint16_t'):
    pat = 'tempUint16'
if (vehicleMessageDataType == 'int8_t'):
    pat = 'tempInt8'
if (vehicleMessageDataType == 'int32_t'):
    pat = 'tempInt32'
addStr = '\tbufferToNumericVehData<{1}>(buffer, &iByte, "{0}", {2}); VehData.{0} = {2};\n'.format(vehicleMessageName, vehicleMessageDataType, pat)

addCode(fullFilePath, searchStr, addStr)


##
# 5) 
fullFilePath = os.path.join(RealSimPath, 'CommonLib\ConfigHelper.cpp')

searchStr = 'SimulationSetup.VehicleMessageField ='
addStr = ', \"{0}\"'. format(vehicleMessageName)

addCode(fullFilePath, searchStr, addStr, addMode='ConfigHelper')


# add to matlab
# TO BE DONE
