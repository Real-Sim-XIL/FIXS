/*==========================================================================*/
/*  LSA_RAHM.CPP                                     Teil von ????????/SIM  */
/*                                                                          */
/*  Rahmenprogramm des Steuerungsverfahrens ????????.                       */
/*  (Vorlage fuer neue Verfahren (xyz_RAHM.CPP))                            */
/*                                                                          */
/*  Version von 2005-01-26                                  Lukas Kautzsch  */
/*==========================================================================*/

#include "lsa_fehl.h"
#include "lsa_puff.h"
#include "lsa_rahm.h"   /* Gemeinsamer Header fuer XYZ_RAHM.C */
#include "lsa_rahm.rh"
#include "sc_dll_main.h"

#pragma unmanaged

/*--------------------------------------------------------------------------*/

void  *NoRefDummyVar;

/*--------------------------------------------------------------------------*/

void  SteuerungDateiLesen (void)
{
  /* Liest die Versorgungsdatei(en) des Steuerungsprogramms ein. */
  /* Wird einmal nach dem Start des Steuerungsprogramms          */
  /* aufgerufen, und zwar vor SteuerungInitialisierung(),        */
  /* ausserdem ggf. spaeter bei expliziter Aufforderung          */
  /* "Neuversorgung" in VISSIM durch den Anwender.               */

  SC_DLL_ReadDataFiles (LSA_Puffer->Number, LSA_Puffer->DataFileName[0].c_str(),
                        LSA_Puffer->DataFileName[1].c_str());
} /* SteuerungDateiLesen */

/*--------------------------------------------------------------------------*/

void  SteuerungInitialisierung (void)
{
  /* Initialisiert das Steuerungsverfahren.                    */
  /* Wird genau einmal aufgerufen, und zwar nach dem Start des */
  /* Steuerungsprogramms nach SteuerungDateiLesen().           */

  SC_DLL_Init (LSA_Puffer->Number);
} /* SteuerungInitialisierung */

/*--------------------------------------------------------------------------*/

void  SteuerungBerechnung (void)
{
  /* Fuehrt einen Durchlauf des Steuerungsverfahrens aus. */
  /* Wird einmal pro Zeitschritt (Sekunde) von VISSIM     */
  /* aufgerufen.                                          */
  SC_DLL_Calculate(LSA_Puffer->Number);
} /* SteuerungBerechnung */

/*--------------------------------------------------------------------------*/

void  Aufraeumen (void)
{
  /* Gibt eine Meldung aus, falls eine Fehlerdatei geschrieben wurde, */
  /* und schliesst diese dann auch, beendet die Kommunikation         */
  /* zu VISSIM und gibt dynamisch belegten Speicher wieder frei.      */
  /* Wird vor Beendigung des Steuerungsprogramms aufgerufen.          */

  SC_DLL_Cleanup (LSA_Puffer->Number);
} /* Aufraeumen */

/*--------------------------------------------------------------------------*/

void  ZustandSchreiben (TCHAR *dateiname)
{
  /* Schreibt die internen Daten der aktuellen LSA in die Datei */
  /* <dateiname> im aktuellen Verzeichnis.                      */

  SC_DLL_WriteSnapshotFile (LSA_Puffer->Number, dateiname);
} /* ZustandSchreiben */

/*--------------------------------------------------------------------------*/

void  ZustandLesen(TCHAR *dateiname)
{
  /* Liest die internen Daten der aktuellen LSA aus der Datei */
  /* <dateiname> im aktuellen Verzeichnis.                    */

  SC_DLL_ReadSnapshotFile (LSA_Puffer->Number, dateiname);
} /* ZustandLesen */

/*==========================================================================*/
/*  Ende von LSA_RAHM.CPP                                                   */
/*==========================================================================*/
