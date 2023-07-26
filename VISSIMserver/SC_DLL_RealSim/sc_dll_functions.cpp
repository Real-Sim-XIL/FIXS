/*==========================================================================*/
/*  SC_DLL_FUNCTIONS.CPP                                                    */
/*                                                                          */
/*  Access functions for the kernel of a signal controller DLL for VISSIM.  */
/*                                                                          */
/*  Version von 2005-09-30                                  Lukas Kautzsch  */
/*==========================================================================*/

#include "assert.h"
#include "lsa_fehl.h"
#include "lsa_puff.h"
#include "lsa_rahm.rh"
#include "sc_dll_functions.h"

#include <stdio.h>

#pragma unmanaged

/*==========================================================================*/
/*============================ General objects =============================*/
/*==========================================================================*/

template <class ObjType>
object_iterator<ObjType>::object_iterator(NumberToIndexMap& objects)
: objects_(objects)
, current_(objects.begin())
{
  /* Creates a new instance of the iterator. */
} /* object_iterator::object_iterator */

/*--------------------------------------------------------------------------*/

template <class ObjType>
object_iterator<ObjType>::~object_iterator(void)
{
  /* The default destructor. */
} /* object_iterator::~object_iterator */

/*--------------------------------------------------------------------------*/

template <class ObjType>
object_iterator<ObjType> & object_iterator<ObjType>::operator ++ (void)
{
  /* Sets the iterator to the next element. */
  ++current_;
  return *this;
} /* object_iterator::operator ++ */

/*--------------------------------------------------------------------------*/

template <class ObjType>
ObjType & object_iterator<ObjType>::operator * (void)
{
  /* Gets the element at the iterator position, if the iterator is valid. */
  return *current_->second;
} /* object_iterator::operator * */

/*--------------------------------------------------------------------------*/

template <class ObjType>
bool object_iterator<ObjType>::IsValid(void)
{
  /* Gets whether the iterator is valid or not. */
  return (current_ != objects_.end());
} /* object_iterator::IsValid */


template class object_iterator<T_Det>;
template class object_iterator<T_SigGr>;

/*==========================================================================*/
/*=========================== General functions ============================*/
/*==========================================================================*/

double  Sim_Time(void)
{
  /* Returns the simulation time at the end of the current time step */
  /* in VISSIM in seconds.                                           */

  return SimulationTime;
} /* Sim_Time */

/*--------------------------------------------------------------------------*/

TCHAR  *Sim_Time_of_Day(void)
{
  /* Returns the simulation time at the end of the current time step */
  /* in VISSIM as string in the form hh:mm:ss.s                      */

  return SimulationTimeOfDay;
} /* Sim_Time_of_Day */

/*--------------------------------------------------------------------------*/

double  Sim_Time_In_Seconds_Since_Midnight(void)
{
  /* Returns the simulation second since midnight in VISSIM in [s]. */
  return SimulationSecondsSinceMidnight;
}

/*--------------------------------------------------------------------------*/

long  Sim_Start_Date(void)
{
  /* Returns the start date of the simulation as defined by the user */
  /* in the VISSIM Simulation Parameters in the format YYYYMMDD.     */
  /* (Can be zero if this simulation parameter is empty.)            */

  return SimulationDate;
} /* Sim_Start_Date */

/*==========================================================================*/
/*======================= Signal controller functions ======================*/
/*==========================================================================*/

double  SC_CycleSecond(unsigned long sc_no)
{
  /* Returns the current cycle second [in seconds, >0 <=SC_CycleTime()] */
  /* of controller no. <sc_no>.                                         */
  /* If controller no. <sc_no> does not exist in the VISSIM network     */
  /* a runtime error occurs (and the return value is 0.0).              */

  if (!SetLSAPuffer(sc_no)) {
    return 0.0;
  }
  return LSA_Puffer->Sekunde;
} /* SC_CycleSecond */

/*--------------------------------------------------------------------------*/

long SC_Index(unsigned long sc_no)
{
  /* Returns a globally unique number for the signal controller. no.    */
  /* <sc_no>.                                                           */
  /* If controller no. <sc_no> does not exist in the VISSIM network     */
  /* a runtime error occurs (and the return value is 0).                */

  if (!SetLSAPuffer(sc_no)) {
    return 0;
  }

  return LSA_Puffer->Index;
}

/*--------------------------------------------------------------------------*/

void SC_SetOptimizationLevel(unsigned long sc_no, unsigned long optimizationLevel)
{
  /* Sets the required optimization level for signal controller. no.    */
  /* <sc_no>. Increasing the optimization level could increase the      */
  /* simulation speed.                                                  */
  /* If controller no. <sc_no> does not exist in the VISSIM network     */
  /* a runtime error occurs (and the return value is 0).                */
  if (!SetLSAPuffer(sc_no)) {
    return;
  }

  LSA_Puffer->OptimizationLevel = optimizationLevel;
}

/*--------------------------------------------------------------------------*/

tstring SC_GetThirdSupplyFile(unsigned long sc_no)
{
  /* Gets the third supply file of signal controller <sc_no>. This feature  */
  /* is not supported in the generic interface. It needs a special handling */
  /* in VISSIM.                                                             */
  /* If controller no. <sc_no> does not exist in the VISSIM network,        */
  /* a runtime error occurs and the return value is an empty string.        */

  if (!SetLSAPuffer(sc_no)) {
    return _T("");
  }

  return LSA_Puffer->DataFileName[2];
}

/*--------------------------------------------------------------------------*/

int  SC_SetCycleSecond (unsigned long sc_no, double cs)
{
  /* Sets the current cycle second of controller no. <sc_no> to <cs> */
  /* [in seconds]. This value is transferred to VISSIM at the end    */
  /* of the controller time step, rounded to a full controller time  */
  /* step (reciprocal of the controller frequency).                  */
  /* If controller no. <sc_no> does not exist in the VISSIM network  */
  /* a runtime error occurs and the return value is 0 (else 1).      */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  LSA_Puffer->Sekunde = cs;
  return 1;
} /* SC_SetCycleSecond */

/*--------------------------------------------------------------------------*/

double  SC_CycleTime (unsigned long sc_no)
{
  /* Returns the cycle time (in seconds) of controller no. <sc_no>  */
  /* (as defined in VISSIM).                                        */
  /* If controller no. <sc_no> does not exist in the VISSIM network */
  /* a runtime error occurs (and the return value is 0.0).          */

  if (!SetLSAPuffer (sc_no)) {
    return 0.0;
  }
  return LSA_Puffer->TU;
} /* SC_CycleTime */

/*--------------------------------------------------------------------------*/

int SC_SetCycleTime (unsigned long sc_no, double ct)
{
  /* Sets the cycle time (in seconds) of controller no. <sc_no>     */
  /* (as defined in VISSIM)                                         */
  /* If controller no. <sc_no> does not exist in the VISSIM network */
  /* a runtime error occurs and the function will return 0 else 1.  */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }

  LSA_Puffer->TU = ct;
  return 1;
}

/*--------------------------------------------------------------------------*/

int  SC_ProgramNumber (unsigned long sc_no)
{
  /* Returns the number of the running signal program of controller */
  /* no. <sc_no> (as defined in VISSIM).                            */
  /* If controller no. <sc_no> does not exist in the VISSIM network */
  /* a runtime error occurs (and the return value is 0).            */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  return LSA_Puffer->ProgNr;
} /* SC_ProgramNumber */

/*--------------------------------------------------------------------------*/

int  SC_SetProgramNumber (unsigned long sc_no, unsigned pn)
{
  /* Sets the number of the running signal program of controller no. */
  /* <sc_no> to <pn>. This value is transferred to VISSIM at the end */
  /* of the controller time step.                                    */
  /* If controller no. <sc_no> does not exist in the VISSIM network  */
  /* a runtime error occurs and the return value is 0 (else 1).      */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  LSA_Puffer->ProgNr = pn;
  return 1;
} /* SC_SetProgramNumber */

/*--------------------------------------------------------------------------*/

int  SC_WriteRecordValue (unsigned long sc_no, long code, long number, 
                          long value)
{
  /* Passes a new data triplet for the SC/Detector Record of      */
  /* controller no. <sc_no> to VISSIM, consisting of <code> (as   */
  /* defined in the WTT file), <number> and <value>. (The value   */
  /* ranges for the parameters are defined in the WTT file, too.) */
  /* If controller no. <sc_no> does not exist in the VISSIM       */
  /* network or if the triplet cannot be written, a runtime error */
  /* occurs (return value 0). Otherwise the return value is 1.    */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  SchreibeProtTripel (code, number, value);
  return 1;
} /* SC_WriteRecordValue */

/*--------------------------------------------------------------------------*/

long  SC_ReadInputChannel (unsigned long sc_no, long ch_no)
{
  /* If the input channel <ch_no> of controller no. <sc_no> is not active */
  /* (undefined in VISSIM), the return value is -1. Otherwise the return  */
  /* value is the last value sent from the controller output channel      */
  /* connected to that input channel.                                     */
  /* If controller no. <sc_no> does not exist in the VISSIM network, a    */
  /* runtime error occurs and the return value is -1.                     */

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  return LiesEingang (ch_no);
} /* SC_ReadInputChannel */

/*--------------------------------------------------------------------------*/

int  SC_SetOutputChannel (unsigned long sc_no, long ch_no, long value)
{
  /* Sets the output channel <ch_no> of controller no. <sc_no> to <value>. */
  /* If this doesn't work or if controller no. <sc_no> does not exist in   */
  /* the VISSIM network, a runtime error occurs and the return value is 0, */
  /* else 1.                                                               */

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  return SetzeAusgang (ch_no, value);
} /* SC_SetOutputChannel */

/*--------------------------------------------------------------------------*/

int  SC_Debug_Mode (unsigned long sc_no)
{
  /* Returns 1 if the use has put controller <sc_no> into debug mode, */
  /* else 0.                                                          */
  /* If controller no. <sc_no> does not exist in the VISSIM network,  */
  /* a runtime error occurs and the return value is -1.               */

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  return LiesDebugModus ();
} /* SC_Debug_Mode */

/*--------------------------------------------------------------------------*/

int  SC_SetFrequency (unsigned long sc_no, unsigned freq)
{
  /* Sets the controller frequency of controller no. <sc_no> to <freq>     */
  /* [1..10] controller time steps per simulation second.                  */
  /* The value is passed to VISSIM after the initialization of the control */
  /* program, so this function must be called from SC_DLL_Init()           */
  /* if the frequency is to be higher than the default value 1.            */
  /* If controller no. <sc_no> does not exist in the VISSIM network,       */
  /* a runtime error occurs and the return value is 0, else 1.             */

  if (!SetLSAPuffer (sc_no) || freq < 1 || freq > 10) {
    return 0;
  }
  SetzeSchaltfrequenz (freq);
  return 1;
} /* SC_SetFrequency */

/*==========================================================================*/
/*========================= Signal group functions =========================*/
/*==========================================================================*/

static  int  signal_state_code (SignalBild state)
{
  /* Returns the SG_STATE_... code equivalent of the SignalBild-type */
  /* value <state>.                                                  */

  switch (state) {
    case rot :
      return SG_STATE_RED;
    case rotgelb :
      return SG_STATE_RED_AMBER;
    case gruen :
      return SG_STATE_GREEN;
    case gelb :
      return SG_STATE_AMBER;
    case aus :
    case aus_rot :
      return SG_STATE_OFF;
    case undefiniert :
      return SG_STATE_UNDEFINED;
    case gelb_blink :
      return SG_STATE_AMBER_FLASHING;
    case rot_blink :
      return SG_STATE_RED_FLASHING;
    case gruen_blink :
      return SG_STATE_GREEN_FLASHING;
    case rgwb :
      return SG_STATE_RED_GREEN_FLASHING;
    case gruengelb :
      return SG_STATE_GREEN_AMBER;
    default :
      HoleRessourcenString (IDS_UNBEKANNTES_SIGNALBILD, res_string,
        sizeof (res_string));
      InternerFehler (_T("signal_state_code"), res_string);
      return SG_STATE_UNDEFINED;
  }
} /* signal_state_code */

/*==========================================================================*/

sg_iterator SignalGroups(void)
{
  return sg_iterator(LSA_Puffer->SgNoToObj);
}

/*--------------------------------------------------------------------------*/

int SGIdFromName(unsigned long sc_no, const tstring & name)
{
  /* Gets the id of the signal group with the given name.                     */
  /* If controller no. <sc_no> or signal group name. <name> does not exist in */
  /* the VISSIM network a runtime error occurs and the return value is -1.    */

  static TCHAR  function_name[] = _T("SGIdFromName");

  if (!SetLSAPuffer(sc_no)) {
    return -1;
  }

  T_SigGr * sg = sg_name_to_object(name, function_name);
  return sg == nullptr ? -1 : sg->VISSIM_Nr;
}

/*--------------------------------------------------------------------------*/

int  SG_Defined (unsigned long sc_no, unsigned long sg_no)
{
  /* If controller no. <sc_no> doesn't exist in VISSIM a runtime error  */
  /* occurs (return value 0).                                           */
  /* Otherwise the return value is 1 if signal group No. <sg_no> exists */
  /* in controller no. <sc_no> in VISSIM, otherwise 0.                  */
  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, NULL, false);
  return sg == nullptr ? 0 : 1;
} /* SG_Defined */

/*--------------------------------------------------------------------------*/

tstring SG_Name(unsigned long sc_no, unsigned long sg_no)
{
  /* Gets the name of the signal group with the given id.                   */
  static TCHAR  function_name[] = _T("SG_Name");

  if (!SetLSAPuffer (sc_no)) {
    return _T("");
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return _T("");
  }
  return sg->Name;
}

/*--------------------------------------------------------------------------*/

int  SG_SetState (unsigned long sc_no, unsigned long sg_no, int state, 
                  int transition)
{
  /* Set signal group no. <sg_no> of controller no. <sc_no> to <state>    */
  /* (see state definitions above).                                       */
  /* If <transition> != 0, a transition state will be inserted            */
  /* between the current state and the new state if necessary (defined    */
  /* by the cycle definition, amber and red/amber times in VISSIM),       */    
  /* else the signal group will change to the new state immediately.      */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or if <state> is not from the list above,      */
  /* a runtime error occurs and the return value is 0, else the           */
  /* return value is 1.                                                   */

  static TCHAR  function_name[] = _T("SG_SetState");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  VonLSA (state, sg_no, transition);
  return FehlerStatus == Okay;
} /* SG_SetState */

/*--------------------------------------------------------------------------*/

double SG_GreenEnd (unsigned long sc_no, unsigned long sg_no)
{
  /* Get the current green end of signal group no. <sg_no> of controller  */
  /* no. <sc_no> to <green_end>.                                          */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_GreenEnd");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  return sg->TGruenende;
}

/*--------------------------------------------------------------------------*/

double SG_RedEnd (unsigned long sc_no, unsigned long sg_no)
{
  /* Get the current red end of signal group no. <sg_no> of controller    */
  /* no. <sc_no> to <red_end>.                                            */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_RedEnd");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  return sg->TRotende;
}

/*--------------------------------------------------------------------------*/

int SG_SetGreenEnd (unsigned long sc_no, unsigned long sg_no, double green_end)
{
  /* Set the current green end of signal group no. <sg_no> of controller  */
  /* no. <sc_no> to <green_end>.                                          */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_SetGreenEnd");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  sg->TGruenende = green_end;

  return 1;
}

/*--------------------------------------------------------------------------*/

int SG_SetRedEnd (unsigned long sc_no, unsigned long sg_no, double red_end)
{
  /* Set the current red end of signal group no. <sg_no> of controller    */
  /* no. <sc_no> to <red_end>.                                            */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_SetRedEnd");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  sg->TRotende = red_end;

  return 1;
}

/*--------------------------------------------------------------------------*/

double SG_GreenEnd2 (unsigned long sc_no, unsigned long sg_no)
{
  /* Get the current second green end of signal group no. <sg_no> of      */
  /* controller no. <sc_no> to <green_end>.                               */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_GreenEnd2");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  return sg->TGruenende2;
}

/*--------------------------------------------------------------------------*/

double SG_RedEnd2 (unsigned long sc_no, unsigned long sg_no)
{
  /* Get the current second red end of signal group no. <sg_no> of        */
  /* controller no. <sc_no> to <red_end>.                                 */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_RedEnd2");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  return sg->TRotende2;
}

/*--------------------------------------------------------------------------*/

int SG_SetGreenEnd2 (unsigned long sc_no, unsigned long sg_no, double green_end)
{
  /* Set the current second green end of signal group no. <sg_no> of      */
  /* controller no. <sc_no> to <green_end>.                               */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_SetGreenEnd2");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  sg->TGruenende2 = green_end;

  return 1;
}

/*--------------------------------------------------------------------------*/

int SG_SetRedEnd2 (unsigned long sc_no, unsigned long sg_no, double red_end)
{
  /* Set the current second red end of signal group no. <sg_no> of        */
  /* controller no. <sc_no> to <red_end>.                                 */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network or a runtime error occurs and the return value */
  /* is 0, else the return value is 1.                                    */
  static TCHAR  function_name[] = _T("SG_SetRedEnd2");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }

  sg->TRotende2 = red_end;

  return 1;
}

/*--------------------------------------------------------------------------*/

int  SG_CurrentState (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the current state of signal group no. <sg_no> of controller  */
  /* no. <sc_no> (see state definitions above).                           */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0).                                                               */

  static TCHAR  function_name[] = _T("SG_CurrentState");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return signal_state_code (sg->Bild);
} /* SG_CurrentState */

/*--------------------------------------------------------------------------*/

double  SG_CurrentDuration (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the elapsed time (in seconds) since signal group no. <sg_no> */
  /* of controller no. <sc_no> was set to its current state.              */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0.0).                                                             */

  static TCHAR  function_name[] = _T("SG_CurrentDuration");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return sg->Laufzeit;
} /* SG_CurrentDuration */

/*--------------------------------------------------------------------------*/

int  SG_DesiredState (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the last desired state set by controller no. <sc_no> for     */
  /* signal group no. <sg_no> (see state definitions above).              */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0).                                                               */

  static TCHAR  function_name[] = _T("SG_DesiredState");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  if (sg->Soll == undefiniert) {
    return signal_state_code (sg->AltSoll);
  }
  else {
    return signal_state_code (sg->Soll);
  }
} /* SG_CurrentState */

/*--------------------------------------------------------------------------*/

double  SG_MinimumRed (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the minimum red time of signal group no. <sg_no> of controller */
  /* no. <sc_no> (in seconds, as defined in VISSIM).                        */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist   */
  /* in the VISSIM network a runtime error occurs (and the return value     */
  /* is 0.0).                                                               */

  static TCHAR  function_name[] = _T("SG_MinimumRed");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return sg->TRotMin;
} /* SG_MinimumRed */

/*--------------------------------------------------------------------------*/

double  SG_MinimumGreen (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the minimum green time of signal group no. <sg_no> of        */
  /* controller no. <sc_no> (in seconds, as defined in VISSIM).           */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0.0).                                                             */

  static TCHAR  function_name[] = _T("SG_MinimumGreen");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return sg->TGruenMin;
} /* SG_MinimumGreen */

/*--------------------------------------------------------------------------*/

double  SG_AmberPeriod (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the amber time of signal group no. <sg_no> of controller     */
  /* no. <sc_no> (in seconds, as defined in VISSIM).                      */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0.0).                                                             */

  static TCHAR  function_name[] = _T("SG_AmberPeriod");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return sg->TGelb;
} /* SG_AmberPeriod */

/*--------------------------------------------------------------------------*/

double  SG_RedAmberPeriod (unsigned long sc_no, unsigned long sg_no)
{
  /* Returns the red/amber time of signal group no. <sg_no> of controller */
  /* no. <sc_no> (in seconds, as defined in VISSIM).                      */
  /* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return value   */
  /* is 0.0).                                                             */

  static TCHAR  function_name[] = _T("SG_RedAmberPeriod");

  if (!SetLSAPuffer (sc_no)) {
    return 0;
  }
  T_SigGr * sg = sg_nr_to_object(sg_no, function_name, false);
  if (sg == nullptr) {
    return 0;
  }
  return sg->TRotgelb;
} /* SG_RedAmberPeriod */

/*==========================================================================*/
/*=========================== Detector functions ===========================*/
/*==========================================================================*/

detector_iterator Detectors()
{
  return detector_iterator(LSA_Puffer->DetNoToObj);
}

/*--------------------------------------------------------------------------*/

int  Det_Defined (unsigned long sc_no, unsigned long det_no)
{
  /* If controller no. <sc_no> doesn't exist in VISSIM a runtime error */
  /* occurs (return value -1).                                         */
  /* Otherwise the return value is 1 if detector No. <det_no> exists   */
  /* in controller no. <sc_no> in VISSIM, otherwise 0.                 */

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, NULL, false);
  return det == nullptr ? 0 : 1;
} /* Det_Defined */

/*--------------------------------------------------------------------------*/

int  Det_Detection (unsigned long sc_no, unsigned long det_no)
{
  /* Returns 1 if there is or was a vehicle on detector <det_no> of   */
  /* controller no. <sc_no> since the previous controller time step,  */
  /* else 0.                                                          */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist */
  /* in the VISSIM network a runtime error occurs (and the return     */
  /* value is -1).                                                    */

  static TCHAR  function_name[] = _T("Det_Detection");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false); 
  if (det == nullptr) {
    return -1;
  }
  if (   det->Anwesenheit
      || det->Vorderkanten > 0
      || det->Hinterkanten > 0) {
    return 1;
  }
  else {
    return 0;
  }
} /* Det_Detection */

/*--------------------------------------------------------------------------*/

int  Det_Presence (unsigned long sc_no, unsigned long det_no)
{
  /* Returns 1 if there is a vehicle on detector <det_no> of controller  */
  /* no. <sc_no> at the end of the current simulation time step, else 0. */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist    */
  /* in the VISSIM network a runtime error occurs (and the return        */
  /* value is -1).                                                       */

  static TCHAR  function_name[] = _T("Det_Presence");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1;
  }
  if (det->Anwesenheit) {
    return 1;
  }
  else {
    return 0;
  }
} /* Det_Presence */

/*--------------------------------------------------------------------------*/

int  Det_FrontEnds (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the number of detected vehicle front ends on detector <det_no> */
  /* of controller no. <sc_no> since the previous controller time step.     */
  /* (If a vehicle moves onto the detector while another one is still       */
  /* there, no new front end will be detected!)                             */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_FrontEnds");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1;
  }
  return det->Vorderkanten;
} /* Det_FrontEnds */

/*--------------------------------------------------------------------------*/

int  Det_RearEnds (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the number of detected vehicle rear ends on detector <det_no>  */
  /* of controller no. <sc_no> since the previous controller time step.     */
  /* (If a vehicle leaves the detector while another one is already on the  */
  /* detector, no new rear end will be detected!)                           */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_RearEnds");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1;
  }
  return det->Hinterkanten;
} /* Det_RearEnds */

/*--------------------------------------------------------------------------*/

double  Det_FrontEndTime (unsigned long sc_no, unsigned long det_no, int k)
{
  /* Returns the time in s between the start of the current controller time */
  /* step and the detection of the <k>-th vehicle front end during this     */
  /* controller time step. [1 <= k <= Det_FrontEnds (sc_no, det_no)]        */
  /* (If a vehicle moves onto the detector while another one is still       */
  /* there, no new front end will be detected!)                             */
  /* If <k> is smaller than 1 or higher than the value returned by          */
  /* Det_FrontEnds (<sc_no>, <det_no>), the return value is -1.             */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_FrontEndTime");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1;
  }
  if (k < 1 || k > det->Vorderkanten) {
    return -1;
  }
  return   (double) det->tVorderkante[k - 1]
         / (double) LSA_Puffer->Schaltfrequenz
         / 100.0;
} /* Det_FrontEndTime */

/*--------------------------------------------------------------------------*/

double  Det_RearEndTime (unsigned long sc_no, unsigned long det_no, int k)
{
  /* Returns the time in s between the start of the current controller time */
  /* step and the detection of the <k>-th vehicle rear end during this      */
  /* controller time step. [1 <= k <= Det_RearEnds (sc_no, det_no)]         */
  /* (If a vehicle leaves the detector while another one is already on the  */
  /* detector, no new rear end will be detected!)                           */
  /* If <k> is smaller than 1 or higher than the value returned by          */
  /* Det_RearEnds (<sc_no>, <det_no>), the return value is -1.              */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_RearEndTime");

  if (!SetLSAPuffer (sc_no)) {
    return -1;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1;
  }
  if (k < 1 || k > det->Hinterkanten) {
    return -1;
  }
  return   (double) det->tHinterkante[k-1] 
         / (double) LSA_Puffer->Schaltfrequenz
         / 100.0;
} /* Det_RearEndTime */

/*--------------------------------------------------------------------------*/

double  Det_OccupancyTime (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the current occupancy time on detector <det_no> of controller  */
  /* no. <sc_no>: 0.0 if no vehicle is present at the end of the current    */
  /* simulation time step, else the time elapsed since its arrival in s.    */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_OccupancyTime");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  if (det->Anwesenheit) {
    return (double) det->Belegungsdauer / 100.0;
  }
  else {
    return 0.0;
  }
} /* Det_OccupancyTime */

/*--------------------------------------------------------------------------*/

double Det_OccupancyRate (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the current occupancy rate of detector <det_no> of controller  */
  /* no. <sc_no> in % [0.0..100.0], i.e. the occupancy percentage since the */
  /* last controller time step.                                             */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_OccupancyRate");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  return det->aktuelleBelegung;
} /* Det_OccupancyRate */

/*--------------------------------------------------------------------------*/

double Det_OccupancyRateSmoothed (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the exponentially smoothed occupancy rate of detector <det_no> */
  /* of controller no. <sc_no> in % [0.0..100.0], i.e. the occupancy        */
  /* percentage since the last controller time step, smoothed exponentially */
  /* with the smoothing factors entered for this detector in VISSIM.        */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_OccupancyRateSmoothed");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  return det->geglaetteteBelegung;
} /* Det_OccupancyRateSmoothed */

/*--------------------------------------------------------------------------*/

double  Det_GapTime (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the current gap time on detector <det_no> of controller no.    */
  /* <sc_no>: 0.0 if a vehicle is present at the end of the current         */
  /* simulation time step, else the time elapsed since the last vehicle has */
  /* left the detector in s.                                                */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_GapTime");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  return (double) det->angefangeneZeitluecke / 100.0;
} /* Det_GapTime */

/*--------------------------------------------------------------------------*/

double  Det_VehSpeed (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the current vehicle speed on detector <det_no> of controller   */
  /* no. <sc_no>: 0.0 if no vehicle was detected since the last controller  */
  /* time step, else the speed of the last vehicle detected in m/s.         */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_VehSpeed");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  return (double) det->v / 100.0;
} /* Det_VehSpeed */

/*--------------------------------------------------------------------------*/

double  Det_VehLength (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the current vehicle length on detector <det_no> of controller  */
  /* no. <sc_no>: 0.0 if no vehicle was detected since the last controller  */
  /* time step, else the length of the last vehicle detected in m.          */
  /* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
  /* the VISSIM network a runtime error occurs and the return value is -1.  */

  static TCHAR  function_name[] = _T("Det_VehLength");

  if (!SetLSAPuffer (sc_no)) {
    return -1.0;
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return -1.0;
  }
  return (double) det->l / 1000.0;
} /* Det_VehLength */

/*--------------------------------------------------------------------------*/

int DetIdFromName(unsigned long sc_no, const tstring & name)
{
  /* Gets the id of the detector with the given name.                       */
  static TCHAR  function_name[] = _T("DetIdFromName");

  if (!SetLSAPuffer(sc_no)) {
    return -1;
  }

  T_Det * det = det_name_to_object(name, function_name);

  return det == nullptr ? -1 : det->VISSIM_Nr;
}

/*--------------------------------------------------------------------------*/

tstring Det_Name(unsigned long sc_no, unsigned long det_no)
{
  /* Gets the name of the detector with the given id.                       */
  static TCHAR  function_name[] = _T("Det_Name");

  if (!SetLSAPuffer (sc_no)) {
    return _T("");
  }
  T_Det * det = det_nr_to_object (det_no, function_name, false);
  if (det == nullptr) {
    return _T("");
  }
  return det->Name;
}

/*--------------------------------------------------------------------------*/

short Det_Type (unsigned long sc_no, unsigned long det_no)
{
  /* Returns the detector type. With use of type it is possible to differ   */
  /* between detectors for pedestrians and detectors for the traffic.       */

  TCHAR function_name[] = _T("Det_Type");

  if (!SetLSAPuffer (sc_no)) {
    return UndefinedDetector;
  }

  T_Det * det = det_nr_to_object(det_no, function_name, false);

  if (det == nullptr) {
    return UndefinedDetector;
  }

  return det->Type;
}
/*==========================================================================*/
/*========================= Calling point functions ========================*/
/*==========================================================================*/

int  CP_Telegrams (void) 
{
  /* Returns the number of public transport telegrams received */
  /* by calling points in the current controller time step.    */

  return Anzahl_OEV_Tele ();
} /* CP_Telegrams */

/*--------------------------------------------------------------------------*/

long  CP_Tele_CP (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the calling point number from the      */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_MP (index);
} /* CP_Tele_CP */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Line (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the line number from the               */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Linie (index);
} /* CP_Tele_Line */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Course (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the course number from the             */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Kurs (index);
} /* CP_Tele_Course */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Route (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the route number from the              */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Route (index);
} /* CP_Tele_Route */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Prio (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the priority value from the            */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Prio (index);
} /* CP_Tele_Prio */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Length (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the tram length value from the         */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Zuglaenge (index);
} /* CP_Tele_Length */

/*--------------------------------------------------------------------------*/

long  CP_Tele_ManualDC (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the manual direction code from the     */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_RVH (index);
} /* CP_Tele_ManualDC */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Delay (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the delay (in s) from the              */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Fahrplanlage (index);
} /* CP_Tele_Delay */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Passengers (unsigned index)
{
  /* If <index> is larger than the number of public transport telegrams   */
  /* received in the current simulation time step the return value is -1. */
  /* Otherwise the return value is the number of passengers from the      */
  /* public transport telegram number <index> [1..n].                     */

  return OEV_Tele_Personen (index);
} /* CP_Tele_Passengers */

/*==========================================================================*/
/*=========================== Network functions ============================*/
/*==========================================================================*/

int  Net_SetRouteFlow (long dec_no, long route_no, long rel_flow)
{
  /* Sets the relative flow of route <route_no> of routing */
  /* decision <dec_no> to the value <rel_flow>.            */
  /* If that doesn't work, a runtime error occurs (return  */
  /* value 0). Otherwise the return value is 1.            */

  return SetzeRoute (dec_no, route_no, rel_flow);
} /* Net_SetRouteFlow */

/*--------------------------------------------------------------------------*/

int  Net_SetDesSpeedDec (long dec_no, long veh_cl, long speed_dist)
{
  /* Assigns the desired speed distribution <speed_dist>  */
  /* to the vehicle class <veh_cl> in the desired speed   */
  /* decision <dec_no>.                                   */
  /* If that doesn't work, a runtime error occurs (return */
  /* value 0). Otherwise the return value is 1.           */

 return SetzeVWunsch (dec_no, veh_cl, speed_dist);
} /* Net_SetDesSpeedDec */

/*--------------------------------------------------------------------------*/

int  Net_SetStopTimeDist (long stop, long veh_cl, long speed_dist)
{
  /* Assigns the desired speed distribution <speed_dist>    */
  /* to the vehicle class <veh_cl> in the stop sign <stop>. */
  /* If that doesn't work, a runtime error occurs (return   */
  /* value 0). Otherwise the return value is 1.             */

  return SetStopTimeDist (stop, veh_cl, speed_dist);
} /* Net_SetStopTimeDist */

/*==========================================================================*/
/*============================ Error functions =============================*/
/*==========================================================================*/

void  ErrFileMessage (TCHAR *message)
{
  /* Writes the text <message> to the current *.err file. */
  /* The filename is the SC DLL filename plus ".err".     */
  /* The *.err file will be opened if necessary, and at   */
  /* the end of the simulation run a dialog box will tell */
  /* the user that an *.err file has been created.        */

  DateiMeldung (message);
} /* ErrFileMessage */

/*--------------------------------------------------------------------------*/

void  ScreenMessage (TCHAR *message)
{
  /* Displays the text <message> in a dialog box on the screen. */
  /* The user has to click "OK" to continue.                    */

  BildschirmMeldung (message);
} /* ErrFileMessage */

/*--------------------------------------------------------------------------*/

bool IsConsoleMode (void)
{
  /* Returns whether VISSIM is started in a non interactive     */
  /* version.                                                   */

  return IstConsoleModus();
} /* IsConsoleMode */

/*--------------------------------------------------------------------------*/

bool IsSimulationCanceled (void)
{
  /* Is true, if the simulation was canceled                   */

  return IstSimulationAbgebrochen();
} /* IsSimulationCanceled */

/*--------------------------------------------------------------------------*/

void StopSimulation(void)
{
  /* Stops the simulation. This function should be called if an error */
  /* occurred in the controller that prevent from continuing the      */
  /* simulation.                                                      */

  FehlerStatus = Abbruch;
}

/*--------------------------------------------------------------------------*/

int ProcessSimRunID (void)
{
  /* Returns the code sent from VISSIM for process ID and simulation run */
  /* index:  100 * process ID + run index.                               */
  /* (process ID = 0 except in the parallel VISSIM version.)             */
  /* If the return value is not NO_PROCESS_ID it should be included in   */
  /* output file names to prevent conflicts with other processes and     */ 
  /* overwriting of files created in earlier simulation runs.            */

  return GetProcessId ();
} /* ProcessSimRunID */

/*==========================================================================*/
/*  End of SC_DLL_FUNCTIONS.CPP                                             */
/*==========================================================================*/
