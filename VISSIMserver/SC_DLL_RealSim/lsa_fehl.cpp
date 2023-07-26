/*==========================================================================*/
/*  LSA_FEHL.CPP                                     Teil von ????????/SIM  */
/*                                                                          */
/*  Dieses Modul nimmt von allen anderen Modulen Fehlermeldungen entgegen   */
/*  und sorgt fuer ihre Ausgabe.                                            */
/*                                                                          */
/*  Version von 2004-05-11                                  Lukas Kautzsch  */
/*==========================================================================*/

#include "lsapuffr.h"
#include "lsa_rahm.h"  /* Gemeinsamer Header fuer XYZ_RAHM.C */
#include "lsa_fehl.h"
#include "lsa_puff.h"
#include "lsa_rahm.rh"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#pragma unmanaged

#include <set>

/*--------------------------------------------------------------------------*/

FehlerStatusTyp  FehlerStatus = Okay;
TCHAR             res_string [255];
TCHAR             errorfile_name[255] = _T("");

static  bool  console_mode = FALSE;
static  int   process_id   = NO_PROCESS_ID;
static  bool is_canceled   = FALSE;

static  BOOLEAN  fehler_aufgetreten = FALSE;
static  FILE     *fehler_datei = NULL;

/*--------------------------------------------------------------------------*/

void CleanupErrorStatics()
{
  sg_fehler.clear();
  det_fehler.clear();
}

/*--------------------------------------------------------------------------*/

TCHAR  *HoleRessourcenString (int str_id, TCHAR *buffer, int buffer_size)
{
  /* Holt aus den Ressourcen des Programms den String mit der Nummer */
  /* <str_id> und schreibt ihn in den Puffer <*buffer> der Laenge    */
  /* <buffer_size>. Zurueckgegeben wird im Fehlerfall NULL, sonst    */
  /* <buffer>.                                                       */
  if (LoadString((HINSTANCE) CONTROLLER_HMODULE, str_id, buffer, buffer_size)) {
    return buffer;
  }
  else {
    return NULL;
  }
  return buffer;
} /* HoleRessourcenString */

/*--------------------------------------------------------------------------*/

static  TCHAR  *lsa_string (int nr)
{
  /* Gibt einen Zeiger auf den String "lsaxy" zurueck, wobei fuer "xy"   */
  /* <nr> eingesetzt wird, ggf. mit fuehrender Null. Ist <nr> groesser   */
  /* als <MAXINSTANZ> - 1 oder kleiner als 0, erfolgt ein Fehlerabbruch. */

  static  TCHAR  t_string[255], nr_string [5];

  HoleRessourcenString (IDS_ERR_FILE_PREFIX, t_string, sizeof (t_string) - 10);
  _stprintf (nr_string, _T("%d"), nr);
  _tcscat (t_string, nr_string);
  return t_string;
} /* lsa_string */

/*--------------------------------------------------------------------------*/

static  void  fehlerabbruch (TCHAR *string)
{
  /* Gibt die Meldung <string> aus und setzt den Fehlerstatus auf Abbruch. */ 

  FehlerStatus = Abbruch;

  if (console_mode) {
    DateiMeldung (string);
  }
  else {
    BildschirmMeldung (string);
  }
} /* fehlerabbruch */

/*--------------------------------------------------------------------------*/

void  DateiMeldung(TCHAR *string)
{
  /* Schreibt die Meldung <string> in die Fehlerdatei (LSAxy.ERR) */
  /* und oeffnet letztere dafuer gegebenenfalls.                  */

  TCHAR  message[1000], message0[255], temp_errorfile_name[255];

  if (!fehler_aufgetreten) {
    fehler_aufgetreten = TRUE;
    if (_tcslen(errorfile_name) == 0) {
      // Entferne Pfad von Error-Datei
      TCHAR * controller_name = _tcsrchr(CONTROLLER_DLL_NAME, '\\');
      ++controller_name;

      _tcscpy_s(errorfile_name, 255, controller_name);
      _tcscat_s(errorfile_name, 255, _T(".err"));

      if (process_id != NO_PROCESS_ID) {
        _stprintf(errorfile_name, _T("%s_%d"), errorfile_name, process_id);
      }
    }

    if (_tfopen_s(&fehler_datei, errorfile_name, _T("w")) != 0) {
      // Wenn der string bereits so lang ist, ist das nicht der erste Versuch
      if (_tcslen(string) > 800)
      {
        FehlerStatus = Abbruch;
        return;
      }
      fehler_aufgetreten = FALSE;
      HoleRessourcenString(IDS_OPEN_ERR_FILE, res_string, sizeof (res_string));
      _stprintf_s(message, 1000, res_string, errorfile_name, string);
      // Die Fehlersatei ist möglicherweise von einem anderen Programm gelockt.
      // Vor dem nächsten Versuch "_" vor den Dateinamen hängen und Fehlerstatus merken
      _tcscpy_s(temp_errorfile_name, 255, errorfile_name);
      _stprintf_s(errorfile_name, 255, _T("_%s"), temp_errorfile_name);
      bool fehlerStatusWarOkay = FehlerStatus == Okay;
      fehlerabbruch(message);
      // nicht abbrechen, wenn das Problem mit der Fehlerdatei behoben werden konnte
      if (fehlerStatusWarOkay)
      {
        FehlerStatus = Okay;
      }
      return;
    }
  }

  HoleRessourcenString(IDS_LSA, res_string, sizeof (res_string));
  _stprintf_s(message0, 255, _T("%s %lu\n"), res_string, LSA_Puffer == nullptr ? 0 : LSA_Puffer->Number);
  if (_ftprintf_s(fehler_datei, message0) <= 0) {
    // Wenn der string bereits so lang ist, ist das nicht der erste Versuch
    if (_tcslen(string) > 800)
    {
      FehlerStatus = Abbruch;
      return;
    }
    HoleRessourcenString(IDS_WRITE_ERR_FILE, res_string, sizeof(res_string));
    _stprintf_s(message, 1000, res_string, errorfile_name, message0);
    fehlerabbruch(message);
    return;
  }

  if (_ftprintf_s(fehler_datei, _T("%s"), string) <= 0) {
    // Wenn der string bereits so lang ist, ist das nicht der erste Versuch
    if (_tcslen(string) > 800)
    {
      FehlerStatus = Abbruch;
      return;
    }
    HoleRessourcenString(IDS_WRITE_ERR_FILE, res_string, sizeof(res_string));
    _stprintf_s(message, 1000, res_string, errorfile_name, string);
    fehlerabbruch(message);
    return;
  }
}

/*--------------------------------------------------------------------------*/

static HWND parentWindow_ = NULL;

static BOOL CALLBACK ShowAllWindows(HWND hwnd,LPARAM lParam)
{
  DWORD currentProcessId = lParam;

  if(GetWindow(hwnd, GW_OWNER))
    return true;

  DWORD windowProcessId;
  GetWindowThreadProcessId(hwnd, &windowProcessId);
  if (windowProcessId != currentProcessId)
    return true;

  parentWindow_ = hwnd;
  return false;
}



static HWND GetParentWindow()
{
  DWORD processId = GetCurrentProcessId();

  if (parentWindow_ == NULL) {
    EnumWindows((WNDENUMPROC)ShowAllWindows, processId);
  }
  return parentWindow_;
}



void  BildschirmMeldung (TCHAR *string)
{
  /* Gibt die Meldung <string> aus: Unter Windows in einer Messagebox, */
  /* unter UNIX nach stderr.                                           */

  if (console_mode) {
    DateiMeldung(string);
  }
  else {
    MessageBox(GetParentWindow(), (LPTSTR) string, (LPTSTR) CONTROLLER_DLL_NAME,
                MB_ICONSTOP | MB_OK);
  }
} /* BildschirmMeldung */

/*--------------------------------------------------------------------------*/

void  AllgemeinerFehler(const TCHAR *absender, const TCHAR * grund)
{
  /* Bildschirmmeldung "Allgemeiner Fehler" mit "Funktion: <absender>" */
  /* und "Grund: <grund>"; <FehlerStatus> wird auf Abbruch gesetzt.    */

  TCHAR  err_txt[200], fe_text [30], fn_text [20], gr_text [20];

  HoleRessourcenString (IDS_ALLGEMEINER_FEHLER, fe_text,    sizeof (fe_text));
  HoleRessourcenString (IDS_FUNKTION,           fn_text,    sizeof (fn_text));
  HoleRessourcenString (IDS_GRUND,              gr_text,    sizeof (gr_text));
  _stprintf(err_txt, _T("%s!\n%s: %s\n%s:    %s\n"), fe_text, fn_text, absender,
                                                gr_text, grund);
  fehlerabbruch (err_txt);
}  /* AllgemeinerFehler */

/*--------------------------------------------------------------------------*/

void  InternerFehler(const TCHAR *absender, const TCHAR *grund)
{
  /* Bildschirmmeldung "Interner Fehler" mit "Funktion: <absender>" */
  /* und "Grund: <grund>"; <FehlerStatus> wird auf Abbruch gesetzt. */

  TCHAR  err_txt[500], fe_text[30], fn_text[20], gr_text[20];

  HoleRessourcenString (IDS_INTERNER_FEHLER, fe_text,    sizeof (fe_text));
  HoleRessourcenString (IDS_FUNKTION,        fn_text,    sizeof (fn_text));
  HoleRessourcenString (IDS_GRUND,           gr_text,    sizeof (gr_text));
  _stprintf(err_txt, _T("%s!\n%s: %s\n%s:    %s\n"), fe_text, fn_text, absender,
                                                gr_text, grund);
  fehlerabbruch (err_txt);
}  /* InternerFehler */

/*--------------------------------------------------------------------------*/

void  Laufzeitfehler(int fehlernr, TCHAR const * prozedurname, unsigned nr)
{
  /* Dateimeldung "Laufzeitfehler" mit <prozedurname> und Klartext fuer die */
  /* Fehlernummer <fehlernr> sowie ggf. einer Zusatzinformation (<nr>).     */

  TCHAR   err_txt1 [1000], err_txt2 [500];
  sc_el  curr_sc_el;

  HoleRessourcenString (IDS_FUNKTION, res_string, sizeof (res_string));
  _stprintf (err_txt1, _T("%s %s: \n"), res_string, prozedurname);
  switch (fehlernr) {
    case SGPNR_UNBEKANNT   :
      curr_sc_el.sc = LSA_Puffer->Number;
      curr_sc_el.el = nr;
      if (sg_fehler.find(curr_sc_el) == sg_fehler.end()) {
        sg_fehler.insert(curr_sc_el);
      }
      else {
        return;
      }
      HoleRessourcenString (IDS_SGPNR_UNBEKANNT, res_string, sizeof (res_string));
      _stprintf (err_txt2, res_string, (unsigned long) nr);
      break;

    case DETNR_UNBEKANNT   :
      curr_sc_el.sc = LSA_Puffer->Number;
      curr_sc_el.el = nr;
      if (det_fehler.find(curr_sc_el) == det_fehler.end()) {
        det_fehler.insert(curr_sc_el);
      }
      else {
        return;
      }
      HoleRessourcenString (IDS_DETNR_UNBEKANNT, res_string, sizeof (res_string));
      _stprintf(err_txt2, res_string, (unsigned long) nr);
      break;

    default                :
      HoleRessourcenString (IDS_UNBEKANNTE_FEHLERNR, res_string,
                            sizeof (res_string));
      InternerFehler (_T("LSA_FEHL.Laufzeitfehler"), res_string);
      break;
  }
  _tcscat (err_txt1, err_txt2);
  _tcscat (err_txt1, _T("\n"));
  DateiMeldung (err_txt1);
}  /* Laufzeitfehler */

/*--------------------------------------------------------------------------*/

void  ExitLSAFehler (void)
{
  /* Schliesst die Fehlerdatei des Steuerungsprogramms, falls sie geoeffnet */
  /* war, und gibt in diesem Fall auch eine Meldungsbox aus.                */

  if (fehler_aufgetreten) {
    if (fehler_datei != NULL) {
      fclose (fehler_datei);
      fehler_datei = NULL;
    }
    fehler_aufgetreten = FALSE;
    //sg_fehler.clear();
    //det_fehler.clear();    
  }
} /* ExitLSAFehler */

/*--------------------------------------------------------------------------*/

void SetConsoleMode (bool consoleMode)
{
  console_mode = consoleMode;
}

/*--------------------------------------------------------------------------*/

bool IstConsoleModus (void) 
{
  return console_mode;
}

/*--------------------------------------------------------------------------*/

bool IstSimulationAbgebrochen (void)
{
  return is_canceled;
}

/*--------------------------------------------------------------------------*/

void SetProcessId(int processId)
{
  process_id = processId;
}

/*--------------------------------------------------------------------------*/

int GetProcessId(void)
{
  return process_id;
}

/*--------------------------------------------------------------------------*/

void SetSimulationCanceled(bool isCanceled)
{
  is_canceled = isCanceled;
}

/*==========================================================================*/
/*  Ende von LSA_FEHL.CPP                                                   */
/*==========================================================================*/
