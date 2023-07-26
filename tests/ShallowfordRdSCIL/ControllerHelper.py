from pysnmp.hlapi import *

class Controller:
    # set controller IP, description
    def __init__(self, IPaddr="0.0.0.0", port='501', desc = "", sigID=None, intID="00"):
        self.IPaddr = IPaddr
        self.NTCIPport = port
        self.Descriotion = desc
        self.SignalId = sigID
        self.IntersectionId = intID
    
    # Register connection to controller through SNMP
    def Register(self):
        self.Connect = SNMPManager(ip = self.IPaddr, port = self.NTCIPport)

    # Deregister and release system control
    def Deregister(self):
        self.RunPattern(0)
        self.Connect = None

    # Invoke a pattern
    def RunPattern(self, pattern):
        self.Connect.set({ "1.3.6.1.4.1.1206.4.2.1.4.14.0": Integer(pattern) })

    # Send new vehicle calls
    def NewGroupVehCalls(self, group, binaryVal):
        self.Connect.set({ "1.3.6.1.4.1.1206.4.2.1.1.5.1.6.{}".format(group): Integer(int(binaryVal, 2)) })


class SNMPManager:
    """
    This is a SNMP manager. Its being used to get and set SNMP messages to the controller. We will be having an SNMP
    instance for every controller. Default values for timeout and max reties are the default values in the PySNMP
    itself.
    """
    def __init__(self, ip, port, timeout=1, max_retries=0):
        self.__ip = ip
        self.__port = port
        self.__timeout = timeout
        self.__max_retries = max_retries
        self.__engine = SnmpEngine()
        self.__context_data = ContextData()
        self.__credentials = CommunityData('public', mpModel=0)

    def get(self, oids: list):
        try:
            handler = getCmd(
                self.__engine,
                self.__credentials,
                UdpTransportTarget((self.__ip, self.__port), timeout=self.__timeout, retries=self.__max_retries),
                self.__context_data,
                *self.__construct_object_types(oids)
            )
        except Exception as e:
            print("Error in getting SNMP response for oids {}".format(oids), exc_info=True)
            return None
        result = self.__fetch(handler, 1)
        return result

    def set(self, value_pairs: dict):
        try:
            handler = setCmd(
                self.__engine,
                self.__credentials,
                UdpTransportTarget((self.__ip, self.__port), timeout=self.__timeout, retries=self.__max_retries),
                self.__context_data,
                *self.__construct_value_pairs(value_pairs)
            )
        except Exception as e:
            print("Error in setting SNMP response for oids {}".format(value_pairs.keys()), exc_info=True)
            return None
        result = self.__fetch(handler, 1)
        # print("Data dict that is set to the controller from SNMP SET: {}".format(result))
        return result

    @staticmethod
    def __construct_object_types(list_of_oids):
        object_types = []
        for oid in list_of_oids:
            object_types.append(ObjectType(ObjectIdentity(oid)))
        return object_types

    @staticmethod
    def __construct_value_pairs(list_of_pairs):
        pairs = []
        for key, value in list_of_pairs.items():
            pairs.append(ObjectType(ObjectIdentity(key), value))
        return pairs

    @staticmethod
    def __cast(value):
        try:
            return int(value)
        except (ValueError, TypeError):
            try:
                return float(value)
            except (ValueError, TypeError):
                try:
                    return str(value)
                except (ValueError, TypeError):
                    pass
        return value

    def __fetch(self, handler, count):
        result = []
        for i in range(count):
            try:
                error_indication, error_status, error_index, var_binds = next(handler)
                if not error_indication and not error_status:
                    items = {}
                    for var_bind in var_binds:
                        items[str(var_bind[0])] = self.__cast(var_bind[1])
                    result.append(items)
                else:
                    print('Got SNMP error: {0}, SNMP error status: {1}'.format(error_indication, error_status))
                    return None
            except StopIteration:
                break
        return result[0]


# if __name__ == '__main__':
#      # Test scripts.

#     # DEFINITIONS AND INITIALIZATIONS
#     from definitions import *
#     m60 = SNMPManager('169.254.97.101', '161')
#     eos = SNMPManager('169.254.97.102', '501')

#     import schedule, time

#     ## Set System Pattern to Pattern 2
#     # print(m60.set({SystemPatternControl.number: Integer(2)}))

#     pattern = 2

#     split = [34, 38, 0, 28, 0, 72, 0, 0]
#     for i in range(0, 8):
#         m60.set({
#             SplitTime.number+'.{}.{}'.format(pattern, i+1): Integer(split[i])
#         })

