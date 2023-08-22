#******************************************************************************
#**  CarMaker - Version $CarMakerVersion$
#**  Vehicle Dynamics Simulation Toolkit
#**
#**  Copyright (C)   IPG Automotive GmbH
#**                  Bannwaldallee 60             Phone  +49.721.98520.0
#**                  76185 Karlsruhe              Fax    +49.721.98520.99
#**                  Germany                      WWW    www.ipg-automotive.com
#******************************************************************************
#**
#**  Build Configuration Script for ConfigurationDesk
#**
#******************************************************************************

from win32com.client import Dispatch, DispatchWithEvents

import collections
import os
import re
import sys
import subprocess
import string
import time
import wx

from collections import OrderedDict

SOURCETYPE_SIMULINK_MODEL = 3




def ShowMsg(text):
    from win32con import MB_TOPMOST, MB_OK
    from win32ui import MessageBox

    MessageBox(text, CM_BUILD_CFG_NAME, MB_TOPMOST | MB_OK)


def CStr(obj):
    if obj is None:
        return ""
    else:
        return str(obj)

def SplitString(strValue, semicolonSeparated = False, stripQuotes = False):
    res = []
    if len(strValue.strip()) > 0:
        if semicolonSeparated:
            res = strValue.split(";")
        else:
            res = strValue.split()
        for i in range(len(res)):
            res[i] = res[i].strip()
            if stripQuotes:
                res[i] = res[i].strip("\"'")
    return res


def JoinStrings(strVec, separator = ", "):
    if strVec is None:
        return ""
    return separator.join(strVec)


def JoinPaths(strVec, separator = ", "):
    paths = JoinStrings(strVec, separator)
    return paths.replace("/", "\\")


def NormPath(path):
    return path.replace("/", "\\")


# Change of OS on dsrt from QNX to Linux since dSPACE 2020-B
def GetCM_Arch(cmdir):
    # Uncomment the following line if QNX compiler is used with versions newer than "2020-A"
    #return "dsrt"
    try:
        cmpu  = NormPath("%s/bin/cmplugutil.exe" %(cmdir))
        dsver = subprocess.check_output([cmpu, "getDSVersion"])
        vsstr = dsver.decode('ascii', 'ignore').partition(' ')[0]
    except:
        vsstr = ""
    if vsstr > "2020-A_X64":
        return "dsrtlx"
    return "dsrt"


CM_BUILD_CFG_NAME = "CarMaker Build Configuration"
CARMAKER_DIR    = "C:/IPG/carmaker/win64-11.0.1"
CARMAKER_VER    = "11.0.1"
CARMAKER_NUMVER = 110001

ARCH  = GetCM_Arch(CARMAKER_DIR)
ARCHF = "win32"

CARMAKER_BIN_DIR = "%s/bin"     %(CARMAKER_DIR)
CARMAKER_LIB_DIR = "%s/lib-%s"  %(CARMAKER_DIR, ARCH)
CARMAKER_INC_DIR = "%s/include" %(CARMAKER_DIR)
CARMAKER_GUI_DIR = "%s/GUI"     %(CARMAKER_DIR)

CARMAKER_LIB = "%s/libcarmaker4sl.a" %(CARMAKER_LIB_DIR)
CAR_LIB      = "%s/libcar4sl.a"      %(CARMAKER_LIB_DIR)
MCYCLE_LIB   = "%s/libmcycle4sl.a"   %(CARMAKER_LIB_DIR)
TRUCK_LIB    = "%s/libtruck4sl.a"    %(CARMAKER_LIB_DIR)

DRIVER_LIB     = "%s/libipgdriver.a"    %(CARMAKER_LIB_DIR)
ROAD_LIB       = "%s/libipgroad.a"      %(CARMAKER_LIB_DIR)
TIRE_LIB       = "%s/libipgtire.a"      %(CARMAKER_LIB_DIR)
APO_LIB        = "%s/libapo.a"          %(CARMAKER_LIB_DIR)
INFO_LIB       = "%s/libinfofile.a"     %(CARMAKER_LIB_DIR)
Z_LIB          = "%s/libz-%s.a"         %(CARMAKER_LIB_DIR, ARCH)
URI_LIB        = "%s/liburiparser-%s.a" %(CARMAKER_LIB_DIR, ARCH)

if ARCH == 'dsrtlx':
    CFLAGS  = ("-include", "ipgrt.h")
    DEFINES = ("RS_CAVE", "RS_DEBUG", "RS_DSPACE", "DSPACE", "DSRTLX", "_DSRTLX", "CM_HIL",
            "CM_NUMVER=%d" %(CARMAKER_NUMVER), "CM4SLDS")
else:
    CFLAGS  = ("-include", "ipgrt.h")
    DEFINES = ("RS_CAVE", "RS_DEBUG", "RS_DSPACE", "DSPACE", "DSRT", "_DSRT", "USE_IPGRT_FUNCS", "CM_HIL",
            "CM_NUMVER=%d" %(CARMAKER_NUMVER), "CM4SLDS")

SRC_DIRS  = ("src_cm4sl", "include")
SRC_FILES = ("CM_Main.c", "CM_Vehicle.c", "User.c", "IO.c", "app_tmp.c")

PROJECT_DIR = ""

AppEvH = None

PROP_COMPILER_DEFINES                  = "CompilerDefines"
PROP_COMPILER_UNDEFINES                = "CompilerUndefines"
PROP_C_COMPILER_OPTIONS                = "CCompilerOptions"
PROP_CPP_COMPILER_OPTIONS              = "CppCompilerOptions"
PROP_COMPILER_OPTIMIZATION_SET         = "CompilerOptimizationSet"
PROP_USER_DEFINED_OPTIMIZATION_OPTIONS = "UserDefinedOptimizationOptions"
PROP_SEARCH_PATHS                      = "SearchPaths"
PROP_CUSTOM_SOURCE_FILES               = "CustomSourceFiles"
PROP_CUSTOM_LIBRARIES                  = "CustomLibraries"

RelevantPropertyNames = (PROP_COMPILER_DEFINES,
                         PROP_COMPILER_UNDEFINES,
                         PROP_C_COMPILER_OPTIONS,
                         PROP_CPP_COMPILER_OPTIONS,
                         PROP_COMPILER_OPTIMIZATION_SET,
                         PROP_USER_DEFINED_OPTIMIZATION_OPTIONS,
                         PROP_SEARCH_PATHS,
                         PROP_CUSTOM_SOURCE_FILES,
                         PROP_CUSTOM_LIBRARIES)


# =============================================================================
# DDE Client
# =============================================================================
from ctypes import POINTER, WINFUNCTYPE, c_char_p, c_void_p, c_int, c_ulong
from ctypes.wintypes import BOOL, DWORD, BYTE, INT, LPCWSTR, UINT, ULONG

# DECLARE_HANDLE(name) typedef void *name;
HCONV     = c_void_p  # = DECLARE_HANDLE(HCONV)
HDDEDATA  = c_void_p  # = DECLARE_HANDLE(HDDEDATA)
HSZ       = c_void_p  # = DECLARE_HANDLE(HSZ)
LPBYTE    = c_char_p  # POINTER(BYTE)
LPDWORD   = POINTER(DWORD)
LPSTR     = c_char_p
ULONG_PTR = c_ulong

PM_NOREMOVE = 0x0000
PM_REMOVE   = 0x0001
PM_NOYIELD  = 0x0002

#PM_QS_INPUT       = (QS_INPUT << 16)
#PM_QS_PAINT       = (QS_PAINT << 16)
#PM_QS_POSTMESSAGE = ((QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER) << 16)
#PM_QS_SENDMESSAGE = (QS_SENDMESSAGE << 16)


# See windows/ddeml.h for declaration of struct CONVCONTEXT
PCONVCONTEXT = c_void_p

DMLERR_NO_ERROR = 0

# Predefined Clipboard Formats
CF_TEXT         =  1
CF_BITMAP       =  2
CF_METAFILEPICT =  3
CF_SYLK         =  4
CF_DIF          =  5
CF_TIFF         =  6
CF_OEMTEXT      =  7
CF_DIB          =  8
CF_PALETTE      =  9
CF_PENDATA      = 10
CF_RIFF         = 11
CF_WAVE         = 12
CF_UNICODETEXT  = 13
CF_ENHMETAFILE  = 14
CF_HDROP        = 15
CF_LOCALE       = 16
CF_DIBV5        = 17
CF_MAX          = 18

DDE_FACK          = 0x8000
DDE_FBUSY         = 0x4000
DDE_FDEFERUPD     = 0x4000
DDE_FACKREQ       = 0x8000
DDE_FRELEASE      = 0x2000
DDE_FREQUESTED    = 0x1000
DDE_FAPPSTATUS    = 0x00FF
DDE_FNOTPROCESSED = 0x0000

DDE_FACKRESERVED  = (~(DDE_FACK | DDE_FBUSY | DDE_FAPPSTATUS))
DDE_FADVRESERVED  = (~(DDE_FACKREQ | DDE_FDEFERUPD))
DDE_FDATRESERVED  = (~(DDE_FACKREQ | DDE_FRELEASE | DDE_FREQUESTED))
DDE_FPOKRESERVED  = (~(DDE_FRELEASE))

XTYPF_NOBLOCK        = 0x0002
XTYPF_NODATA         = 0x0004
XTYPF_ACKREQ         = 0x0008

XCLASS_MASK          = 0xFC00
XCLASS_BOOL          = 0x1000
XCLASS_DATA          = 0x2000
XCLASS_FLAGS         = 0x4000
XCLASS_NOTIFICATION  = 0x8000

XTYP_ERROR           = (0x0000 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)
XTYP_ADVDATA         = (0x0010 | XCLASS_FLAGS)
XTYP_ADVREQ          = (0x0020 | XCLASS_DATA | XTYPF_NOBLOCK)
XTYP_ADVSTART        = (0x0030 | XCLASS_BOOL)
XTYP_ADVSTOP         = (0x0040 | XCLASS_NOTIFICATION)
XTYP_EXECUTE         = (0x0050 | XCLASS_FLAGS)
XTYP_CONNECT         = (0x0060 | XCLASS_BOOL | XTYPF_NOBLOCK)
XTYP_CONNECT_CONFIRM = (0x0070 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)
XTYP_XACT_COMPLETE   = (0x0080 | XCLASS_NOTIFICATION )
XTYP_POKE            = (0x0090 | XCLASS_FLAGS)
XTYP_REGISTER        = (0x00A0 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK )
XTYP_REQUEST         = (0x00B0 | XCLASS_DATA )
XTYP_DISCONNECT      = (0x00C0 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK )
XTYP_UNREGISTER      = (0x00D0 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK )
XTYP_WILDCONNECT     = (0x00E0 | XCLASS_DATA | XTYPF_NOBLOCK)
XTYP_MONITOR         = (0x00F0 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)

XTYP_MASK            = 0x00F0
XTYP_SHIFT           = 4

TIMEOUT_ASYNC        = 0xFFFFFFFF

def get_winfunc(libname, funcname, restype=None, argtypes=(), _libcache={}):
    """Retrieve a function from a library, and set the data types."""
    from ctypes import windll

    if libname not in _libcache:
        _libcache[libname] = windll.LoadLibrary(libname)
    func = getattr(_libcache[libname], funcname)
    func.argtypes = argtypes
    func.restype = restype

    return func


DDECALLBACK = WINFUNCTYPE(HDDEDATA, UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA,
                          ULONG_PTR, ULONG_PTR)

# =============================================================================
# Project EventHandling
# =============================================================================
from win32com import client

CONTROLLER = None
ProjEvH_act = 0


# =============================================================================
# Class: DDE
# =============================================================================
class DDE:
    """Object containing all the DDE functions"""
    AccessData         = get_winfunc("user32", "DdeAccessData",          LPBYTE,   (HDDEDATA, LPDWORD))
    ClientTransaction  = get_winfunc("user32", "DdeClientTransaction",   HDDEDATA, (LPBYTE, DWORD, HCONV, HSZ, UINT, UINT, DWORD, LPDWORD))
    Connect            = get_winfunc("user32", "DdeConnect",             HCONV,    (DWORD, HSZ, HSZ, PCONVCONTEXT))
    CreateStringHandle = get_winfunc("user32", "DdeCreateStringHandleW", HSZ,      (DWORD, LPCWSTR, UINT))
    Disconnect         = get_winfunc("user32", "DdeDisconnect",          BOOL,     (HCONV,))
    GetLastError       = get_winfunc("user32", "DdeGetLastError",        UINT,     (DWORD,))
    Initialize         = get_winfunc("user32", "DdeInitializeW",         UINT,     (LPDWORD, DDECALLBACK, DWORD, DWORD))
    FreeDataHandle     = get_winfunc("user32", "DdeFreeDataHandle",      BOOL,     (HDDEDATA,))
    FreeStringHandle   = get_winfunc("user32", "DdeFreeStringHandle",    BOOL,     (DWORD, HSZ))
    QueryString        = get_winfunc("user32", "DdeQueryStringA",        DWORD,    (DWORD, HSZ, LPSTR, DWORD, c_int))
    UnaccessData       = get_winfunc("user32", "DdeUnaccessData",        BOOL,     (HDDEDATA,))
    Uninitialize       = get_winfunc("user32", "DdeUninitialize",        BOOL,     (DWORD,))

# =============================================================================
# Class: DDEError
# =============================================================================
class DDEError:
    """Exception raise when a DDE errpr occures."""
    def __init__(self, msg, idInst=None):
        if idInst is None:
            RuntimeError.__init__(self, msg)
        else:
            RuntimeError.__init__(self, "%s (err=%s)" % (msg, hex(DDE.GetLastError(idInst))))

# =============================================================================
# Class: DDEClient
# =============================================================================
class DDEClient:
    """The DDEClient class.

    Use this class to create and manage a connection to a service/topic.
    To get classbacks subclass DDEClient and overwrite callback."""

    def __init__(self, service, topic):
        """Create a connection to a service/topic."""
        from ctypes import byref

        self._idInst = DWORD(0)
        self._hConv = HCONV()

        self._callback = DDECALLBACK(self._callback)
        res = DDE.Initialize(byref(self._idInst), self._callback, 0x00000010, 0)
        if res != DMLERR_NO_ERROR:
            raise DDEError("Unable to register with DDEML (err=%s)" % hex(res))

        hszService = DDE.CreateStringHandle(self._idInst, service, 1200)
        hszTopic = DDE.CreateStringHandle(self._idInst, topic, 1200)
        self._hConv = DDE.Connect(self._idInst, hszService, hszTopic, PCONVCONTEXT())
        DDE.FreeStringHandle(self._idInst, hszTopic)
        DDE.FreeStringHandle(self._idInst, hszService)
        if not self._hConv:
            raise DDEError("Unable to establish a conversation with server", self._idInst)

    def __del__(self):
        """Cleanup any active connections."""
        if self._hConv:
            DDE.Disconnect(self._hConv)
        if self._idInst:
            DDE.Uninitialize(self._idInst)

    def advise(self, item, stop=False):
        """Request updates when DDE data changes."""
        from ctypes import byref

        hszItem = DDE.CreateStringHandle(self._idInst, item, 1200)
        hDdeData = DDE.ClientTransaction(LPBYTE(), 0, self._hConv, hszItem, CF_TEXT, XTYP_ADVSTOP if stop else XTYP_ADVSTART, TIMEOUT_ASYNC, LPDWORD())
        DDE.FreeStringHandle(self._idInst, hszItem)
        if not hDdeData:
            raise DDEError("Unable to %s advise" % ("stop" if stop else "start"), self._idInst)
        DDE.FreeDataHandle(hDdeData)

    def execute(self, command, timeout=5000):
        """Execute a DDE command."""
        from ctypes import byref

        if sys.version < '3':
            pData = c_char_p(command).value
        else:
            pData = command.encode('utf-8')
        cbData = DWORD(len(pData) + 1)
        hDdeData = DDE.ClientTransaction(pData, cbData, self._hConv, HSZ(), CF_TEXT, XTYP_EXECUTE, timeout, LPDWORD())
        if not hDdeData:
            raise DDEError("Unable to send command", self._idInst)
        DDE.FreeDataHandle(hDdeData)

    def request(self, item, timeout=5000):
        """Request data from DDE service."""
        from ctypes import byref

        hszItem = DDE.CreateStringHandle(self._idInst, item, 1200)
        hDdeData = DDE.ClientTransaction(LPBYTE(), 0, self._hConv, hszItem, CF_TEXT, XTYP_REQUEST, timeout, LPDWORD())
        DDE.FreeStringHandle(self._idInst, hszItem)
        if not hDdeData:
            raise DDEError("Unable to request item", self._idInst)

        if timeout != TIMEOUT_ASYNC:
            pdwSize = DWORD(0)
            pData = DDE.AccessData(hDdeData, byref(pdwSize))
            if not pData:
                DDE.FreeDataHandle(hDdeData)
                raise DDEError("Unable to access data", self._idInst)
            # TODO: use pdwSize
            DDE.UnaccessData(hDdeData)
        else:
            pData = None
            DDE.FreeDataHandle(hDdeData)
        return pData

    def callback(self, value, item=None):
        """Callback function for advice."""
        print(("%s: %s" % (item, value)))

    def _callback(self, wType, uFmt, hConv, hsz1, hsz2, hDdeData, dwData1, dwData2):
        if wType == XTYP_ADVDATA:
            from ctypes import byref, create_string_buffer

            dwSize = DWORD(0)
            pData = DDE.AccessData(hDdeData, byref(dwSize))
            if pData:
                item = create_string_buffer('\000' * 128)
                DDE.QueryString(self._idInst, hsz2, item, 128, 1004)
                self.callback(pData, item.value)
                DDE.UnaccessData(hDdeData)
            return DDE_FACK
        return 0


# =============================================================================
# Class: CM_BuildEvents
# =============================================================================
class CM_BuildEvents:
    def OnBuildFinished(self, x):
        EArgs = Dispatch(x)
        BRes = EArgs.Item("BuildResult")
        if BRes is None:
            return
        if BRes.Success and BRes.Downloaded:
            ConnectCM_GUI()

    def OnBuildStarted(self, x):
        EArgs = Dispatch(x)
        StartCM_GUI()

    def OnBuildStarting(self, x):
        pass

    def OnQuitting(self):
        pass

    def OnStarted(self):
        pass


# =============================================================================
# Class: BuildConfigSetInfo
# =============================================================================
class BuildConfigSetInfo:
    def __init__(self, bcs=None, name=""):
        self.AppProcCount = 0
        self.SL_AppProcCount = 0
        self.CM_AppProcCount = 0
        self.CreateOnCfgDesk = False
        self.DeleteOnCfgDesk = False
        self.NameOnCfgDesk = ""
        self.Properties = {}
        if bcs is None:
            self.Name = name
        else:
            self.Name = bcs.Name
            self.NameOnCfgDesk = bcs.Name
            for p in bcs.Properties:
                if CStr(p.Value).count("COMObject") > 0:
                    elements = []
                    for e in p.Value:
                        elements.append(e)
                    self.Properties[p.Name] = ";".join(elements)
                else:
                    self.Properties[p.Name] = CStr(p.Value)

    def GetCompDefines(self):
        return CStr(self.Properties.get(PROP_COMPILER_DEFINES))

    def SetCompDefines(self, strValue):
        self.Properties[PROP_COMPILER_DEFINES] = strValue

    def GetCompUnDefines(self):
        return CStr(self.Properties.get(PROP_COMPILER_UNDEFINES))

    def SetCompUnDefines(self, strValue):
        self.Properties[PROP_COMPILER_UNDEFINES] = strValue

    def GetSearchPaths(self):
        return CStr(self.Properties.get(PROP_SEARCH_PATHS))

    def SetSearchPaths(self, strValue):
        self.Properties[PROP_SEARCH_PATHS] = strValue

    def GetCustomLibraries(self):
        return CStr(self.Properties.get(PROP_CUSTOM_LIBRARIES))

    def SetCustomLibraries(self, strValue):
        self.Properties[PROP_CUSTOM_LIBRARIES] = strValue

    def GetCustomSrcFiles(self):
        return CStr(self.Properties.get(PROP_CUSTOM_SOURCE_FILES))

    def SetCustomSrcFiles(self, strValue):
        self.Properties[PROP_CUSTOM_SOURCE_FILES] = strValue

    def GetCCompOpts(self):
        return CStr(self.Properties.get(PROP_C_COMPILER_OPTIONS))

    def SetCCompOpts(self, strValue):
        self.Properties[PROP_C_COMPILER_OPTIONS] = strValue

    def GetCPPCompOpts(self):
        return CStr(self.Properties.get(PROP_CPP_COMPILER_OPTIONS))

    def SetCPPCompOpts(self, strValue):
        self.Properties[PROP_CPP_COMPILER_OPTIONS] = strValue

    def IsEqual(self, bcsi):
        if self.Name != bcsi.Name:
            return False
        return self.PropertiesAreEqual(bcsi)

    def PropertiesAreEqual(self, bcsi):
        if len(self.Properties) != len(bcsi.Properties):
            return False
        for item in list(self.Properties.items()):
            if item[0] == "Name":
                continue
            prop = bcsi.Properties.get(item[0])
            if CStr(item[1]) != CStr(prop):
                return False
        return True

    def CloneProperties(self, obcsi):
        self.Properties.clear()
        for propItem in list(obcsi.Properties.items()):
            self.Properties[propItem[0]] = propItem[1]


# =============================================================================
# Class: ApplProcInfo
# =============================================================================
class ApplProcInfo:
    def __init__(self, name, bcsInfo):
        self.Name      = name
        self.BcsName   = bcsInfo.Name
        self.ModelName = ""
        self.Model     = None
        self.ModelPath = None


class CM_BuildConfig:
    def __init__(self):
        # Initialize the ConfigurationDesk application object.
        self.CfgDesk = None
        # Initialize the application to perform a build process with.
        self.ActApp = None

        # Initialize Macro and Search Path template
        self.CM_COMPILER_DEFINES   = ""
        self.CM_COMPILER_UNDEFINES = ""
        self.CM_COMPILER_OPTIONS   = ""
        self.CM_COMPILER_OPT_SET   = ""
        self.CM_INCLUDES           = ""
        self.CM_USER_OPT_OPTIONS   = ""
        self.CM_SOURCE_FILES       = ""
        self.CM_LIBRARIES          = ""
        self.CM_SEARCH_PATHS       = ""

        # Initialize Collections for Build Configuration Informations
        self.BldCfgSetInfos = collections.OrderedDict()
        self.ApplProcInfos  = collections.OrderedDict()


    def Initialize(self):
        try:
            # Get the ConfigurationDesk Automation interface
            self.CfgDesk = Dispatch("ConfigurationDesk.Application")
            self.CfgDesk.MainWindow.Visible = True
            if self.CfgDesk.ActiveApplication is None:
                return "NoApp"
            self.ActApp = self.CfgDesk.ActiveApplication
            self.CM_COMPILER_DEFINES = JoinStrings(DEFINES, " ")
            self.CM_COMPILER_OPTIONS = JoinStrings(CFLAGS, " ")
            srch = [ CARMAKER_INC_DIR ]
            self.CM_SEARCH_PATHS = JoinPaths(srch, "; ")
            libs = [ "libdscandrv.so", "libRealSimDsLib_2021b.a"  ]
            self.CM_LIBRARIES = JoinPaths(libs, "; ")
            # srcf = [ "MySource_File.c" ]
            # self.CM_SOURCE_FILES = JoinPaths(srcf, "; ")
            return True
        except Exception as exc:
            ShowMsg("Error initializing: " + str(exc))
            return False


    def ReadBuildConfigs(self, actApp=None):
        # Read Build Configuration Sets
        if actApp is None:
            actApp = self.ActApp
        self.BldCfgSetInfos.clear()
        self.ApplProcInfos.clear()
        # get access to all BuildConfiguration relations of active app
        relBC = actApp.Relations.Item("BuildConfiguration")
        # get 1st executable application in active BuildConfiguration relations
        rootApp = relBC.GetTopNodes().Item(0)
        # collect ApplicationProcessOptions:
        # -> scan all relations of rootApp (executable application)
        for bcs in relBC.GetElements(rootApp):
            if not bcs.IsOfRole("ApplicationProcessOptions"):
                continue
            # read all Build Configuration Set informations
            bcsi = BuildConfigSetInfo(bcs)
            if bcsi.Name in self.BldCfgSetInfos:
                self.BldCfgSetInfos.clear()
                self.ApplProcInfos.clear()
                return False
            self.BldCfgSetInfos[bcsi.Name] = bcsi
            # scan all relations of this Build Configuration Set
            for proc in relBC.GetElements(bcs):
                bcsi.AppProcCount += 1
                if relBC.GetElements(proc).Count <= 0:
                    continue
                model = relBC.GetElements(proc).Item(0)
                if model.Properties.Item("Source type").Value != SOURCETYPE_SIMULINK_MODEL:
                    continue
                appi = ApplProcInfo(proc.Name, bcs)
                appi.ModelName = model.Name
                appi.Model     = model
                for comp in actApp.Components.Item("ModelTopology"):
                    if comp.Name != appi.ModelName:
                        continue
                    prop = comp.Properties.TryGetItem("Model location")
                    if prop is None:
                        continue
                    appi.ModelPath = prop.Value
                if appi.Name in self.ApplProcInfos:
                    self.BldCfgSetInfos.clear()
                    self.ApplProcInfos.clear()
                    return False
                self.ApplProcInfos[appi.Name] = appi
                bcsi.SL_AppProcCount += 1
                if bcsi.Name == CM_BUILD_CFG_NAME:
                    bcsi.CM_AppProcCount += 1
        return True


    def CM_BuildConfigInfo(self, actApp=None):
        if actApp is None:
            actApp = self.ActApp
        if CM_BUILD_CFG_NAME in self.BldCfgSetInfos:
            return self.BldCfgSetInfos.get(CM_BUILD_CFG_NAME)
        return None


    def ApplyBuildConfig(self, bcsi=None):
        global PROJECT_DIR
        # get access to all BuildConfiguration relations of active app
        relBC = self.ActApp.Relations.Item("BuildConfiguration")
        # get 1st executable application in active BuildConfiguration relations
        rootApp = relBC.GetTopNodes().Item(0)
        bcsType = relBC.GetCreatableTypes(rootApp).Item("ApplicationProcessOptions")
        dictBCS = OrderedDict()
        dictAP  = OrderedDict()
        # Read the current configuration
        for bcs in relBC.GetElements(rootApp):
            if not bcs.IsOfRole("ApplicationProcessOptions"):
                continue
            dictBCS[bcs.Name] = bcs
            for ap in relBC.GetElements(bcs):
                dictAP[ap.Name] = ap

        if not bcsi is None:
            bcsi_set = collections.OrderedDict()
            bcsi_set[bcsi.Name] = bcsi
        else:
            bcsi_set = self.BldCfgSetInfos
        for bcsi in list(bcsi_set.values()):
            if bcsi.DeleteOnCfgDesk:
                # Delete the Build Configuration Info
                if bcsi.NameOnCfgDesk in dictBCS:
                    relBC.RemoveElements(rootApp, [dictBCS[bcsi.NameOnCfgDesk]])
                continue
            # Get the Application Processes to assign to the BCS
            appisToAssign = self.GetApplProcInfosForBcs(bcsi)
            # Get an existing BCS from CfgDesk or create a new one
            curBCS = None
            if bcsi.CreateOnCfgDesk:
                curBCS = relBC.CreateDataObject(bcsType, rootApp)
                curBCS.Name = bcsi.NameOnCfgDesk
            else:
                if bcsi.NameOnCfgDesk in dictBCS:
                    curBCS = dictBCS[bcsi.NameOnCfgDesk]
            # If a BCS was found or created, assign the Application Processes
            if curBCS is None:
                continue
            for propName in RelevantPropertyNames:
                curBCS.Properties[propName].Value = CStr(bcsi.Properties.get(propName))
            for appi in appisToAssign:
                if appi.Name not in dictAP:
                    continue
                spaths = bcsi.Properties.get(PROP_SEARCH_PATHS)
                if curBCS.Name == CM_BUILD_CFG_NAME and appi.Model is not None:
                    srch = [ CARMAKER_INC_DIR ]
                    if appi.ModelPath is not None:
                        mdir = os.path.dirname(appi.ModelPath)
                        pdir = os.path.dirname(mdir)
                        if pdir != "":
                            PROJECT_DIR = pdir
                            for d in SRC_DIRS:
                                srch.append("%s/%s" %(pdir, d))
                    spaths = JoinPaths(srch, "; ")
                curBCS.Properties[PROP_SEARCH_PATHS].Value = CStr(spaths)
                relBC.AddElements(curBCS, [dictAP[appi.Name]])
        return True

    def GetApplProcInfosForBcs(self, bcsi):
        res = []
        for appi in list(self.ApplProcInfos.values()):
            if appi.BcsName == bcsi.Name:
                res.append(appi)
        return res


# =============================================================================
# Class: MainController
# =============================================================================
class MainController(object):
    def __init__(self):
        # Initialize the ConfigurationDesk application object.
        self.ConfigurationDeskApplication = None

        # Initialize the project events object
        self.ProjectEvents = None

    def Initialize(self):
        # Start ConfigurationDesk and set it to the demo controller class.
        self.ConfigurationDeskApplication = client.Dispatch("ConfigurationDesk.Application")

        # ConfigurationDesk starts windowless if started via automation.
        self.ConfigurationDeskApplication.MainWindow.Visible = True

    def ConnectToEvents(self):
        # The project events are accessible via the ICaProjectEvents interface. The events
        # provider is the ICaProjectManagement object, the ProjectEvents class, which is
        # defined below, must be used in the same way like the ApplicationEvents.
        ProjectManagement = self.ConfigurationDeskApplication.ProjectManagement
        self.ProjectEvents = client.DispatchWithEvents(ProjectManagement, ProjectEvents)

    def Delete(self):
        del self.ConfigurationDeskApplication
        del self.ProjectEvents


# =============================================================================
# Class: ProjectEvents
# =============================================================================
class ProjectEvents(object):
    def OnApplicationLoaded(self, args):
        MakeCM_BuildConfig()

    def OnProjectClosing(self, args):
        DeleteProjectEventHandler()


def MakeCM_BuildConfig():
    global AppEvH
    global ProjEvH_act
    cmbc = CM_BuildConfig()
    rv = cmbc.Initialize()
    if cmbc is None or rv is False:
        return
    elif rv == "NoApp":
        if ProjEvH_act == 0:
            ProjectEventHandler()
        else:
            ShowMsg("No active application detected")
        return
    cmbc.ReadBuildConfigs()
    cbci = cmbc.CM_BuildConfigInfo()
    if cbci is None:
        cbci = BuildConfigSetInfo(None, CM_BUILD_CFG_NAME)
        cbci.CreateOnCfgDesk = True
        cbci.NameOnCfgDesk   = CM_BUILD_CFG_NAME
    cbci.SetCompDefines(cmbc.CM_COMPILER_DEFINES)
    cbci.SetCompUnDefines(cmbc.CM_COMPILER_UNDEFINES)
    cbci.SetSearchPaths(cmbc.CM_SEARCH_PATHS)
    cbci.SetCustomLibraries(cmbc.CM_LIBRARIES)
    cbci.SetCustomSrcFiles(cmbc.CM_SOURCE_FILES)
    cbci.SetCCompOpts(cmbc.CM_COMPILER_OPTIONS)
    cbci.SetCPPCompOpts(cmbc.CM_COMPILER_OPTIONS)
    if cmbc.ApplyBuildConfig(cbci) is False:
        ShowMsg("Failed to apply Build Configuration settings")
        return
    AppEvH = DispatchWithEvents("ConfigurationDesk.Application", CM_BuildEvents)
    return


def StartCM_GUI():
    global PROJECT_DIR
    TCL_DDE_EXECUTE_RESULT = "$TCLEVAL$EXECUTE$RESULT"
    cmdde = None
    try:
        cmdde = DDEClient("TclEval", "CarMaker")
        cmdde.execute("wm deiconify .; raise .")
    except(Exception):
        exepath = NormPath("%s/CM_HIL.exe" %(CARMAKER_BIN_DIR))
        subprocess.call(exepath)
        n_try = 0
        while n_try < 10:
            try:
                cmdde = DDEClient("TclEval", "CarMaker")
            except:
                time.sleep(1)
            n_try = n_try + 1
    finally:
        if PROJECT_DIR is None or cmdde is None:
            return
        cmdde.execute("""\
            set nprjdir [file normalize {%s}]
            if {$nprjdir ne $HIL(ProjDir)} {
                Project::Select $nprjdir
            }
            Appl::StartIPGRT""" %(PROJECT_DIR))
        del cmdde


def ConnectCM_GUI():
    TCL_DDE_EXECUTE_RESULT = "$TCLEVAL$EXECUTE$RESULT"
    cmdde = None
    try:
        cmdde = DDEClient("TclEval", "CarMaker")
        cmdde.execute("wm deiconify .; raise .")
        cmdde.execute("Appl::Connect DSx86_32")
    except(Exception):
        ShowMsg("CarMaker GUI: Connect failed!")
    finally:
        if cmdde is not None:
            del cmdde


def ProjectEventHandler():
    global CONTROLLER
    global ProjEvH_act

    try:
        # Create the controller which controls the signal chain handling examples in this script.
        CONTROLLER = MainController()

        # Call the initialize function of the DemoController to setup ConfigurationDesk.
        CONTROLLER.Initialize()

        # Connect to the events
        CONTROLLER.ConnectToEvents()
        ProjEvH_act = 1

    except Exception as exc:
        ShowMsg("Exception in script CM_BuildConfig.py: " + str(exc))


def DeleteProjectEventHandler():
    global CONTROLLER
    global ProjEvH_act

    if ProjEvH_act == 1:
        try:
            CONTROLLER.Delete()

        except Exception as exc:
            ShowMsg("Exception in script CM_BuildConfig.py: " + str(exc))

        ProjEvH_act = 0





# Main Program
if __name__ == '__main__' and ProjEvH_act == 0:
    MakeCM_BuildConfig()
