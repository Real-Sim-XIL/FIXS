/*==========================================================================*/
/*  LSA_RAHM.H                                       Teil von ????????/SIM  */
/*                                                                          */
/*  Rahmenprogramm des Steuerungsverfahrens.                                */
/*  (Gemeinsame Headerdatei fuer verschiedene Verfahren (xyz_RAHM.CPP))     */
/*                                                                          */
/*  Version von 2005-01-26                                  Lukas Kautzsch  */
/*==========================================================================*/

#ifndef __LSA_RAHM_H
#define __LSA_RAHM_H

#include <windows.h>  /* for definition of BOOLEAN */

/*==========================================================================*/

#define  MAX(a,b)  ((a) > (b) ? (a) : (b))
#define  MIN(a,b)  ((a) < (b) ? (a) : (b))

/* ### */
extern HMODULE CONTROLLER_HMODULE;
extern TCHAR CONTROLLER_DLL_NAME[MAX_PATH];

/*==========================================================================*/

/* Einige Deklarationen aus DEF.H */
typedef  unsigned int       CARDINAL;
typedef  float              REAL;
typedef  double             LONGREAL;     /* mindestens 8 Byte */
typedef  long int           LONGINT;      /* mindestens 32 bit */
typedef  unsigned long      LONGCARD;     /* mindestens 32 bit */
#define  STRNCPY(x,y,z)  strncpy ((x), (y), (z)-1); (x)[(z)-1] = '\0'
extern   void  *NoRefDummyVar;
#define  NOREF(x)  NoRefDummyVar = &(x)

/* Einige Deklarationen aus STRUKTUR.H */
typedef  enum {rot, rotgelb, gruen, gelb, aus, undefiniert, gelb_blink,
               rot_blink, gruen_blink, rgwb, gruengelb, aus_rot}  SignalBild;

typedef enum {TrafficDetector = 0, PedestrianDetector = 1, UndefinedDetector = -1} DetectorType;

/*==========================================================================*/

void  SteuerungDateiLesen (void);

/* Liest die Versorgungsdatei(en) des Steuerungsprogramms ein. */
/* Wird einmal nach dem Start des Steuerungsprogramms          */
/* aufgerufen, und zwar vor SteuerungInitialisierung(),        */
/* ausserdem ggf. spaeter bei expliziter Aufforderung          */
/* "Neuversorgung" in VISSIM durch den Anwender.               */

/*--------------------------------------------------------------------------*/

void  SteuerungInitialisierung (void);

/* Initialisiert das Steuerungsverfahren.                    */
/* Wird genau einmal aufgerufen, und zwar nach dem Start des */
/* Steuerungsprogramms nach SteuerungDateiLesen().           */

/*--------------------------------------------------------------------------*/

void  SteuerungBerechnung (void);

/* Fuehrt einen Durchlauf des Steuerungsverfahrens aus. */
/* Wird einmal pro Zeitschritt (Sekunde) von VISSIM     */
/* aufgerufen.                                          */

/*--------------------------------------------------------------------------*/

void  Aufraeumen (void);

/* Gibt eine Meldung aus, falls eine Fehlerdatei geschrieben wurde, */
/* und schliesst diese dann auch, beendet die Kommunikation         */
/* zu VISSIM und gibt dynamisch belegten Speicher wieder frei.      */
/* Wird vor Beendigung des Steuerungsprogramms aufgerufen.          */

/*--------------------------------------------------------------------------*/

void  ZustandSchreiben(TCHAR *dateiname);

/* Schreibt die internen Daten der aktuellen LSA in die Datei */
/* <dateiname> im aktuellen Verzeichnis.                      */

/*--------------------------------------------------------------------------*/

void  ZustandLesen(TCHAR *dateiname);

/* Liest die internen Daten der aktuellen LSA aus der Datei */
/* <dateiname> im aktuellen Verzeichnis.                    */

/*==========================================================================*/

#endif  /* __LSA_RAHM_H */

/*==========================================================================*/
/*  Ende von LSA_RAHM.H                                                     */
/*==========================================================================*/
