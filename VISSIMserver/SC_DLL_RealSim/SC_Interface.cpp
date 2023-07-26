/*==========================================================================*/
/*  SC_Interface.cpp                                 DLL Module for VISSIM  */
/*                                                                          */
/*  Interface module for signal controller DLLs.                            */
/*                                                                          */
/*  Version of 2011-03-25                                   Lukas Kautzsch  */
/*==========================================================================*/
#include "SC_Interface.h"
#include "lsa_fehl.h"
#include "lsa_puff.h"
#include "lsa_rahm.h"
#include "lsa_fehl.h"
#include "lsa_rahm.rh"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#pragma unmanaged

extern  TCHAR errorfile_name[255];

/*==========================================================================*/


HMODULE CONTROLLER_HMODULE = NULL;
TCHAR CONTROLLER_DLL_NAME[MAX_PATH];

static  bool  demo_version = false;

/*==========================================================================*/

BOOL APIENTRY DllMain (HANDLE  hModule, 
                       DWORD   ul_reason_for_call, 
                       LPVOID  lpReserved)
{
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
      CONTROLLER_HMODULE = (HMODULE)hModule;
      GetModuleFileName (CONTROLLER_HMODULE, CONTROLLER_DLL_NAME, MAX_PATH);
      break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      ExitLSAFehler ();
      break;
  }
  return TRUE;
}

/*==========================================================================*/

static int ControllerSetValueT(long   type,
                               long   sc_no,
                               long   index1,
                               long   index2,
                               long   long_value,
                               double double_value,
                               TCHAR  *string_value)
{
  if (sc_no == 0) {
    /* Values that are relevant for the library, not for a specific      */
    /* signal controller.                                                */
    switch (type) {
      case SC_DLL_DATA_LICENSE_CONTROLLERS:
        return 1;

      case SC_DLL_DATA_CONSOLE_MODE:
        SetConsoleMode(long_value != 0);
        return 1;

      case SC_DLL_DATA_PROCESS_ID:
        SetProcessId(long_value);
        return 1;

      case SC_DLL_DATA_SIM_CANCELED:
        SetSimulationCanceled(long_value != 0);
        return 1;

      default:
        return 0;
    }
  }
  else {
    /* Values that are relevant for a specific signal controller         */

    if (!SetLSAPuffer (sc_no)) return 0;

    switch (type) {
      case SC_DLL_DATA_LICENSE_CONTROLLERS :
        /* ### return 0 if the relevant bit is not set in long_value */
        return 1;
      case SC_DLL_DATA_SNAPSHOT_FILE :
        SnapshotFilename = tstring(string_value);
        return 1;

      case SC_DLL_DATA_DATA_FILE :
        if (index1 < 1 || index1 > MAX_DATA_FILES) {
          return 0;
        }
        LSA_Puffer->DataFileName[index1 - 1] = tstring(string_value);
        return 1;

      case DEMOVERSION :
        demo_version = TRUE;
        return 1;

      case SC_DLL_DATA_SIM_TIME_OF_DAY :
        _tcscpy(SimulationTimeOfDay, string_value);
        return 1;

      case SC_DLL_DATA_SIM_DATE :
        SimulationDate = long_value;
        return 1;

      case SC_DLL_DATA_SIM_TIME :
      case SC_DLL_DATA_CYCLE_TIME :
      case SC_DLL_DATA_CYCLE_SECOND :
      case SC_DLL_DATA_SG_T_GREEN_MIN :
      case SC_DLL_DATA_SG_T_RED_MIN :
      case SC_DLL_DATA_SG_T_AMBER :
      case SC_DLL_DATA_SG_T_RED_AMBER :
      case SC_DLL_DATA_SG_CURR_DURATION :
        return DoubleToLSAPuffer (type, index1, double_value);

      case SC_DLL_DATA_CONSOLE_MODE:
        SetConsoleMode(long_value != 0);
        return 1;

      case SC_DLL_DATA_PROCESS_ID:
        SetProcessId(long_value);
        return 1;

      case SC_DLL_DATA_DETECTOR_NAME:
        return StringToLSAPuffer(type, index1, tstring(string_value));

      case SC_DLL_DATA_SG_NAME:
        return StringToLSAPuffer(type, index1, tstring(string_value));

      case SC_DLL_DATA_INDEX:
        return LongToLSAPuffer(type, index1, long_value);

      default :
        return LongToLSAPuffer (type, index1, long_value);
    } /* switch (type) */
  }
}

/*--------------------------------------------------------------------------*/

#ifdef _UNICODE
SC_DLL_API  int  ControllerSetValueW(long   type,
                                    long   sc_no,
                                    long   index1,
                                    long   index2,
                                    long   long_value,
                                    double double_value,
                                    const wchar_t   *string_value)
{
  /* Called from VISSIM to set the value of a data object of type <type> */
  /* (see definition of type numbers above) of controller no. <sc_no>,   */
  /* selected by <index1> and possibly <index2>, to <long_value>,        */
  /* <double_value> or <*string_value> (object and value selection       */
  /* depending on <type>).                                               */
  /* Return value is 1 on success, otherwise 0.                          */
  return ControllerSetValueT(type, sc_no, index1, index2, long_value, double_value, const_cast<wchar_t*>(string_value));
} 
#endif
/*--------------------------------------------------------------------------*/

SC_DLL_API  int  ControllerSetValue(long   type,
                                    long   sc_no,
                                    long   index1,
                                    long   index2,
                                    long   long_value,
                                    double double_value,
                                    char   *string_value)
{
  /* Called from VISSIM to set the value of a data object of type <type> */
  /* (see definition of type numbers above) of controller no. <sc_no>,   */
  /* selected by <index1> and possibly <index2>, to <long_value>,        */
  /* <double_value> or <*string_value> (object and value selection       */
  /* depending on <type>).                                               */
  /* Return value is 1 on success, otherwise 0.                          */

#ifdef _UNICODE
  if (string_value != nullptr) {
    std::string stringA(string_value);
    std::wstring stringW(stringA.size(), L' ');
    stringW.resize(std::mbstowcs(&stringW[0], stringA.c_str(), stringA.size()));
    return ControllerSetValueT(type, sc_no, index1, index2, long_value, double_value, const_cast<wchar_t*>(stringW.c_str()));
  }
  else {
    return ControllerSetValueT(type, sc_no, index1, index2, long_value, double_value, nullptr);
  }
#else
  return ControllerSetValueT(type, sc_no, index1, index2, long_value, double_value, string_value);
#endif
} /* ControllerSetValue  */

/*--------------------------------------------------------------------------*/

P_ProtTripel  ppt_by_index (long sc_no, long index) 
{
  /* Returns a pointer to the <index>th record triple of controller */
  /* number <sc_no>. Is faster if it is called with increasing      */
  /* <index> values.                                                */

          P_ProtTripel  ppt, start_ppt;
          long          i, start_index;
  static  P_ProtTripel  last_ppt = NULL;
  static  long          last_sc_no = 0, last_index = 0;

  if (sc_no == last_sc_no) {
    if (index == last_index) {
      return last_ppt;
    }
    else if (index > last_index) {
      start_index = last_index;
      start_ppt   = last_ppt;
    }
    else {
      start_index = 1;
      start_ppt   = LSA_Puffer->ProtTripel;
    }
  }
  else {
    last_sc_no = sc_no;
    start_index = 1;
    start_ppt   = LSA_Puffer->ProtTripel;
  }
  for (ppt = start_ppt, i = start_index;
       ppt != NULL && i < index; 
       ppt = ppt->next, i++);
  last_ppt = ppt;
  last_index = index;
  return ppt;
} /* ppt_by_index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int ControllerGetValueT(long   type,
                               long   sc_no,
                               long   index1,
                               long   index2,
                               long   *long_value,
                               double *double_value,
                               TCHAR  *string_value)
{
  /* Called from VISSIM to get the value of a data object of type <type>  */
  /* (see definition of type numbers above) of controller no. <sc_no>,    */
  /* selected by <index1> and possibly <index2>.                          */
  /* This function writes that value to <*long_value>, <*double_value>    */
  /* or <*string_value> (object and value selection depending on <type>). */
  /* Return value is 1 on success, otherwise 0.                           */
  TCHAR proc_name[] = _T("ControllerGetValue");
  P_ProtTripel  ppt;

  if (sc_no == 0) {
    switch (type) {
      case SC_DLL_CHECK_SIGNAL_SEQUENCE:
        *long_value = 1;
        return 1;

      default:
        return 0;
    }
  }
  else {
    if (!SetLSAPuffer (sc_no)) return 0;

    switch (type) {      
      case SC_DLL_DATA_TERMINATION:
        if (FehlerStatus == Okay) {       
          if (_tcslen(errorfile_name) != 0) {
            *long_value = TERM_ERRORFILE_CREATED;
            _tcscpy(string_value, errorfile_name);
            // Reset file name to prevent from further error messages
            _tcscpy(errorfile_name, _T(""));
          }
          else {
            *long_value = TERM_SUCCESSFUL;
          }
        }
        else {
          *long_value = TERM_ABORT;
        }
        return 1;

      case SC_DLL_DATA_SG_TRANSITION :
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *long_value = sg->Uebergang;
          return 1;
        }
      }
      case SC_DLL_DATA_NO_OF_RECORD_DATA :
        *long_value = LSA_Puffer->AnzProtTripel;
        return 1;
      case SC_DLL_DATA_RECORD_DATA_TYPE :
        ppt = ppt_by_index (sc_no, index1);
        if (ppt == NULL) {
          return 0;
        }
        *long_value = ppt->Code;
        return 1;
      case SC_DLL_DATA_RECORD_DATA_NUMBER :
        ppt = ppt_by_index (sc_no, index1);
        if (ppt == NULL) {
          return 0;
        }
        *long_value = ppt->Nummer;
        return 1;
      case SC_DLL_DATA_RECORD_DATA_VALUE :
        ppt = ppt_by_index (sc_no, index1);
        if (ppt == NULL) {
          return 0;
        }
        *long_value = ppt->Wert;
        return 1;
      case SCHALTFREQUENZ :
        *long_value = LSA_Puffer->Schaltfrequenz;
        return 1;
      case UMLAUFSEKUNDE :
        *long_value = (long) (LSA_Puffer->Sekunde * LSA_Puffer->Schaltfrequenz + 0.001);
        return 1;
      case PROGRAMMNUMMER :
        *long_value = LSA_Puffer->ProgNr;
        return 1;
      case SOLL_BILD :
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *long_value = puffer_bild(sg->Soll);
          return 1;
        }
      }
      case IMPULSSPEICHER :
      {
        T_Det * det = det_nr_to_object(index1, proc_name, false);
        if (det == nullptr) {
          return 0;
        }
        else {
          *long_value = det->ImpulsspeicherLoeschen;
          return 1;
        }
      }
      case LSAKOPPLUNG :
      {
        for (EingangListe al = LSA_Puffer->AlleAusgaenge; al != NULL; al = al->next) {
          if (al->inhalt->nummer == index1) {
            *long_value = al->inhalt->wert;
            return 1;
          }
        }
        *long_value = 0;
        return 1;
      }
      case SC_DLL_DATA_SG_GREENEND:
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *double_value = sg->TGruenende;
          return 1;
        }

        return 1;
      }
      case SC_DLL_DATA_SG_REDEND:
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *double_value = sg->TRotende;
          return 1;
        }

        return 1;
      }
      case SC_DLL_DATA_SG_GREENEND2:
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *double_value = sg->TGruenende2;
          return 1;
        }

        return 1;
      }
      case SC_DLL_DATA_SG_REDEND2:
      {
        T_SigGr * sg = sg_nr_to_object(index1, proc_name, false);
        if (sg == nullptr) {
          return 0;
        }
        else {
          *double_value = sg->TRotende2;
          return 1;
        }

        return 1;
      }
      case SC_DLL_DATA_CYCLE_TIME:
      {
        *double_value = LSA_Puffer->TU;
        return 1;
      }
      default :
        return 0;
    }
  }

  return 0;
} /* ControllerGetValue  */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef _UNICODE
SC_DLL_API  int  ControllerGetValueW(long   type,
                                     long   sc_no,
                                     long   index1,
                                     long   index2,
                                     long   *long_value,
                                     double *double_value,
                                     wchar_t  *string_value)
{
  /* Called from VISSIM to get the value of a data object of type <type>  */
  /* (see definition of type numbers above) of controller no. <sc_no>,    */
  /* selected by <index1> and possibly <index2>.                          */
  /* This function writes that value to <*long_value>, <*double_value>    */
  /* or <*string_value> (object and value selection depending on <type>). */
  /* Return value is 1 on success, otherwise 0.                           */
  return ControllerGetValueT(type, sc_no, index1, index2, long_value, double_value, string_value);
}
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

SC_DLL_API  int  ControllerGetValue (long   type,
                                     long   sc_no,
                                     long   index1,
                                     long   index2,
                                     long   *long_value,
                                     double *double_value,
                                     char   *string_value)
{
  /* Called from VISSIM to get the value of a data object of type <type>  */
  /* (see definition of type numbers above) of controller no. <sc_no>,    */
  /* selected by <index1> and possibly <index2>.                          */
  /* This function writes that value to <*long_value>, <*double_value>    */
  /* or <*string_value> (object and value selection depending on <type>). */
  /* Return value is 1 on success, otherwise 0.          
  */
#ifdef _UNICODE
  if (string_value != nullptr) {
    wchar_t wstring_value[256];
    int returnValue = ControllerGetValueT(type, sc_no, index1, index2, long_value, double_value, wstring_value);
    size_t returnSize;
    wcstombs_s(&returnSize, string_value, 255, wstring_value, _TRUNCATE);
    return returnSize;
  }
  else{
    return ControllerGetValueT(type, sc_no, index1, index2, long_value, double_value, nullptr);
  }
#else
  return ControllerGetValueT(type, sc_no, index1, index2, long_value, double_value, string_value);
#endif
} /* ControllerGetValue  */

/*==========================================================================*/

SC_DLL_API  int  ControllerExecuteCommand (long number, long sc_no)
{
  /* Called from VISSIM to execute the command <number> if that is */
  /* available in the DLL (see definitions above) for controller   */
  /* no. <sc_no>.                                                  */
  /* Return value is 1 on success, otherwise 0.                    */
  
  TCHAR  filename [300];

  switch (number) {
    case SC_DLL_COMMAND_CREATE_CONTROLLER :
      InitLSAPuffer (sc_no);
      if (!SetLSAPuffer (sc_no)) return 0;
      InitDataFromSIM ();
      InitDataToSIM ();
      return 1;
    case SC_DLL_COMMAND_READ_DATA_FILES :
      if (!SetLSAPuffer (sc_no)) return 0;
      SteuerungDateiLesen ();
      if (FehlerStatus != Okay) return 0;
      return 1;
    case SC_DLL_COMMAND_INIT_CONTROLLER :
      if (!SetLSAPuffer (sc_no)) return 0;
      SteuerungInitialisierung ();
      if (FehlerStatus != Okay) return 0;
      return 1;
    case SC_DLL_COMMAND_RUN_CONTROLLER :
#ifdef _DEMO_
      if (!demo_version) {
        HoleRessourcenString (IDS_LSA_DEMO, res_string, sizeof (res_string));
        AllgemeinerFehler ("ControllerExecuteCommand", res_string);
        return 0;
      }
#endif
      if (!SetLSAPuffer (sc_no)) return 0;
      InitDataToSIM ();
      SteuerungBerechnung ();
      if (FehlerStatus != Okay) return 0;
      InitDataFromSIM ();
      return 1;
    case SC_DLL_COMMAND_EXIT_CONTROLLER :
      if (!SetLSAPuffer (sc_no)) return 0;
      Aufraeumen ();
      ExitLSAPuffer (sc_no);
      if (FehlerStatus != Okay) return 0;
      return 1;
    case SC_DLL_COMMAND_WRITE_SNAPSHOT :
      if (!SetLSAPuffer (sc_no)) return 0;
      _stprintf (filename, _T("%s%lu"), SnapshotFilename, sc_no);
      ZustandSchreiben (filename);
      if (FehlerStatus != Okay) return 0;
      return 1;
    case SC_DLL_COMMAND_READ_SNAPSHOT :
      if (!SetLSAPuffer (sc_no)) return 0;
      _stprintf (filename, _T("%s%lu"), SnapshotFilename, sc_no);
      ZustandLesen (filename);
      if (FehlerStatus != Okay) return 0;
      return 1;
    default :
      return 0;
  }
} /* ControllerExecuteCommand  */

/*==========================================================================*/
/*  End of SC_Interface.cpp                                                 */
/*==========================================================================*/
