/*==========================================================================*/
/*  SC_DLL_FUNCTIONS.H                                                      */
/*                                                                          */
/*  Access functions for the kernel of a signal controller DLL for VISSIM.  */
/*                                                                          */
/*  Version von 2009-10-19                                  Lukas Kautzsch  */
/*==========================================================================*/

#ifndef  __SC_DLL_FUNCTIONS_H
#define  __SC_DLL_FUNCTIONS_H

#include <map>
#include <tchar.h>
#include <string>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif
/*==========================================================================*/
/*=========================== Object iterators =============================*/
/*==========================================================================*/

/* Iterates over all objects and returns the detector ids.       */
/* The iterator can be used to iterate over all existing         */
/* detectors or signal controls.                                 */
template <class ObjType>
class object_iterator {
public:
  typedef  std::map<unsigned long, ObjType*> NumberToIndexMap;

  object_iterator(std::map<unsigned long, ObjType*> & objects);
  virtual ~object_iterator(void);

  object_iterator & operator ++(void);
  ObjType & operator * (void);

  bool IsValid(void);

private:
  std::map<unsigned long, ObjType*> & objects_;
  typename std::map<unsigned long, ObjType*>::iterator current_;
};

/*==========================================================================*/

struct T_Det;
struct T_SigGr;

typedef object_iterator<T_Det> detector_iterator;
typedef object_iterator<T_SigGr> sg_iterator;

/*==========================================================================*/
/*=========================== General functions ============================*/
/*==========================================================================*/

double  Sim_Time (void);

/* Returns the simulation time at the end of the current time step */
/* in VISSIM in seconds.                                           */

/*--------------------------------------------------------------------------*/

TCHAR *Sim_Time_of_Day (void);

/* Returns the simulation time at the end of the current time step */
/* in VISSIM as string in the form hh:mm:ss.s                      */

/*--------------------------------------------------------------------------*/

double  Sim_Time_In_Seconds_Since_Midnight(void);
/* Returns the simulation second since midnight in VISSIM in [s]. */

/*--------------------------------------------------------------------------*/

long  Sim_Start_Date (void);

/* Returns the start date of the simulation as defined by the user */
/* in the VISSIM Simulation Parameters in the format YYYYMMDD.     */
/* (Can be zero if this simulation parameter is empty.)            */

/*==========================================================================*/
/*======================= Signal controller functions ======================*/
/*==========================================================================*/

long SC_Index (unsigned long sc_no);

/* Returns a globally unique number for the signal controller. no.    */
/* <sc_no>.                                                           */
/* If controller no. <sc_no> does not exist in the VISSIM network     */
/* a runtime error occurs (and the return value is 0).                */

/*--------------------------------------------------------------------------*/

void SC_SetOptimizationLevel(unsigned long sc_no, unsigned long optimizationLevel);

/* Sets the required optimization level for signal controller. no.    */
/* <sc_no>. Increasing the optimization level could increase the      */
/* simulation speed.                                                  */
/* If controller no. <sc_no> does not exist in the VISSIM network     */
/* a runtime error occurs (and the return value is 0).                */

/*--------------------------------------------------------------------------*/

double  SC_CycleSecond (unsigned long sc_no);

/* Returns the current cycle second [in seconds, >0 <=SC_CycleTime()] */
/* of controller no. <sc_no>.                                         */
/* If controller no. <sc_no> does not exist in the VISSIM network     */
/* a runtime error occurs (and the return value is 0.0).              */

/*--------------------------------------------------------------------------*/

int  SC_SetCycleSecond (unsigned long sc_no, double cs);

/* Sets the current cycle second of controller no. <sc_no> to <cs> */
/* [in seconds]. This value is transferred to VISSIM at the end    */
/* of the controller time step, rounded to a full controller time  */
/* step (reciprocal of the controller frequency).                  */
/* If controller no. <sc_no> does not exist in the VISSIM network  */
/* a runtime error occurs and the return value is 0 (else 1).      */

/*--------------------------------------------------------------------------*/

double  SC_CycleTime (unsigned long sc_no);

/* Returns the cycle time (in seconds) of controller no. <sc_no>  */
/* (as defined in VISSIM).                                        */
/* If controller no. <sc_no> does not exist in the VISSIM network */
/* a runtime error occurs (and the return value is 0.0).          */

/*--------------------------------------------------------------------------*/

int SC_SetCycleTime (unsigned long sc_no, double ct);

/* Sets the cycle time (in seconds) of controller no. <sc_no>     */
/* (as defined in VISSIM)                                         */
/* If controller no. <sc_no> does not exist in the VISSIM network */
/* a runtime error occurs and the function will return 0 else 1.  */


/*--------------------------------------------------------------------------*/

int  SC_ProgramNumber (unsigned long sc_no);

/* Returns the number of the running signal program of controller */
/* no. <sc_no> (as defined in VISSIM).                            */
/* If controller no. <sc_no> does not exist in the VISSIM network */
/* a runtime error occurs (and the return value is 0).            */

/*--------------------------------------------------------------------------*/

int  SC_SetProgramNumber (unsigned long sc_no, unsigned pn);

/* Sets the number of the running signal program of controller no. */
/* <sc_no> to <pn>. This value is transferred to VISSIM at the end */
/* of the controller time step.                                    */
/* If controller no. <sc_no> does not exist in the VISSIM network  */
/* a runtime error occurs and the return value is 0 (else 1).      */

/*--------------------------------------------------------------------------*/

int  SC_WriteRecordValue (unsigned long sc_no, long code, long number, 
                          long value);

/* Passes a new data triplet for the SC/Detector Record of      */
/* controller no. <sc_no> to VISSIM, consisting of <code> (as   */
/* defined in the WTT file), <number> and <value>. (The value   */
/* ranges for the parameters are defined in the WTT file, too.) */
/* If controller no. <sc_no> does not exist in the VISSIM       */
/* network or if the triplet cannot be written, a runtime error */
/* occurs (return value 0). Otherwise the return value is 1.    */

/*--------------------------------------------------------------------------*/

long  SC_ReadInputChannel (unsigned long sc_no, long ch_no);

/* If the input channel <ch_no> of controller no. <sc_no> is not active */
/* (undefined in VISSIM), the return value is -1. Otherwise the return  */
/* value is the last value sent from the controller output channel      */
/* connected to that input channel.                                     */
/* If controller no. <sc_no> does not exist in the VISSIM network, a    */
/* runtime error occurs and the return value is -1.                     */

/*--------------------------------------------------------------------------*/

int  SC_SetOutputChannel (unsigned long sc_no, long ch_no, long value);

/* Sets the output channel <ch_no> of controller no. <sc_no> to <value>. */
/* If this doesn't work or if controller no. <sc_no> does not exist in   */
/* the VISSIM network, a runtime error occurs and the return value is 0, */
/* else 1.                                                               */

/*--------------------------------------------------------------------------*/

int  SC_Debug_Mode (unsigned long sc_no);

/* Returns 1 if the use has put controller <sc_no> into debug mode, */
/* else 0.                                                          */
/* If controller no. <sc_no> does not exist in the VISSIM network,  */
/* a runtime error occurs and the return value is -1.               */

/*--------------------------------------------------------------------------*/

int  SC_SetFrequency (unsigned long sc_no, unsigned freq);

/* Sets the controller frequency of controller no. <sc_no> to <freq>     */
/* [1..10] controller time steps per simulation second.                  */
/* The value is passed to VISSIM after the initialization of the control */
/* program, so this function must be called from SC_DLL_Init()           */
/* if the frequency is to be higher than the default value 1.            */
/* If controller no. <sc_no> does not exist in the VISSIM network,       */
/* a runtime error occurs and the return value is 0, else 1.             */

/*--------------------------------------------------------------------------*/

tstring SC_GetThirdSupplyFile(unsigned long sc_no);
/* Gets the third supply file of signal controller <sc_no>. This feature  */
/* is not supported in the generic interface. It needs a special handling */
/* in VISSIM.                                                             */
/* If controller no. <sc_no> does not exist in the VISSIM network,        */
/* a runtime error occurs and the return value is an empty string.        */

/*==========================================================================*/
/*========================= Signal group functions =========================*/
/*==========================================================================*/

#define SG_STATE_UNDEFINED            0
#define SG_STATE_GREEN                1
#define SG_STATE_RED                  2
#define SG_STATE_OFF                  4
#define SG_STATE_RED_AMBER            7
#define SG_STATE_AMBER                8
#define SG_STATE_AMBER_FLASHING       9
#define SG_STATE_RED_FLASHING        10
#define SG_STATE_GREEN_FLASHING      11
#define SG_STATE_RED_GREEN_FLASHING  12
#define SG_STATE_GREEN_AMBER         13

/*--------------------------------------------------------------------------*/

sg_iterator SignalGroups (void);

/* Returns an iterator that iterates over all signal groups.         */

/*--------------------------------------------------------------------------*/

int SGIdFromName (unsigned long sc_no, const tstring & name);

/* Gets the id of the signal group with the given name.                    */
/* If controller no. <sc_no> or signal group name <name> does not exist in */
/* the VISSIM network a runtime error occurs and the return value is -1.   */

/*--------------------------------------------------------------------------*/

int  SG_Defined (unsigned long sc_no, unsigned long sg_no);

/* If controller no. <sc_no> doesn't exist in VISSIM a runtime error  */
/* occurs (return value 0).                                           */
/* Otherwise the return value is 1 if signal group No. <sg_no> exists */
/* in controller no. <sc_no> in VISSIM, otherwise 0.                  */

/*--------------------------------------------------------------------------*/

tstring SG_Name(unsigned long sc_no, unsigned long det_no);

/* Gets the name of the signal group.                                   */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and an empty string is */
/* returned).                                                           */

/*--------------------------------------------------------------------------*/

int  SG_SetState (unsigned long sc_no, unsigned long sg_no, int state, 
                  int transition);

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

/*--------------------------------------------------------------------------*/

double SG_GreenEnd (unsigned long sc_no, unsigned long sg_no);

/* Get the current green end of signal group no. <sg_no> of controller  */
/* no. <sc_no> to <green_end>.                                          */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

double SG_RedEnd (unsigned long sc_no, unsigned long sg_no);

/* Get the current red end of signal group no. <sg_no> of controller    */
/* no. <sc_no> to <red_end>.                                            */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int SG_SetGreenEnd (unsigned long sc_no, unsigned long sg_no, double green_end);

/* Set the current green end of signal group no. <sg_no> of controller  */
/* no. <sc_no> to <green_end>.                                          */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int SG_SetRedEnd (unsigned long sc_no, unsigned long sg_no, double red_end);

/* Set the current red end of signal group no. <sg_no> of controller    */
/* no. <sc_no> to <red_end>.                                            */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

double SG_RedEnd2 (unsigned long sc_no, unsigned long sg_no);

/* Get the current second red end of signal group no. <sg_no> of        */
/* controller no. <sc_no> to <red_end>.                                 */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int SG_SetGreenEnd2 (unsigned long sc_no, unsigned long sg_no, double green_end);

/* Set the current second green end of signal group no. <sg_no> of      */
/* controller no. <sc_no> to <green_end>.                               */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int SG_SetGreenEnd2 (unsigned long sc_no, unsigned long sg_no, double green_end);
/* Set the current second green end of signal group no. <sg_no> of      */
/* controller no. <sc_no> to <green_end>.                               */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int SG_SetRedEnd2 (unsigned long sc_no, unsigned long sg_no, double red_end);
/* Set the current second red end of signal group no. <sg_no> of        */
/* controller no. <sc_no> to <red_end>.                                 */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network or a runtime error occurs and the return value */
/* is 0, else the return value is 1.                                    */

/*--------------------------------------------------------------------------*/

int  SG_CurrentState (unsigned long sc_no, unsigned long sg_no);

/* Returns the current state of signal group no. <sg_no> of controller  */
/* no. <sc_no> (see state definitions above).                           */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0).                                                               */

/*--------------------------------------------------------------------------*/

double  SG_CurrentDuration (unsigned long sc_no, unsigned long sg_no);

/* Returns the elapsed time (in seconds) since signal group no. <sg_no> */
/* of controller no. <sc_no> was set to its current state.              */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0.0).                                                             */

/*--------------------------------------------------------------------------*/

int  SG_DesiredState (unsigned long sc_no, unsigned long sg_no);

/* Returns the last desired state set by controller no. <sc_no> for     */
/* signal group no. <sg_no> (see state definitions above).              */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0).                                                               */

/*--------------------------------------------------------------------------*/

double  SG_MinimumRed (unsigned long sc_no, unsigned long sg_no);

/* Returns the minimum red time of signal group no. <sg_no> of controller */
/* no. <sc_no> (in seconds, as defined in VISSIM).                        */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist   */
/* in the VISSIM network a runtime error occurs (and the return value     */
/* is 0.0).                                                               */

/*--------------------------------------------------------------------------*/

double  SG_MinimumGreen (unsigned long sc_no, unsigned long sg_no);

/* Returns the minimum green time of signal group no. <sg_no> of        */
/* controller no. <sc_no> (in seconds, as defined in VISSIM).           */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0.0).                                                             */

/*--------------------------------------------------------------------------*/

double  SG_AmberPeriod (unsigned long sc_no, unsigned long sg_no);

/* Returns the amber time of signal group no. <sg_no> of controller     */
/* no. <sc_no> (in seconds, as defined in VISSIM).                      */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0.0).                                                             */

/*--------------------------------------------------------------------------*/

double  SG_RedAmberPeriod (unsigned long sc_no, unsigned long sg_no);

/* Returns the red/amber time of signal group no. <sg_no> of controller */
/* no. <sc_no> (in seconds, as defined in VISSIM).                      */
/* If controller no. <sc_no> or signal group no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return value   */
/* is 0.0).                                                             */

/*==========================================================================*/
/*=========================== Detector functions ===========================*/
/*==========================================================================*/

#define DET_DETECTOR_TYPE_TRAFFIC     0
#define DET_DETECTOR_TYPE_PEDESTRIAN  1
#define DET_DETECTOR_TYPE_UNDEFINED  -1

/*--------------------------------------------------------------------------*/

detector_iterator Detectors (void);

/* Returns an iterator that iterates over all detectors. */

/*--------------------------------------------------------------------------*/

int  Det_Defined (unsigned long sc_no, unsigned long det_no);

/* If controller no. <sc_no> doesn't exist in VISSIM a runtime error */
/* occurs (return value -1).                                         */
/* Otherwise the return value is 1 if detector No. <det_no> exists   */
/* in controller no. <sc_no> in VISSIM, otherwise 0.                 */

/*--------------------------------------------------------------------------*/

tstring Det_Name (unsigned long sc_no, unsigned long det_no);

/* Gets the name of the detector.                                   */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and an emptry      */
/* string is returned).                                             */

/*--------------------------------------------------------------------------*/

int  Det_Detection (unsigned long sc_no, unsigned long det_no);

/* Returns 1 if there is or was a vehicle on detector <det_no> of   */
/* controller no. <sc_no> since the previous controller time step,  */
/* else 0.                                                          */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist */
/* in the VISSIM network a runtime error occurs (and the return     */
/* value is -1).                                                    */

/*--------------------------------------------------------------------------*/

int  Det_Presence (unsigned long sc_no, unsigned long det_no);

/* Returns 1 if there is a vehicle on detector <det_no> of controller  */
/* no. <sc_no> at the end of the current simulation time step, else 0. */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist    */
/* in the VISSIM network a runtime error occurs (and the return        */
/* value is -1).                                                       */

/*--------------------------------------------------------------------------*/

int  Det_FrontEnds (unsigned long sc_no, unsigned long det_no);

/* Returns the number of detected vehicle front ends on detector <det_no> */
/* of controller no. <sc_no> since the previous controller time step.     */
/* (If a vehicle moves onto the detector while another one is still       */
/* there, no new front end will be detected!)                             */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

int  Det_RearEnds (unsigned long sc_no, unsigned long det_no);

/* Returns the number of detected vehicle rear ends on detector <det_no>  */
/* of controller no. <sc_no> since the previous controller time step.     */
/* (If a vehicle leaves the detector while another one is already on the  */
/* detector, no new rear end will be detected!)                           */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_FrontEndTime (unsigned long sc_no, unsigned long det_no, int k);

/* Returns the time in s between the start of the current controller time */
/* step and the detection of the <k>-th vehicle front end during this     */
/* controller time step. [1 <= k <= Det_FrontEnds (sc_no, det_no)]        */
/* (If a vehicle moves onto the detector while another one is still       */
/* there, no new front end will be detected!)                             */
/* If <k> is smaller than 1 or higher than the value returned by          */
/* Det_FrontEnds (<sc_no>, <det_no>), the return value is -1.             */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_RearEndTime (unsigned long sc_no, unsigned long det_no, int k);

/* Returns the time in s between the start of the current controller time */
/* step and the detection of the <k>-th vehicle rear end during this      */
/* controller time step. [1 <= k <= Det_RearEnds (sc_no, det_no)]         */
/* (If a vehicle leaves the detector while another one is already on the  */
/* detector, no new rear end will be detected!)                           */
/* If <k> is smaller than 1 or higher than the value returned by          */
/* Det_RearEnds (<sc_no>, <det_no>), the return value is -1.              */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_OccupancyTime (unsigned long sc_no, unsigned long det_no);

/* Returns the current occupancy time on detector <det_no> of controller  */
/* no. <sc_no>: 0.0 if no vehicle is present at the end of the current    */
/* simulation time step, else the time elapsed since its arrival in s.    */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double Det_OccupancyRate (unsigned long sc_no, unsigned long det_no);

/* Returns the current occupancy rate of detector <det_no> of controller  */
/* no. <sc_no> in % [0.0..100.0], i.e. the occupancy percentage since the */
/* last controller time step.                                             */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double Det_OccupancyRateSmoothed (unsigned long sc_no, unsigned long det_no);

/* Returns the exponentially smoothed occupancy rate of detector <det_no> */
/* of controller no. <sc_no> in % [0.0..100.0], i.e. the occupancy        */
/* percentage since the last controller time step, smoothed exponentially */
/* with the smoothing factors entered for this detector in VISSIM.        */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_GapTime (unsigned long sc_no, unsigned long det_no);

/* Returns the current gap time on detector <det_no> of controller no.    */
/* <sc_no>: 0.0 if a vehicle is present at the end of the current         */
/* simulation time step, else the time elapsed since the last vehicle has */
/* left the detector in s.                                                */
/* If controller no. <sc_no> or detector no. <sg_no> does not exist in    */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_VehSpeed (unsigned long sc_no, unsigned long det_no);

/* Returns the current vehicle speed on detector <det_no> of controller   */
/* no. <sc_no>: 0.0 if no vehicle was detected since the last controller  */
/* time step, else the speed of the last vehicle detected in m/s.         */
/* If controller no. <sc_no> or detector no. <det_no> does not exist in   */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*--------------------------------------------------------------------------*/

double  Det_VehLength (unsigned long sc_no, unsigned long det_no);

/* Returns the current vehicle length on detector <det_no> of controller  */
/* no. <sc_no>: 0.0 if no vehicle was detected since the last controller  */
/* time step, else the length of the last vehicle detected in m.          */
/* If controller no. <sc_no> or detector no. <det_no> does not exist in   */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

int DetIdFromName(unsigned long sc_no, const tstring & name);

/* Gets the id of the detector with the given name.                       */
/* If controller no. <sc_no> or detector name. <name> does not exist in   */
/* the VISSIM network a runtime error occurs and the return value is -1.  */


tstring DetNameFromId(unsigned long sc_no, unsigned long det_no);

/* Gets the name of the detector with the given id.                       */
/* If controller no. <sc_no> or detector no. <det_no> does not exist in   */
/* the VISSIM network a runtime error occurs and the return value is -1.  */


short Det_Type (unsigned long sc_no, unsigned long det_no);

/* Returns the detector type. With use of type it is possible to differ   */
/* between detectors for pedestrians and detectors for the traffic.       */
/* If controller no. <sc_no> or detector no. <det_no> does not exist in   */
/* the VISSIM network a runtime error occurs and the return value is -1.  */

/*==========================================================================*/
/*========================= Calling point functions ========================*/
/*==========================================================================*/

int  CP_Telegrams (void);

/* Returns the number of public transport telegrams received */
/* by calling points in the current controller time step.    */

/*--------------------------------------------------------------------------*/

long  CP_Tele_CP (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the calling point number from the      */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Line (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the line number from the               */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Course (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the course number from the             */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Route (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the route number from the              */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Prio (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the priority value from the            */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Length (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the tram length value from the         */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_ManualDC (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the manual direction code from the     */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Delay (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the delay (in s) from the              */
/* public transport telegram number <index> [1..n].                     */

/*--------------------------------------------------------------------------*/

long  CP_Tele_Passengers (unsigned index);

/* If <index> is larger than the number of public transport telegrams   */
/* received in the current simulation time step the return value is -1. */
/* Otherwise the return value is the number of passengers from the      */
/* public transport telegram number <index> [1..n].                     */

/*==========================================================================*/
/*=========================== Network functions ============================*/
/*==========================================================================*/

int  Net_SetRouteFlow (long dec_no, long route_no, long rel_flow);

/* Sets the relative flow of route <route_no> of routing */
/* decision <dec_no> to the value <rel_flow>.            */
/* If that doesn't work, a runtime error occurs (return  */
/* value 0). Otherwise the return value is 1.            */

/*--------------------------------------------------------------------------*/

int  Net_SetDesSpeedDec (long dec_no, long veh_cl, long speed_dist);

/* Assigns the desired speed distribution <speed_dist>  */
/* to the vehicle class <veh_cl> in the desired speed   */
/* decision <dec_no>.                                   */
/* If that doesn't work, a runtime error occurs (return */
/* value 0). Otherwise the return value is 1.           */

/*--------------------------------------------------------------------------*/

int  Net_SetStopTimeDist (long stop, long veh_cl, long speed_dist);

/* Assigns the desired speed distribution <speed_dist>    */
/* to the vehicle class <veh_cl> in the stop sign <stop>. */
/* If that doesn't work, a runtime error occurs (return   */
/* value 0). Otherwise the return value is 1.             */

/*==========================================================================*/
/*============================ Error functions =============================*/
/*==========================================================================*/

void  ErrFileMessage (TCHAR *message);

/* Writes the text <message> to the current *.err file. */
/* The filename is the SC DLL filename plus ".err".     */
/* The *.err file will be opened if necessary, and at   */
/* the end of the simulation run a dialog box will tell */
/* the user that an *.err file has been created.        */

/*--------------------------------------------------------------------------*/

void  ScreenMessage (TCHAR *message);

/* Displays the text <message> in a dialog box on the screen. */
/* The user has to click "OK" to continue.                    */

/*--------------------------------------------------------------------------*/

bool IsConsoleMode (void);

/* Returns whether VISSIM is started in a non interactive     */
/* version.                                                   */

/*--------------------------------------------------------------------------*/

bool IsSimulationCanceled (void);

/* Is true, if the simulation was canceled.                   */

/*--------------------------------------------------------------------------*/

int ProcessSimRunID (void);

/* Returns the code sent from VISSIM for process ID and simulation run */
/* index:  100 * process ID + run index.                               */
/* (process ID = 0 except in the parallel VISSIM version.)             */
/* If the return value is not NO_PROCESS_ID it should be included in   */
/* output file names to prevent conflicts with other processes and     */ 
/* overwriting of files created in earlier simulation runs.            */

/*--------------------------------------------------------------------------*/

void StopSimulation (void);

/* Stops the simulation. This function should be called if an error */
/* occurred in the controller that prevent from continuing the      */
/* simulation.                                                      */

/*==========================================================================*/

#endif /* __SC_DLL_FUNCTIONS_H */

/*==========================================================================*/
/*  End of SC_DLL_FUNCTIONS.H                                               */
/*==========================================================================*/
