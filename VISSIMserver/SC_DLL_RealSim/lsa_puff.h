/*==========================================================================*/
/*  LSA_PUFF.H                                       Teil von ????????/SIM  */
/*                                                                          */
/*  Zugriffsfunktionen auf den LSA-Puffer fuer das Steuerungsprogramm.      */
/*                                                                          */
/*  Version von 2005-11-18                                  Lukas Kautzsch  */
/*==========================================================================*/

#ifndef  __LSA_PUFF_H
#define  __LSA_PUFF_H

#include "lsa_rahm.h"  /* Gemeinsamer Header fuer XYZ_RAHM.C */

#include <stdio.h>
#include <map>
#include <string>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

/*==========================================================================*/

#define  MAX_DATA_FILES  3

#define  MAXDET    999   /* maximale interne Detektornummer      */
                         /* maximum internal detector number     */
#define  MAXSIGGR  999   /* maximale interne Signalgruppennummer */
                         /* maximum internal signal group number */

#define  MAX_KANTEN_PRO_SI  10  /* Maximalzahl Vorder/Hinterkanten         */
                                /* pro Schaltintervall auf einem Detektor  */

/*--------------------------------------------------------------------------*/

struct T_Det 
{
  long            VISSIM_Nr;                /* Detektornummer in VISSIM         */
  tstring     Name;                     /* Name des Detektors               */
  BOOLEAN         Anwesenheit;              /* am Ende dieses Zeitschritts      */
  unsigned short  Impulsspeicher;           /* falls Neuanmeldung, 1, sonst 0   */
  BOOLEAN         ImpulsspeicherLoeschen;   /* wird von Steuerung gesetzt       */
  unsigned short  Vorderkanten;             /* Anzahl seit letztem Durchlauf    */
  unsigned long   VorderkantenSumme;        /* Anzahl seit Loeschung            */
  unsigned short  Hinterkanten;             /* Anzahl seit letztem Durchlauf    */
  unsigned long   HinterkantenSumme;        /* Anzahl seit Loeschung            */
  unsigned long   angefangeneZeitluecke;    /* aktuell aufgelaufen [1/100 s]    */
  unsigned short  abgeschlosseneZeitluecke; /* seit letztem Durchlauf [1/100 s] */
  unsigned short  aktuelleBelegung;         /* seit letztem Durchlauf [%]       */
  unsigned short  geglaetteteBelegung;      /* seit letztem Durchlauf [%]       */
  unsigned long   Belegungsdauer;           /* angefangene Belegung [1/100 s]   */
  unsigned long   v;                        /* Geschwindigkeit [1/100 m/s]      */
  unsigned long   l;                        /* Fahrzeuglaenge [mm]              */
  DetectorType    Type;                     /* Typ des Detektors                */
  short           tVorderkante [MAX_KANTEN_PRO_SI];
  short           tHinterkante [MAX_KANTEN_PRO_SI];
                  /* 0..99, relative Zeitpunkte innerhalb des Schaltintervalls, */
                  /* [1/100 Schaltintervall], Inhalt -1 = keine Kante           */
};
typedef  T_Det  *P_Det;

struct T_SigGr 
{
  long        VISSIM_Nr;   /* Signalgruppennummer in VISSIM   */
  tstring Name;        /* Name der Signalgruppe           */
  SignalBild  Bild;        /* bei Solidstates und Schutzbl.:  */
  SignalBild  Soll;        /*  an = gruen, aus = rot!         */
  SignalBild  AltSoll;     /* nach letztem Logik-Durchlauf    */
  BOOLEAN     Uebergang;   /* soll einer geschaltet werden?   */
  double      TGruenMin;   /* Mindestgruenzeit [s]            */
  double      TRotMin;     /* Mindestrotzeit   [s]            */
  double      TRotgelb;    /* Rotgelbzeit      [s]            */
  double      TGelb;       /* Gelbzeit         [s]            */
  double      Laufzeit;    /* des aktuellen Signalbilds [s]   */
  double      TRotende;    /* aktuelles Rotende   [s]         */
  double      TGruenende;  /* aktuelles Gruenende [s]         */
  double      TRotende2;   /* aktuelles Rotende 2   [s]       */
  double      TGruenende2; /* aktuelles Gruenende 2 [s]       */
};
typedef  T_SigGr  *P_SigGr;

typedef  struct sT_ProtTripel  *P_ProtTripel;
typedef  struct sT_ProtTripel 
{
  long          Code;
  long          Nummer;
  long          Wert;
  P_ProtTripel  next;
 } T_ProtTripel;

struct SIM_OEV_TELE
{
  long      mp_nr;
  unsigned  linien_nr;
  unsigned  kurs_nr;
  unsigned  routen_nr;
  unsigned  prioritaet;
  unsigned  zuglaenge;
  unsigned  richtung_von_hand;
  int       fahrpl_sek;
  unsigned  personen;
};
typedef  SIM_OEV_TELE  *OEV_Telegramm;

typedef  struct T_TelegrammListe  *TelegrammListe;
struct T_TelegrammListe {
                          OEV_Telegramm   inhalt;
                          TelegrammListe  next;
                        };

struct T_Eingang
{
  long  nummer;
  long  wert;
} ;
typedef  T_Eingang  *Eingang;

typedef  struct T_EingangListe  *EingangListe;
struct T_EingangListe 
{
  Eingang       inhalt;
  EingangListe  next;
};



typedef  std::map<tstring, T_SigGr *> NameToSigGrMap;
typedef  std::map<unsigned long, T_SigGr *> NumberToSigGrMap;

typedef  std::map<tstring, T_Det *> NameToDetMap;
typedef  std::map<unsigned long, T_Det *> NumberToDetMap;


enum OptimizationLevel
{
  OptimizationLevelNoSignalGroupData = 0x1,
  OptimizationLevelNoDetectorData = 0x2,
  OptimizationLevelNoSimTimeOfDay = 0x4,
  OptimizationLevelDefault = 0x0
};



typedef  struct T_LSA_Puffer  *P_LSA_Puffer;

struct T_LSA_Puffer {
  unsigned long    Number;
  tstring          DataFileName[MAX_DATA_FILES];
  double           Sekunde;    /* Umlaufsekunde [1..TU] [s]   */
  double           TU;         /* [s]                         */
  unsigned         ProgNr;     /* Nr. des laufenden Programms */
  BOOLEAN          DebugModus; /* Debugger aktiv?             */
  T_Det            Det [MAXDET+1];
  unsigned         num_Det;
  NameToDetMap     DetNameToObj;
  NumberToDetMap   DetNoToObj;
  T_SigGr          SigGr [MAXSIGGR+1];
  unsigned         num_SG;
  NameToSigGrMap   SgNameToObj;
  NumberToSigGrMap SgNoToObj;
  P_ProtTripel     ProtTripel;    /* Liste der Protokoll-Tripel  */
  unsigned         AnzProtTripel;
  P_ProtTripel     *pNaechstesTripel;
  EingangListe     AlleEingaenge;
  EingangListe     AlleAusgaenge;
  unsigned         Schaltfrequenz;  /* Logikdurchläufe pro Sekunde */
  unsigned         Index;
  unsigned         OptimizationLevel;
  P_LSA_Puffer     next;
};

/*--------------------------------------------------------------------------*/

extern  P_LSA_Puffer  LSA_Puffer;
extern  P_LSA_Puffer  Alle_LSA_Puffer;
extern  double        SimulationTime;                 /* [s] */
extern  TCHAR         SimulationTimeOfDay [11];       /* "hh:mm:ss.s" */
extern  double        SimulationSecondsSinceMidnight; /* [s] */
extern  long          SimulationDate;                 /* YYYYMMDD */
extern  tstring       SnapshotFilename;         /* VISSIM *.snp */

/*==========================================================================*/

BOOLEAN  InitLSAPuffer (long sc_no);

/* Belegt den Speicher fuer den LSA_Puffer der LSA Nr. <sc_no>.     */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE und ein         */
/* Laufzeitfehler erzeugt.                                          */
/*                                                                  */
/* Allocates memory for the buffer <LSA_Puffer> for SC No. <sc_no>. */
/* Return value: TRUE in case of success, otherwise FALSE (and      */
/* runtime error).                                                  */

/*--------------------------------------------------------------------------*/

bool  SetLSAPuffer (long sc_no);

/* Sets the global variable <LSA_Puffer> to the list element of */
/* SC No. <sc_no>. Returns true on success, else false (after a */
/* runtime error message).                                      */

/*--------------------------------------------------------------------------*/

void  InitDataFromSIM (void);

/* Initializes dynamic data sent from VISSIM for the next */
/* controller time step.                                  */

/*--------------------------------------------------------------------------*/

void  InitDataToSIM (void);

/* Initializes dynamic data sent to VISSIM for the next */
/* controller time step.                                */

/*--------------------------------------------------------------------------*/

void  ExitLSAPuffer (long sc_no);

/* Gibt den Speicher fuer den LSA_Puffer der LSA Nr. <sc_no> wieder frei. */
/*                                                                        */
/* Frees the allocated memory for the <LSA_Puffer> of SC No. <sc_no>.     */

/*--------------------------------------------------------------------------*/

T_Det * det_nr_to_object(long vissim_nr, TCHAR const * function_name, bool create);

/* Gibt einen Zeiger auf den Detektor mit dem VISSIM-Detektorkanal    */
/* <vissim_nr> in der aktuellen LSA (gesetzt durch SetLSAPuffer())    */
/* zurueck.                                                           */
/* Falls kein Detektor mit dieser Kanalnummer definiert ist und       */
/* <create> true ist, wird er neu angelegt und ein Zeiger auf ihn     */
/* zurückgegeben.                                                     */
/* Falls kein Detektor mit dieser Kanalnummer definiert ist und       */
/* <create> false ist, wird nullptr zurückgegeben. Außerdem wird,     */
/* falls <function_name> nicht NULL ist, eine Fehlermeldung inclusive */
/* <function_name> auf dem Bildschirm angezeigt.                      */
/*                                                                    */
/* Returns a pointer to the detector with VISSIM's detector channel   */
/* number <vissim_nr> in the current SC (set by SetLSAPuffer()).      */
/* If no such detector exists and if <create> is true, the detector   */
/* is added and a pointer to the detector is returned. (If the        */
/* maximum number of detectors is exceeded, an error message is       */
/* displayed instead and the return value is nullptr.)                */
/* If no such detector exists and if <create> is false, the return    */
/* value is nullptr, and if <function_name> is not nullptr, an error  */
/* message including <function_name> is displayed.                    */

/*--------------------------------------------------------------------------*/

T_Det * det_name_to_object(const tstring & vissim_name, TCHAR* function_name);

/* Gibt einen Zeiger auf den Detektor mit dem Namen <vissim_name> zurück, */
/* falls er existiert, sonst nullptr.                                     */
/*                                                                        */
/* Returns a pointer to the detector with the name <vissim_name> if that  */
/* detector exists, else returns nullptr.                                 */

/*--------------------------------------------------------------------------*/

T_SigGr * sg_name_to_object(const tstring & vissim_name, TCHAR* function_name);

/* Gibt einen Zeiger auf die Signalgruppe mit dem Namen <vissim_name>    */
/* zurück, falls sie existiert, sonst nullptr.                           */
/*                                                                       */
/* Returns a pointer to the signal group with the name <vissim_name> if  */
/* that signal group exists, else returns nullptr.                       */

/*--------------------------------------------------------------------------*/

T_SigGr *  sg_nr_to_object(long vissim_nr, TCHAR const * function_name, bool create);

/* Gibt einen Zeiger auf die Signalgruppe mit der VISSIM-Nummer       */
/* <vissim_nr> in der aktuellen LSA (gesetzt durch SetLSAPuffer())    */
/* zurueck.                                                           */
/* Falls keine Signalgruppe mit dieser Nummer definiert ist und       */
/* <create> true ist, wird sie neu angelegt und ein Zeiger auf sie    */
/* zurückgegeben.                                                     */
/* Falls keine Signalgruppe mit dieser Nummer definiert ist und       */
/* <create> false ist, wird nullptr zurückgegeben. Außerdem wird,     */
/* falls <function_name> nicht NULL ist, eine Fehlermeldung inclusive */
/* <function_name> auf dem Bildschirm angezeigt.                      */
/*                                                                    */
/* Returns a pointer to the signal group with the number <vissim_nr>  */
/* in VISSIM in the current SC (set by SetLSAPuffer()).               */
/* If no such signal group exists and if <create> is true, the signal */
/* group is added and a pointer to it is returned. (If the maximum    */
/* number of signal groups is exceeded, an error message is           */
/* displayed instead and the return value is nullptr.)                */
/* If no such signal group exists and if <create> is false, the       */
/* return value is nullptr, and if <function_name> is not nullptr, an */
/* error message including <function_name> is displayed.              */

/*--------------------------------------------------------------------------*/

SignalBild lsa_bild(long bild);

/*--------------------------------------------------------------------------*/

long  puffer_bild (SignalBild bild);

/* Gibt den Signalbild-Code des LSA-Puffers fuer das Signalbild <bild> */
/* zurueck.                                                            */

/*--------------------------------------------------------------------------*/

int  DoubleToLSAPuffer (long typ, long nummer, double wert);

/* Writes the value <wert> of type <typ>, index <nummer> to <LSA_Puffer>. */

/*--------------------------------------------------------------------------*/

int  LongToLSAPuffer (long typ, long nummer, long wert);
/* Writes the value <wert> of type <typ>, index <nummer> to <LSA_Puffer>. */

/*--------------------------------------------------------------------------*/

int StringToLSAPuffer (long typ, long nummer, tstring& wert);

/* Writes the value <wert> of type <typ>, index <nummer> to <LSA_Puffer>. */

/*==========================================================================*/

void  VonLSA (int kommando, int nr, BOOLEAN uebergang);

/* Gibt je nach <kommando> einen Schaltbefehl an die Signalgruppe <nr>: */
/*  1: Signalgruppe gruen                                               */
/*  2: Signalgruppe rot                                                 */
/*  3: Solidstate ein                                                   */
/*  4: Solidstate oder Signalgruppe aus                                 */
/*  5: Schutzblinker ein                                                */
/*  6: Schutzblinker aus                                                */
/*  7: Signalgruppe rotgelb                                             */
/*  8: Signalgruppe gelb                                                */
/*  9: Signalgruppe gelb blinkend                                       */
/* 10: Signalgruppe rot blinkend                                        */
/* 11: Signalgruppe gruen blinkend                                      */
/* 12: Signalgruppe rot-gruen wechselblinkend                           */
/* 13: Signalgruppe gruengelb                                           */
/* Falls <uebergang> gesetzt ist, soll beim Umschalten ggf. ein         */
/* Uebergangssignal geschaltet werden.                                  */
/* Falls die Signalgruppe <nr> im VISSIM-Netz nicht definiert ist oder  */
/* <kommando> nicht im zulaessigen Bereich liegt, erfolgt eine          */
/* Laufzeitfehlermeldung.                                               */
/*                                                                      */
/* Sends new signal state to signal group No. <nr>, depending on the    */
/* code No. <kommando>:                                                 */
/*  1: signal group green                                               */
/*  2: signal group red                                                 */
/*  3: solid state signal on (green)                                    */
/*  4: solid state signal or signal group off (black)                   */
/*  5: flashing signal on (amber)                                       */
/*  6: flashing signal off (black)                                      */
/*  7: signal group red/amber                                           */
/*  8: signal group amber                                               */
/*  9: signal group amber flashing                                      */
/* 10: signal group red flashing                                        */
/* 11: signal group green flashing                                      */
/* 12: signal group red-green flashing                                  */
/* 13: signal group green/amber                                         */
/* If <uebergang> == TRUE, a transition state will be requested.        */
/* If signal group No. <nr> does not exist in the VISSIM network or     */
/* if the code No. <kommando> is not in [1..8], a runtime error occurs. */

/*--------------------------------------------------------------------------*/

unsigned short  NachLSA (int kommando, int nr);

/* Gibt je nach <kommando> einen Wert der Signalgruppe <nr> bzw. LSA */
/* an das Steuerungsprogramm zurueck:                                */
/*  1: Mindestrotzeit                                                */
/*  2: Vorbereitungszeit (Rotgelbzeit) (wie in VISSIM versorgt)      */
/*  3: Mindestgruenzeit                                              */
/*  4: Gelbzeit                        (wie in VISSIM versorgt)      */
/*  5: Bild == Rot oder Gelb?        (falls ja: 1, sonst 0)          */
/*  6: Bild == Rotgelb?              (falls ja: 1, sonst 0)          */
/*  7: Bild == Gruen oder Rotgelb?   (falls ja: 1, sonst 0)          */
/*  8: Bild == Gelb?                 (falls ja: 1, sonst 0)          */
/*  9: Mindestrotzeit abgelaufen?    (falls ja: 0, sonst Laufzeit)   */
/* 10: Mindestgruenzeit abgelaufen?  (falls ja: 0, sonst Laufzeit)   */
/* 11: aktuelle Rotdauer (incl. Gelb!)                               */
/* 12: aktuelle Vorbereitungsdauer (Rotgelbdauer)                    */
/* 13: aktuelle Gruendauer (incl. Rotgelb!)                          */
/* 14: aktuelle Gelbdauer                                            */
/* 15: aktuelle Sekunde im Umlauf                                    */
/* 16: Umlaufzeit                                                    */
/* 17: Soll == Rot?                  (falls ja: 1, sonst 0)          */
/* 18: Soll == Gruen?                (falls ja: 1, sonst 0)          */
/* 19: bisherige Dauer des aktuellen Signalbilds                     */
/* Falls die Signalgruppe im VISSIM-Netz nicht definiert ist,        */
/* <nr> < 0, (<nr> == 0 und (<kommando> < 15 oder <kommando> > 16)), */
/* <nr> > MAXSIGGR ist oder <kommando> ausserhalb des zulaessigen    */
/* Bereichs liegt, erfolgt eine Laufzeitfehlermeldung (Rueckgabewert */
/* 0). Alle Zeiten werden in ganzen Sekunden angegeben.              */
/*                                                                   */
/* Returns a status value of signal group No. <nr> (respectively     */
/* the whole SCJ) to the signal control, depending on the code No.   */
/* <kommando>:                                                       */
/*  1: minimum red time                                              */
/*  2: red/amber time (fixed value as entered in VISSIM)             */
/*  3: minimum green time                                            */
/*  4: amber time     (fixed value as entered in VISSIM)             */
/*  5: is state red or amber?        (yes: 1, otherwise: 0)          */
/*  6: is state red/amber?           (yes: 1, otherwise: 0)          */
/*  7: is state green or red/amber?  (yes: 1, otherwise: 0)          */
/*  8: is state amber?               (yes: 1, otherwise: 0)          */
/*  9: is minimum red time over?     (yes: 0, otherwise red time)    */
/* 10: is minimum green time over?   (yes: 0, otherwise green time)  */
/* 11: current red time (including amber time!)                      */
/* 12: current red/amber time                                        */
/* 13: current green time (including red/amber time!)                */
/* 14: current amber time                                            */
/* 15: current cycle second                                          */
/* 16: cycle time                                                    */
/* 17: new state red?                (yes: 1, otherwise: 0)          */
/* 18: new state green?              (yes: 1, otherwise: 0)          */
/* 19: running time of current state                                 */
/* If <nr> < 0, (<nr> == 0 and (<kommando> < 15 or <kommando> >16)), */
/* <nr> > MAXSIGGR, <kommando> is not in [1..18] or the signal group */
/* does not exist in the VISSIM network, a runtime error occurs      */
/* (return value 0). All times are measured in seconds.              */

/*--------------------------------------------------------------------------*/

double  NachLSA_Real (int kommando, int nr);
/* See NachLSA for a detailed description. The only difference is */
/* the return type of double.                                     */

/*--------------------------------------------------------------------------*/

float  Simulationssekunde (void);

/* Gibt die aktuelle Simulationssekunde zurueck. */
/*                                               */
/* Returns the current simulation second.        */

/*--------------------------------------------------------------------------*/

float  Umlaufsekunde (void);

/* Gibt die aktuelle Umlaufsekunde zurueck. */
/*                                          */
/* Returns the current cycle second.        */

/*--------------------------------------------------------------------------*/

int  ProgrammNummer (void);

/* Gibt die Nummer des laufenden Programms zurueck (Default = 1). */
/*                                                                */
/* Returns number of the running signal program (default = 1).    */

/*--------------------------------------------------------------------------*/

int  sg_definiert (int nr);

/* Falls die Signalgruppe Nr. <nr> in VISSIM definiert ist, wird 1  */
/* zurückgegeben, andernfalls 0.                                    */
/*                                                                  */
/* The return value is 1 if signal group No. <nr> exists in VISSIM, */
/* otherwise 0.                                                     */

/*--------------------------------------------------------------------------*/

tstring sg_name (int nr);

/* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                     */
/* Ansonsten wird der Name der Signalgruppe zurückgegeben.                      */
/*                                                                              */
/* If signal group No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value -1).                                              */
/* Otherwise the name of the signal group will be returned.                     */

/*--------------------------------------------------------------------------*/

int  det_definiert (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM definiert ist, wird 1 */
/* zurückgegeben, andernfalls 0.                               */
/*                                                             */
/* The return value is 1 if detector No. <nr> exists in the    */
/* VISSIM network, otherwise 0.                                */

/*--------------------------------------------------------------------------*/

int  d_imp (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird der Wert des Impulsspeichers des Detektors Nr. <nr>       */
/* zurueckgegeben: 1, wenn sich der Zustand des Detektors von unbelegt auf  */
/* belegt geändert hat (wenn also eine Fahrzeugvorderkante erfasst wurde,   */
/* während nicht schon ein anderes Fahrzeug auf dem Detektor war), sonst 0. */
/* Der Impulsspeicher wird von VISSIM nicht automatisch geloescht - siehe   */
/* Funktion d_limp() weiter unten!                                          */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value 0).                                           */
/* Otherwise the return value is 1, if detector No. <nr> detected a vehicle */
/* front end (with no other vehicle still on the detector), otherwise 0.    */
/* This detection is not cleared automatically by VISSIM - see function     */
/* d_limp() below!                                                          */

/*--------------------------------------------------------------------------*/

int  d_limp (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert -1).                     */
/* Ansonsten wird beim Detektor Nr. <nr> vermerkt, dass dessen        */
/* Impulsspeicher am Ende des Zeitschritts auf 0 gesetzt werden soll, */
/* und 0 zurueckgegeben.                                              */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value -1).                          */
/* Otherwise a flag is set at detector No. <nr> to clear the detected */
/* vehicle front end at the end of the simulation time step (return   */
/* value 0).                                                          */

/*--------------------------------------------------------------------------*/

int  d_blgzt (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird 0 zurueckgegeben, falls sich zum Ende des Zeitschritts    */
/* kein Fahrzeug auf dem Detektor Nr. <nr> befindet, andernfalls die Zeit   */
/* seit seinem Eintreffen in 1/10 s.                                        */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value 0).                                           */
/* Otherwise the return value is the current occupancy time of detector No. */
/* <nr>: 0, if the detector detects no vehicle present at the end of the    */
/* simulation time step, otherwise the time since its arrival in 1/10 s.    */

/*--------------------------------------------------------------------------*/

int  d_belgg (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird der Wert des exponentiell geglaetteten Belegungsgrads     */
/* in % des Detektors Nr. <nr> zurueckgegeben (Glaettungsfaktoren gemaess   */
/* Versorgung dieses Detektors in VISSIM).                                  */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value 0).                                           */
/* Otherwise the return value is the exponentially smoothed occupancy rate  */
/* of detector No. <nr> in %, i.e. the occupancy percentage since the last  */
/* pass through the signal control logic, smoothed exponentially with the   */
/* smoothing factors entered for this detector in VISSIM.                   */

/*--------------------------------------------------------------------------*/

long  d_ztlkn (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt    */
/* eine Fehlermeldung (Rueckgabewert LONG_MAX = 2147483647L).            */
/* Ansonsten wird die angefangene Nettozeitluecke beim Detektor Nr. <nr> */
/* in 1/100 s zurueckgegeben (aber maximal LONG_MAX).                    */
/*                                                                       */
/* If detector No. <nr> does not exist in the VISSIM network,            */
/* a runtime error occurs (return value LONG_MAX = 2147483647L).         */
/* Otherwise the return value is the current running time headway (rear  */
/* to front) (maximum value: LONG_MAX), respectively 0, if detector No.  */
/* <nr> detects a vehicle present at the end of the current simulation   */
/* time step.                                                            */

/*--------------------------------------------------------------------------*/

long  d_v (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird 0 zurueckgegeben, falls seit dem letzten Durchlauf durch  */
/* die Steuerungslogik kein Fahrzeug vom Detektor Nr. <nr> erfasst wurde,   */
/* andernfalls die aktuelle Geschwindigkeit des letzten erfassten Fahrzeugs */
/* in 1/100 m/s.                                                            */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value 0).                                           */
/* Otherwise the return value is the vehicle speed detected by detector No. */
/* <nr>: 0, if the detector has detected no vehicle since the last pass     */
/* through the signal control, otherwise the speed of the last detected     */
/* vehicle in 1/100 m/s.                                                    */

/*--------------------------------------------------------------------------*/

long  d_l (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird 0 zurueckgegeben, falls seit dem letzten Durchlauf durch  */
/* die Steuerungslogik kein Fahrzeug vom Detektor Nr. <nr> erfasst wurde,   */
/* andernfalls die Laenge des letzten erfassten Fahrzeugs in mm.            */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value -1).                                          */
/* Otherwise the return value is the vehicle length detected by detector    */
/* No. <nr>: 0, if the detector has detected no vehicle since the last pass */
/* through the signal control, otherwise the length of the last detected    */
/* vehicle in mm (1/1000 m).                                                */

/*--------------------------------------------------------------------------*/

DetectorType d_typ(int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird der Typ des Detektors zurückgegeben.                      */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value -1).                                          */
/* Otherwise the type of the detector will be returned.                     */

/*--------------------------------------------------------------------------*/

tstring d_name(int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
/* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
/* Ansonsten wird der Name des Detektors zurückgegeben.                      */
/*                                                                          */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime     */
/* error occurs (return value -1).                                          */
/* Otherwise the name of the detector will be returned.                     */

/*--------------------------------------------------------------------------*/
unsigned char  anwesenheit (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt    */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                         */
/* Ansonsten wird 1 zurueckgegeben, falls sich zum Ende des aktuellen    */
/* Zeitschritts ein Fahrzeug im Erfassungsbereich des Detektors Nr. <nr> */
/* befindet, andernfalls 0.                                              */
/*                                                                       */
/* If detector No. <nr> does not exist in the VISSIM network, a runtime  */
/* error occurs (return value 0).                                        */
/* Otherwise the return value is 1, if detector No. <nr> detects a       */
/* vehicle present at the end of the current simulation time step,       */
/* otherwise 0.                                                          */

/*--------------------------------------------------------------------------*/

unsigned char  zaehlwert (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der seit dem letzten Durchlauf durch die */
/* Steuerungslogik erfassten Fahrzeugvorderkanten zurueckgegeben      */
/* (wobei einander ueberlappende Impulse zu einem zusammengefasst     */
/* werden).                                                           */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the return value is the number of detected vehicle front */
/* ends since the last pass through the signal control. (If a vehicle */
/* moves onto the detector while another one is still there, no new   */
/* front end will be detected!)                                       */

/*--------------------------------------------------------------------------*/

unsigned char  hinterkanten (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der seit dem letzten Durchlauf durch     */
/* die Steuerungslogik erfassten Fahrzeughinterkanten zurueckgegeben  */
/* (wobei einander ueberlappende Impulse zu einem zusammengefasst     */
/* werden).                                                           */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the return value is the number of detected vehicle rear  */
/* ends since the last pass through the signal control.               */
/* (If a vehicle leaves the detector while another one is already on  */
/* the detector, no new rear end will be detected!)                   */

/*--------------------------------------------------------------------------*/

unsigned long  vorderkantensumme (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der seit der letzten Loeschung erfassten */
/* Fahrzeugvorderkanten zurueckgegeben (wobei einander ueberlappende  */
/* Impulse zu einem zusammengefasst werden).                          */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the return value is the number of detected vehicle front */
/* ends since it was last cleared by loesche_vorderkantesumme().      */
/* (If a vehicle moves onto the detector while another one is still   */
/* there, no new front end will be detected!)                         */

/*--------------------------------------------------------------------------*/

void  loesche_vorderkantensumme (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der erfassten Fahrzeugvorderkanten       */
/* geloescht.                                                         */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the number of detected vehicle front ends (for           */
/* vorderkantesumme()) is reset to zero.                              */

/*--------------------------------------------------------------------------*/

unsigned long  hinterkantensumme (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der seit der letzten Loeschung erfassten */
/* Fahrzeughinterkanten zurueckgegeben (wobei einander ueberlappende  */
/* Impulse zu einem zusammengefasst werden).                          */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the return value is the number of detected vehicle rear  */
/* ends since it was last cleared by loesche_hinterkantesumme().      */
/* (If a vehicle leaves the detector while another one is already on  */
/* the detector, no new rear end will be detected!)                   */

/*--------------------------------------------------------------------------*/

void  loesche_hinterkantensumme (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird die Anzahl der erfassten Fahrzeughinterkanten       */
/* geloescht.                                                         */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the number of detected vehicle rear ends (for            */
/* vorderkantesumme()) is reset to zero.                              */

/*--------------------------------------------------------------------------*/

unsigned char  belegung (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
/* Ansonsten wird der (nicht geglaettete) Belegungsgrad des           */
/* Detektors Nr. <nr> seit dem letzten Durchlauf durch die            */
/* Steuerungslogik zurueckgegeben (in %).                             */
/*                                                                    */
/* If detector No. <nr> does not exist in the VISSIM network,         */
/* a runtime error occurs (return value 0).                           */
/* Otherwise the return value is the occupancy rate of detector No.   */
/* <nr> in %, i.e. the occupancy percentage since the last pass       */
/* through the signal control (without exponential smoothing).        */

/*--------------------------------------------------------------------------*/

unsigned char  abgeschl_luecke (int nr);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt  */
/* eine Laufzeitfehlermeldung (Rueckgabewert 0).                       */
/* Ansonsten wird der 0 zurueckgegeben, falls seit dem letzten         */
/* Durchlauf durch die Steuerungslogik keine Zeitluecke begonnen UND   */
/* beendet wurde, andernfalls die Dauer der letzten solchen Zeitluecke */
/* in 1/100 s.                                                         */
/*                                                                     */
/* If detector No. <nr> does not exist in the VISSIM network,          */
/* a runtime error occurs (return value 0).                            */
/* Otherwise the return value is 0, if there was no time headway       */
/* (rear to front) started AND completed since the last pass           */
/* through the signal control, otherwise its duration in 1/100 s.      */

/*--------------------------------------------------------------------------*/

int  d_on (int nr, int intervall_nr, int intervalle_pro_schaltint);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,        */
/* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).            */
/* Ansonsten wird 1 zurueckgegeben, falls im Intervall Nummer        */
/* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint> */
/* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf    */
/* durch die Steuerungslogik eine Fahrzeugvorderkante erfasst wurde, */
/* sonst 0.                                                          */
/*                                                                   */
/* If detector No. <nr> does not exist in the VISSIM network,        */
/* a runtime error occurs (return value -1).                         */
/* Otherwise the return value is 1, if there was a vehicle front end */
/* detected in interval number <interval_nr> (counting from 0) of    */
/* <intervalle_pro_schaltint> ([1..100]) equal sized intervals since */
/* the last pass through the signal control, otherwise 0.            */

/*--------------------------------------------------------------------------*/

int  d_off (int nr, int intervall_nr, int intervalle_pro_schaltint);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,        */
/* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).            */
/* Ansonsten wird 1 zurueckgegeben, falls im Intervall Nummer        */
/* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint> */
/* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf    */
/* durch die Steuerungslogik eine Fahrzeughinterkante erfasst wurde, */
/* sonst 0.                                                          */
/*                                                                   */
/* If detector No. <nr> does not exist in the VISSIM network,        */
/* a runtime error occurs (return value -1).                         */
/* Otherwise the return value is 1, if there was a vehicle rear end  */
/* detected in interval number <interval_nr> (counting from 0) of    */
/* <intervalle_pro_schaltint> ([1..100]) equal sized intervals since */
/* the last pass through the signal control, otherwise 0.            */

/*--------------------------------------------------------------------------*/

int  d_presence (int nr, int intervall_nr, int intervalle_pro_schaltint);

/* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,           */
/* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).               */
/* Ansonsten wird 1 zurueckgegeben, falls am Ende des Intervalls Nummer */
/* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint>    */
/* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf       */
/* durch die Steuerungslogik ein Fahrzeug über dem Detektor war,        */
/* sonst 0.                                                             */
/*                                                                      */
/* If detector No. <nr> does not exist in the VISSIM network,           */
/* a runtime error occurs (return value -1).                            */
/* Otherwise the return value is 1, if there was a vehicle present      */
/* at the end of interval number <interval_nr> (counting from 0) of     */
/* <intervalle_pro_schaltint> ([1..100]) equal sized intervals since    */
/* the last pass through the signal control, otherwise 0.               */

/*--------------------------------------------------------------------------*/

void  SetzeUmlaufsekunde (float uml_sek);

/* Setzt die aktuelle Umlaufsekunde auf <uml_sek>.                   */
/* (Dieser Wert wird am Ende des Zeitschritts an VISSIM uebergeben,  */
/* auf ein volles Schaltintervall (Kehrwert der Schaltfrequenz)      */
/* genau.)                                                           */
/*                                                                   */
/* Sets the current cyle second to <uml_sek>.                        */
/* (This value is transferred to VISSIM at the end of the simulation */
/* second, with a precision of a full signal control period          */
/* (reciprocal of the controller frequency).)                        */

/*--------------------------------------------------------------------------*/

void  SetzeProgrammnummer (unsigned prg_nr);

/* Setzt die aktuelle Programmnummer auf <prg_nr>.                   */
/* (Dieser Wert wird am Ende des Zeitschritts an VISSIM uebergeben.) */
/*                                                                   */
/* Sets the current program number to <prg_nr>.                      */
/* (This value is transferred to VISSIM at the end of the simulation */
/* time step.)                                                       */

/*--------------------------------------------------------------------------*/

BOOLEAN  SchreibeProtTripel (long code, long nummer, long wert);

/* Schreibt ein neues Protokoll-Tripel in den LSA-Puffer.    */
/* <code>, <nummer> und <wert> sollten in den Wertebereichen */
/* liegen, die in der WTT-Datei des Steuerungsverfahrens     */
/* definiert sind. Insbesondere sollte <code> > 9999 sein.   */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE          */
/* und ein Laufzeitfehler erzeugt.                           */
/*                                                           */
/* Writes a new protocol triplet to the buffer <LSA_Puffer>, */
/* consisting of a code <code>, a number <nummer> and a      */
/* value <wert> as defined in the WTT file. The value ranges */
/* for the parameters are defined there, too.                */
/* If the triplet cannot be written, a runtime error occurs  */
/* (return value FALSE). Otherwise the return value is TRUE. */

/*--------------------------------------------------------------------------*/

int  Anzahl_OEV_Tele (void);

/* Gibt die Anzahl der im aktuellen Zeitschritt empfangenen */
/* OEV-Telegramme zurueck.                                  */
/*                                                          */
/* Returns the number of public transport telegrams         */
/* received in the current simulation time step.            */

/*--------------------------------------------------------------------------*/

long  OEV_Tele_MP (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
/* Meldepunkt-Nummer aus dem Telegramm Nummer <index> (erstes: 1).        */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the calling point number from the        */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Linie (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
/* Liniennummer aus dem Telegramm Nummer <index> (erstes: 1).             */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the line number from the public          */
/* transport telegram number <index>.                                     */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Kurs (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
/* Kursnummer aus dem Telegramm Nummer <index> (erstes: 1).               */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the course number from the               */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Route (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
/* Routennummer aus dem Telegramm Nummer <index> (erstes: 1).             */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the route number from the                */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Prio (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls der    */
/* Prioritaetswert aus dem Telegramm Nummer <index> (erstes: 1).          */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the priority value from the              */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Zuglaenge (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
/* Zuglaenge aus dem Telegramm Nummer <index> (erstes: 1).                */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the tram length value from the           */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_RVH (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls der    */
/* Code "Richtung von Hand" aus dem Telegramm Nummer <index> (erstes: 1). */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is -1.   */
/* Otherwise the return value is the manual direction code from the       */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Fahrplanlage (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird 0 zurueckgegeben, andernfalls die     */
/* Fahrplanlage [s] aus dem Telegramm Nummer <index> (erstes: 1).         */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is 0.    */
/* Otherwise the return value is the delay [s] from the                   */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Personen (unsigned index);

/* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
/* empfangenen OEV-Telegramme, wird 0 zurueckgegeben, andernfalls die     */
/* Fahrgastanzahl aus dem Telegramm Nummer <index> (erstes: 1).           */
/*                                                                        */
/* If <index> is larger than the number of public transport telegrams     */
/* received in the current simulation time step the return value is 0.    */
/* Otherwise the return value is the number of passengers from the        */
/* public transport telegram number <index>.                              */

/*--------------------------------------------------------------------------*/

long  LiesEingang (long nummer);

/* Gibt den am LSA-Kopplungs-Eingang Nummer <nummer> zuletzt eingegangen */
/* Wert zurueck, bzw. -1, falls dieser Eingang nicht verkabelt ist.      */
/*                                                                       */
/* If the input channel <nummer> is not active (undefined in VISSIM),    */
/* the return value is -1. Otherwise the return value is the value sent  */
/* from the controller output channel connected to that input channel.   */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeAusgang (long nummer, long wert);

/* Setzt dem LSA-Kopplungs-Ausgang Nummer <nummer> auf <wert>. */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE            */
/* und ein Laufzeitfehler erzeugt.                             */
/*                                                             */
/* Sets the output channel <nummer> to the value <wert>.       */
/* If that doesn't work, a runtime error occurs                */
/* (return value FALSE). Otherwise the return value is TRUE.   */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeRoute (long rout_ent, long route, long rel_bel);

/* Setzt die relative Belastung der Route <route> der   */
/* Routenentscheidung <rout_ent> auf <rel_bel>.         */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE     */
/* und ein Laufzeitfehler erzeugt.                      */
/*                                                      */
/* Sets the relative flow of route <route> of routing   */
/* decision <rout_ent> to the value <rel_bel>.          */
/* If that doesn't work, a runtime error occurs (return */
/* value FALSE). Otherwise the return value is TRUE.    */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeVWunsch (long vwunsch_ent, long fz_kl, long vwunsch_vert);

/* Ordnet in der Wunschgeschwindigkeitsentscheidung      */
/* <vwunsch_ent> der Fahrzeugklasse <fz_kl> die          */
/* Wunschgeschwindigkeitsverteilung <vwunsch_vert> zu.   */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE      */
/* und ein Laufzeitfehler erzeugt.                       */
/*                                                       */
/* Assigns the desired speed distribution <vwunsch_vert> */
/* to the vehicle class <fz_kl> in the desired speed     */
/* decision <vwunsch_vert>.                              */
/* If that doesn't work, a runtime error occurs (return  */
/* value FALSE). Otherwise the return value is TRUE.     */

/*--------------------------------------------------------------------------*/

BOOLEAN  LiesDebugModus (void);

/* Gibt den von VISSIM uebergebenen Debug-Modus zurueck - */
/* wenn nichts uebergeben wurde, FALSE.                   */
/*                                                        */
/* Returns the debug mode transmitted from VISSIM -       */
/* default (if nothing transmitted) is FALSE.             */

/*--------------------------------------------------------------------------*/

SignalBild  SigGrBild (int nr);

/* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist,    */
/* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert <undefiniert>). */
/* Ansonsten wird das aktuelle Signalbild der Signalgruppe Nummer    */
/* <nr> zurueckgegeben.                                              */
/*                                                                   */
/* If signal group No. <nr> does not exist in the VISSIM network,    */
/* a runtime error occurs (return value <undefiniert> = undefined).  */
/* Otherwise the return value is the current state of signal         */
/* group No. <nr> (as set by the control logic).                     */
/*                                                                   */
/* SignalBild is defined in LSA_RAHM.H:                              */
/* typedef enum { rot,            = red                              */
/*                rotgelb,        = red/amber                        */
/*                gruen,          = green                            */
/*                gelb,           = amber                            */
/*                aus,            = off                              */
/*                undefiniert,    = undefined                        */
/*                gelb_blink,     = amber flashing                   */
/*                rot_blink,      = red flashing                     */
/*                gruen_blink,    = green flashing                   */
/*                rgwb,           = red/green alternating            */
/*                gruengelb,      = green/amber                      */
/*                aus_rot         = off meaning red                  */
/*              }  SignalBild;                                       */

/*--------------------------------------------------------------------------*/

SignalBild  SigGrSoll (int nr);

/* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist,    */
/* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert <undefiniert>). */
/* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist,    */
/* erfolgt auch eine Meldung (Rueckgabewert <undefiniert>).          */
/* Ansonsten wird das letzte von der Steuerung gesetzte Sollbild     */
/* der Signalgruppe Nummer <nr> zurueckgegeben.                      */
/*                                                                   */
/* If signal group No. <nr> does not exist in the VISSIM network,    */
/* a runtime error occurs (return value <undefiniert> = undefined).  */
/* Otherwise the return value is the last desired state of signal    */
/* group No. <nr> (as set by the control logic).                     */
/*                                                                   */
/* SignalBild is defined in LSA_RAHM.H:                              */
/* typedef enum { rot,            = red                              */
/*                rotgelb,        = red/amber                        */
/*                gruen,          = green                            */
/*                gelb,           = amber                            */
/*                aus,            = off                              */
/*                undefiniert,    = undefined                        */
/*                gelb_blink,     = amber flashing                   */
/*                rot_blink,      = red flashing                     */
/*                gruen_blink,    = green flashing                   */
/*                rgwb,           = red/green alternating            */
/*                gruengelb,      = green/amber                      */
/*                aus_rot         = off meaning red                  */
/*              }  SignalBild;                                       */

/*--------------------------------------------------------------------------*/

void  SetzeSchaltfrequenz (unsigned freq);

/* Setzt die Schaltfrequenz der LSA auf <freq> [1..10] Durchlaeufe pro   */
/* Sekunde.                                                              */
/* Dieser Wert wird nach der Initialisierung der Steuerung an VISSIM     */
/* uebergeben. Die Funktion muss also schon waehrend der Initialisierung */
/* aufgerufen werden, wenn die Schaltfrequenz hoeher als der             */
/* Standardwert 1 ist.                                                   */
/*                                                                       */
/* Sets the controller frequency to <freq> passes per second [1..10].    */
/* The value is passed to VISSIM after the initialization of the control */
/* program, so this function must be called during that initialization   */
/* if the frequency is to be higher than the default value 1.            */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetStopTimeDist (long stop, long fz_kl, long vwunsch_vert);

/* Ordnet im Stopschild <stop> der Fahrzeugklasse <fz_kl>  */
/* die Wunschgeschwindigkeitsverteilung <vwunsch_vert> zu. */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE        */
/* und ein Laufzeitfehler erzeugt.                         */
/*                                                         */
/* Assigns the desired speed distribution <vwunsch_vert>   */
/* to the vehicle class <fz_kl> in the stop sign <stop>.   */
/* If that doesn't work, a runtime error occurs (return    */
/* value FALSE). Otherwise the return value is TRUE.       */

/*==========================================================================*/

#endif /* __LSA_PUFF_H */

/*==========================================================================*/
/*  Ende von LSA_PUFF.H                                                     */
/*==========================================================================*/
