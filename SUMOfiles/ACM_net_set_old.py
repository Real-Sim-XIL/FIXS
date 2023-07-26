import xml.etree.ElementTree as ET
# tree = ET.parse('SUMOfiles/ACM_coordMerge_v1.net.xml')
# #tree = ET.parse('SUMOfiles/parseMe.xml')
# root = tree.getroot()
# root = ET.fromstring(country_data_as_string)

net = './SUMOfiles/ACM_coordMerge_v1.net.xml'
savenet = './SUMOfiles/ACM_coordMerge_v2.net.xml'
setSpeed = '25'

def parser(net, savenet, laneIdList, **kwargs):

    tree = ET.parse(net)
    root = tree.getroot()



    print(laneIdList)
    changeVal = 0

    i = 0
    j= 0
    for edge in root.findall('edge'):
        i = 0
        j = 0
        for lane in edge.findall('lane'):
            print(edge[i].attrib)
            print('(*****)')

            for key in edge[i].attrib:
                if key == 'id':
                    print(edge[i].attrib[key])
                    j = 0
                    for arg in laneIdList:
                        #print('arg check is:'+ str(args[j]))
                        
                        if edge[i].attrib[key] == laneIdList[j]:
                            print('MATCH')
                            changeVal = 1
                        else:
                            pass
                            #print('No match')
                        j += 1

                else:
                    pass
            # print('Change Val is: '+ str(changeVal))
            if changeVal == 1:
                # change all the properties specified
                for argKey, argVal in kwargs.items():
                    if argKey == 'allow':
                        lane.set('allow', argVal)
                    elif argKey == 'disallow':
                        lane.set('disallow', argVal)
                    elif argKey == 'speed':
                        lane.set('speed', argVal)
                    elif argKey == 'changeLeft':
                        lane.set('changeLeft', argVal)
                    elif argKey == 'changeRight':
                        lane.set('changeRight', argVal)
                # print('Value changing')
                # print(edge[i].attrib)
                changeVal = 0
            else:
                pass
            i += 1

    tree.write(savenet)


disableLaneList = ['-1008000.0.00_2','-1008000.0.00_3',
        '-1008000.127.36_1','-1008000.127.36_2',
        '-1008000.203.53_2','-1008000.203.53_3',
        '-1008000.252.14_0', '-1008000.252.14_2','-1008000.252.14_3',
        '-1008000.284.93.0', '-1008000.284.93.2','-1008000.284.93.3'
        '-1008000.289.23_0','-1008000.289.23_2','-1008000.289.23_3',
        '-1006000.0.00_2','-1006000.0.00_3',
        '-1006000.65.28_3','-1006000.65.28_4',
        '-1006000.82.49_3','-1006000.82.49_4',
        '-1006000.231.07_3',
        '-1006000.250.21_2',
        '-1006000.289.31_2',
        '-1005000.0.00_2',
        '-1005000.71.22_2',
        '-1005000.132.14_2',
        '-1005000.167.06_1',
        '-1005000.207.66_1',
        '-1005000.214.65_1',
        '-1005000.293.08_1',
        '-1005000.310.98_1',
        '-1005000.423.02_1',
        '-1005000.434.88_1',
        '-1005000.480.85_1',
        '-1005000.570.08_1' ]
setSpeedLaneList = ['-1008000.0.00_1',
        '-1008000.127.36_0',
        '-1008000.203.53_1',
        '-1008000.252.14_1', 
        '-1008000.284.93.1',
        '-1008000.289.23_1',
        '-1006000.0.00_1',
        '-1006000.65.28_2',
        '-1006000.82.49_2',
        '-1006000.231.07_2',
        '-1006000.250.21_1',
        '-1006000.289.31_1',
        '-1005000.0.00_1', '-1005000.0.00_0',
        '-1005000.71.22_1', '-1005000.71.22_0',
        '-1005000.132.14_1',
        '-1005000.167.06_0',
        '-1005000.207.66_0',
        '-1005000.214.65_0',
        '-1005000.293.08_0',
        '-1005000.310.98_0',
        '-1005000.423.02_0',
        '-1005000.434.88_0',
        '-1005000.480.85_0',
        '-1005000.570.08_0',
        '-1010000.131.17_0',
        '-1010000.453.24_1', 
        '-1010000.460.94_1', 
        '-1010000.465.51_1', 
        '-1007000.0.00_0', 
        '-1007000.303.47_0'] 
parser(net, savenet, disableLaneList, disallow='passenger pedestrian tram rail_urban rail rail_electric rail_fast ship')
#parser(net, savenet, ['-1008000.0.00_2','-1008000.0.00_3'], disallow='passenger pedestrian tram rail_urban rail rail_electric rail_fast ship')
#parser(net, savenet, setSpeedLaneList, changeLeft='ignoring')
parser(savenet, savenet, setSpeedLaneList, speed='13.89')