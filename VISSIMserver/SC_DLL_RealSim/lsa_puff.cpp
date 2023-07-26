/*==========================================================================*/
/*  LSA_PUFF.CPP                                     Teil von ????????/SIM  */
/*                                                                          */
/*  Zugriffsfunktionen fuer den LSA-Puffer.                                 */
/*                                                                          */
/*  Version von 2011-04-29                                  Lukas Kautzsch  */
/*==========================================================================*/

#include "assert.h"
#include "lsa_fehl.h"
#include "lsa_rahm.h"   /* Gemeinsamer Header fuer XYZ_RAHM.C */
#include "lsapuffr.h"
#include "lsa_puff.h"
#include "lsa_rahm.rh"
#include "SC_Interface.h"

#ifdef _HPUX_SOURCE
#define  far           /* das kennt   HPUX nicht! */
#endif

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <tchar.h>

#pragma unmanaged

/*==========================================================================*/

P_LSA_Puffer    LSA_Puffer = NULL;
P_LSA_Puffer    Alle_LSA_Puffer = NULL;
double          SimulationTime;          /* [s] */
TCHAR           SimulationTimeOfDay [11] = _T("hh:mm:ss.s");
double          SimulationSecondsSinceMidnight = 0.0;
long            SimulationDate = 0;      /* YYYYMMDD */
tstring         SnapshotFilename;  /* VISSIM *.snp */

static T_SigGr * lastUsedSigGr = nullptr;
static T_Det * lastUsedDet = nullptr;

/*==========================================================================*/

static  TCHAR           kein_speicher [50];
static  TelegrammListe  AlleOEVTele = NULL;

/*==========================================================================*/

static  unsigned  long_to_unsigned (long wert)
{
  /* Gibt den Wert <wert> als unsigned int zurueck, wobei negative Werte */
  /* auf 0 abgebildet werden und Werte groesser als der groesste         */
  /* darstellbare Wert auf ebendiesen.                                   */

  if (wert < 0) {
    return 0;
  }
  else if (wert > UINT_MAX) {
    return UINT_MAX;
  }
  else {
    return (unsigned) wert;
  }
} /* long_to_unsigned */

/*--------------------------------------------------------------------------*/

static  unsigned short  long_to_unsigned_short (long wert)
{
  /* Gibt den Wert <wert> als unsigned short zurueck, wobei negative Werte */
  /* auf 0 abgebildet werden und Werte groesser als der groesste           */
  /* darstellbare Wert auf ebendiesen.                                     */

  if (wert < 0) {
    return 0;
  }
  else if (wert > USHRT_MAX) {
    return USHRT_MAX;
  }
  else {
    return (unsigned short) wert;
  }
} /* long_to_unsigned_short */

/*--------------------------------------------------------------------------*/

BOOLEAN  InitLSAPuffer (long sc_no)
{
  /* Belegt den Speicher fuer den LSA_Puffer der LSA Nr. <sc_no>. */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE und ein     */
  /* Laufzeitfehler erzeugt.                                      */

  int           i;
  P_LSA_Puffer  *pplp, plp;

  HoleRessourcenString (IDS_KEIN_SPEICHER, kein_speicher,
                        sizeof (kein_speicher));

  /* Grundgeruest des LSA_Puffers anlegen: */
    plp = new T_LSA_Puffer();
    if (plp == NULL) {
      AllgemeinerFehler (_T("LSA_PUFF.InitLSAPuffer"), kein_speicher);
      return FALSE;
    }
    plp->Number = sc_no;

  /* An Liste der LSA_Puffer anhängen: */
    pplp = &Alle_LSA_Puffer;
    while (*pplp != NULL) {
      pplp = &(*pplp)->next;
    }
    *pplp = plp;
    plp->next = NULL;

  /* Speicherplatz fuer Detektoren belegen: */
    for (i = 0; i <= MAXDET; ++i) {
      plp->Det[i].Type              = TrafficDetector;
      plp->Det[i].VISSIM_Nr         = 0;
      plp->Det[i].VorderkantenSumme = 0;
      plp->Det[i].HinterkantenSumme = 0;
    }
    plp->num_Det = 0;

  /* Speicherplatz fuer Signalgruppen belegen: */
    for (i = 0; i <= MAXSIGGR; ++i) {
      plp->SigGr[i].Bild        = undefiniert;
      plp->SigGr[i].Soll        = undefiniert;
      plp->SigGr[i].AltSoll     = undefiniert;
      plp->SigGr[i].VISSIM_Nr   = 0;
      plp->SigGr[i].TGruenende  = 0;
      plp->SigGr[i].TRotende    = 0;
      plp->SigGr[i].TGruenende2 = 0;
      plp->SigGr[i].TRotende2   = 0;
    }
    plp->num_SG = 0;

  /* Anfangswerte setzen (Liste der Protokoll-Tripel ist leer!): */
    plp->Schaltfrequenz    = 1;
    plp->ProtTripel        = NULL;
    plp->AnzProtTripel     = 0;
    plp->pNaechstesTripel  = &plp->ProtTripel;
    plp->AlleEingaenge     = NULL;
    plp->AlleAusgaenge     = NULL;
    plp->ProgNr            = 1;     /* Default, falls von VISSIM nichts kommt */
    plp->DebugModus        = FALSE; /* Default, falls von VISSIM nichts kommt */
    plp->TU                = 0;
    plp->OptimizationLevel = OptimizationLevelDefault;

    AlleOEVTele = NULL;

  return TRUE;
} /* InitLSAPuffer */

/*--------------------------------------------------------------------------*/

bool  SetLSAPuffer (long sc_no)
{
  /* Sets the global variable <LSA_Puffer> to the list element of */
  /* SC No. <sc_no>. Returns true on success, else false (after a */
  /* runtime error message).                                      */
  if (LSA_Puffer != nullptr && LSA_Puffer->Number == sc_no) {
    return true;
  }

  TCHAR  message[255];

  lastUsedDet = nullptr;
  lastUsedSigGr = nullptr;

  for (P_LSA_Puffer plp = Alle_LSA_Puffer; plp != NULL; plp = plp->next) {
    if (plp->Number == sc_no) {
      LSA_Puffer = plp;
      return true;
    }
  }
  _stprintf(message, _T("SC No. %ld not found!"), sc_no);
  AllgemeinerFehler (_T("LSA_PUFF.SetLSAPuffer"), message);
  return false;
} /* SetLSAPuffer */

/*--------------------------------------------------------------------------*/

static  void  loesche_telegramm_liste (TelegrammListe *liste)
{
  /* Gibt den durch die uebergebene Telegrammliste <*liste> belegten */
  /* Speicher wieder frei und setzt <*liste> auf NULL.               */

  TelegrammListe  tl = *liste, tl2;

  while (tl != NULL) {
    tl2 = tl->next;
    delete tl->inhalt;
    delete tl;
    tl = tl2;
  }
  *liste = NULL;
} /* loesche_telegramm_liste */

/*--------------------------------------------------------------------------*/

static  void  loesche_eingang_liste (EingangListe *liste)
{
  /* Gibt den durch die uebergebene Eingangliste <*liste> belegten */
  /* Speicher wieder frei und setzt <*liste> auf NULL.             */

  EingangListe  tl = *liste, tl2;

  while (tl != NULL) {
    tl2 = tl->next;
    delete tl->inhalt;
    delete tl;
    tl = tl2;
  }
  *liste = NULL;
} /* loesche_eingang_liste */

/*--------------------------------------------------------------------------*/

void  InitDataFromSIM (void)
{
  /* Initializes dynamic data sent from VISSIM for the next */
  /* controller time step.                                  */

  unsigned  i, j;

  for (i = 0; i <= LSA_Puffer->num_Det; i++) {
    T_Det & det = LSA_Puffer->Det[i];

    for (j = 0; j < MAX_KANTEN_PRO_SI; j++) {
      det.tVorderkante[j] = -1;
      det.tHinterkante[j] = -1;
    }
  }

  lastUsedDet = nullptr;
  lastUsedSigGr = nullptr;
} /* InitDataFromSIM */

/*--------------------------------------------------------------------------*/

void  InitDataToSIM (void)
{
  /* Initializes dynamic data sent to VISSIM for the next */
  /* controller time step.                                */

  unsigned  i;

  for (i = 0; i <= LSA_Puffer->num_Det; i++) {
    LSA_Puffer->Det[i].ImpulsspeicherLoeschen = FALSE;
  }
  for (i = 0; i <= LSA_Puffer->num_SG; i++) {
    T_SigGr & sigGr = LSA_Puffer->SigGr[i];

    if (sigGr.Soll != undefiniert) {
      sigGr.AltSoll = sigGr.Soll;
    }
    sigGr.Soll = undefiniert;
  }

  LSA_Puffer->AnzProtTripel = 0;
  LSA_Puffer->pNaechstesTripel = &LSA_Puffer->ProtTripel;

  lastUsedDet = nullptr;
  lastUsedSigGr = nullptr;
} /* InitDataToSIM */

/*--------------------------------------------------------------------------*/

void  ExitLSAPuffer (long sc_no)
{
  /* Gibt den Speicher fuer den LSA_Puffer der LSA Nr. <sc_no> wieder frei. */
  P_ProtTripel  p, p2;
  P_LSA_Puffer  *pplp, n_plp;

  for (pplp = &Alle_LSA_Puffer; *pplp != NULL; pplp = &(*pplp)->next) {
    if ((*pplp)->Number == sc_no) {
      if (LSA_Puffer == *pplp) {
        LSA_Puffer = nullptr;
      }
      p = (*pplp)->ProtTripel;
      while (p != NULL) {
        p2 = p->next;
        delete (p);
        p = p2;
      }
      (*pplp)->pNaechstesTripel = NULL;
      loesche_telegramm_liste (&AlleOEVTele);
      loesche_eingang_liste (&(*pplp)->AlleEingaenge);
      loesche_eingang_liste (&(*pplp)->AlleAusgaenge);
      (*pplp)->DetNoToObj.clear();
      (*pplp)->SgNoToObj.clear();
      n_plp = (*pplp)->next;
      delete *pplp;
      *pplp = n_plp;

      lastUsedDet = nullptr;
      lastUsedSigGr = nullptr;
      break;
    }
  }
} /* ExitLSAPuffer */

/*==========================================================================*/

T_Det * det_nr_to_object(long vissim_nr, TCHAR const * function_name, bool create)
{
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
  
  if (lastUsedDet != nullptr) {
    if (lastUsedDet->VISSIM_Nr == vissim_nr) {
      return lastUsedDet;
    }

    size_t index = lastUsedDet - LSA_Puffer->Det + 1;
    if (index < LSA_Puffer->num_Det && (++lastUsedDet)->VISSIM_Nr == vissim_nr) {
      return lastUsedDet;
    }
  }

  lastUsedDet = nullptr;

  if (0 < LSA_Puffer->num_Det) {
    if (LSA_Puffer->Det[0].VISSIM_Nr != vissim_nr) {
      NumberToDetMap::iterator pos = LSA_Puffer->DetNoToObj.find(vissim_nr);
      if (pos != LSA_Puffer->DetNoToObj.end()) {
        lastUsedDet = pos->second;
      }
    }
    else {
      lastUsedDet = &LSA_Puffer->Det[0];
    }
  }

  if (lastUsedDet == nullptr) {
    if (create) {
      if (LSA_Puffer->DetNoToObj.size() < MAXDET) {
        lastUsedDet = &LSA_Puffer->Det[LSA_Puffer->num_Det++];
        lastUsedDet->VISSIM_Nr = vissim_nr;
        LSA_Puffer->DetNoToObj.insert(std::make_pair(vissim_nr, lastUsedDet));
      }
      else {
        TCHAR err_txt[200];
        HoleRessourcenString(IDS_TOO_MANY_DET, res_string, sizeof(res_string));
        _stprintf(err_txt, res_string, MAXDET);
        AllgemeinerFehler(function_name, err_txt);
        return nullptr;
      }
    }
    else {
      if (function_name != NULL) {
        Laufzeitfehler(DETNR_UNBEKANNT, function_name, vissim_nr);
      }
      return nullptr;
    }
  }

  return lastUsedDet;
} /* det_nr_to_object */

/*--------------------------------------------------------------------------*/

T_Det * det_name_to_object(const tstring & vissim_name, TCHAR *function_name)
{
  /* Gibt einen Zeiger auf den Detektor mit dem Namen <vissim_name> zurück, */
  /* falls er existiert, sonst nullptr.                                     */

  NameToDetMap::iterator iter = LSA_Puffer->DetNameToObj.find(vissim_name);

  if (iter == LSA_Puffer->DetNameToObj.end()) {
    if (function_name != NULL) {
      std::basic_stringstream<TCHAR> reason;
      reason << "No detector with name \"" << vissim_name << "\" found.";
      InternerFehler (function_name, reason.str().c_str());
    }
    return nullptr;
  }
  else {
    return iter->second;
  }
} /* det_name_to_object */

/*--------------------------------------------------------------------------*/

T_SigGr * sg_name_to_object (const tstring & vissim_name, TCHAR* function_name)
{
  /* Gibt einen Zeiger auf die Signalgruppe mit dem Namen <vissim_name>    */
  /* zurück, falls sie existiert, sonst nullptr.                           */

  NameToSigGrMap::iterator iter = LSA_Puffer->SgNameToObj.find(vissim_name);

  if (iter == LSA_Puffer->SgNameToObj.end()) {
    if (function_name != NULL) {
      std::basic_stringstream<TCHAR> reason;
      reason << _T("No signal group with name \"") << vissim_name << _T("\" found.");
      InternerFehler (function_name, reason.str().c_str());
    }

    return nullptr;
  }
  else {
    return iter->second;
  }
}

/*--------------------------------------------------------------------------*/

T_SigGr * sg_nr_to_object(long vissim_nr, TCHAR const * function_name, bool create)
{
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

  if (lastUsedSigGr != nullptr) {
    if (lastUsedSigGr->VISSIM_Nr == vissim_nr) {
      return lastUsedSigGr;
    }

    size_t index = lastUsedSigGr - LSA_Puffer->SigGr + 1;
    if (index < LSA_Puffer->num_SG && (++lastUsedSigGr)->VISSIM_Nr == vissim_nr) {
      return lastUsedSigGr;
    }

    lastUsedSigGr = nullptr;
  }
  else {
    if (0 < LSA_Puffer->num_SG) {
      if (LSA_Puffer->SigGr[0].VISSIM_Nr == vissim_nr) {
        lastUsedSigGr = &LSA_Puffer->SigGr[0];
        return lastUsedSigGr;
      }
    }
  }
  
  NumberToSigGrMap::iterator pos = LSA_Puffer->SgNoToObj.find(vissim_nr);
  if (pos != LSA_Puffer->SgNoToObj.end()) {
    lastUsedSigGr = pos->second;
  }

  if (lastUsedSigGr == nullptr) {
    if (create) {
      if (LSA_Puffer->SgNoToObj.size() < MAXSIGGR) {
        lastUsedSigGr = &LSA_Puffer->SigGr[LSA_Puffer->num_SG++];
        LSA_Puffer->SgNoToObj.insert(std::make_pair(vissim_nr, lastUsedSigGr));
        lastUsedSigGr->VISSIM_Nr = vissim_nr;
      }
      else {
        TCHAR  err_txt[200];
        HoleRessourcenString(IDS_TOO_MANY_SG, res_string, sizeof(res_string));
        _stprintf(err_txt, res_string, MAXSIGGR);
        AllgemeinerFehler(function_name, err_txt);
        return nullptr;
      }
    }
    else {
      if (function_name != NULL) {
        Laufzeitfehler(SGPNR_UNBEKANNT, function_name, vissim_nr);
      }
      return nullptr;
    }
  }

  return lastUsedSigGr;
} /* sg_nr_to_object */

/*--------------------------------------------------------------------------*/

long  puffer_bild (SignalBild bild)
{
  /* Gibt den Signalbild-Code des LSA-Puffers fuer das Signalbild <bild> */
  /* zurueck.                                                            */

  switch (bild) {
    case rot :
      return BILD_ROT;
    case rotgelb :
      return BILD_ROTGELB;
    case gruen :
      return BILD_GRUEN;
    case gelb :
      return BILD_GELB;
    case aus :
      return BILD_AUS;
    case undefiniert :
      return BILD_UNDEFINIERT;
    case gelb_blink :
      return BILD_GELB_BLINK;
    case rot_blink :
      return BILD_ROT_BLINK;
    case gruen_blink :
      return BILD_GRUEN_BLINK;
    case rgwb :
      return BILD_RGWB;
    case gruengelb :
      return BILD_GRUENGELB;
    default :
      HoleRessourcenString (IDS_UNBEKANNTES_SIGNALBILD, res_string,
        sizeof (res_string));
      InternerFehler (_T("LSA_PUFF.puffer_bild"), res_string);
      return -1L;
  }
} /* puffer_bild */

/*==========================================================================*/

SignalBild lsa_bild (long bild)
{
  /* Gibt das Signalbild zurueck, das dem Signalbildcode <bild> */
  /* des LSAPuffers entspricht.                                 */

  switch (bild) {
    case BILD_AUS     :
      return aus;
    case BILD_AN      :
      return gruen;
    case BILD_ROT     :
      return rot;
    case BILD_ROTGELB :
      return rotgelb;
    case BILD_GELB    :
      return gelb;
    case BILD_GRUEN   :
      return gruen;
    case BILD_GELB_BLINK :
      return gelb_blink;
    case BILD_ROT_BLINK :
      return rot_blink;
    case BILD_GRUEN_BLINK :
      return gruen_blink;
    case BILD_RGWB :
      return rgwb;
    case BILD_GRUENGELB :
      return gruengelb;
    default      :
      HoleRessourcenString (IDS_UNBEKANNTES_SIGNALBILD, res_string,
                            sizeof (res_string));
      InternerFehler (_T("LSA_PUFF.lsa_bild"), res_string);
      return aus;
  }
} /* lsa_bild */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static  void  neues_oev_telegramm (long nummer)
{
  /* Legt im <LSA_Puffer> ein neues, leeres OEV-Telegramm mit der */
  /* Meldepunktsnummer <nr> an.                                   */

  TCHAR            prozedurname[] = _T("LSA_PUFF.neues_oev_telegramm");
  TelegrammListe  tl;

  tl = new T_TelegrammListe();
  if (tl == NULL) {
    AllgemeinerFehler (prozedurname, kein_speicher);
    return;
  }
  tl->inhalt = new SIM_OEV_TELE();
  if (tl->inhalt == NULL) {
    AllgemeinerFehler (prozedurname, kein_speicher);
    return;
  }
  tl->next = AlleOEVTele;
  tl->inhalt->mp_nr             = nummer;
  tl->inhalt->linien_nr         = 0;
  tl->inhalt->kurs_nr           = 0;
  tl->inhalt->routen_nr         = 0;
  tl->inhalt->prioritaet        = 0;
  tl->inhalt->zuglaenge         = 0;
  tl->inhalt->richtung_von_hand = 0;
  tl->inhalt->fahrpl_sek        = 0;
  tl->inhalt->personen          = 0;
  AlleOEVTele = tl;
} /* neues_oev_telegramm */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static  BOOLEAN  mp_nummer_stimmt (long nummer)
{
  /* Gibt TRUE zurueck, falls das letzte begonnene OEV-Telegramm */
  /* vom Meldepunkt Nummer <nummer> stammt, andernfalls FALSE.   */

  return (BOOLEAN) (   AlleOEVTele                != NULL
                    && AlleOEVTele->inhalt        != NULL
                    && AlleOEVTele->inhalt->mp_nr == nummer);
} /* mp_nummer_stimmt */

int StringToLSAPuffer (long typ, long nummer, tstring& wert)
{
  switch (typ) {
    case SC_DLL_DATA_DETECTOR_NAME:
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Name = wert;

        if (!wert.empty() && LSA_Puffer->DetNameToObj.find(wert) == LSA_Puffer->DetNameToObj.end()) {
          LSA_Puffer->DetNameToObj.insert(std::make_pair(wert, det));
        }
      }
      break;
    }
    case SC_DLL_DATA_SG_NAME:
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->Name = wert;

        if (!wert.empty() && LSA_Puffer->SgNameToObj.find(wert) == LSA_Puffer->SgNameToObj.end()) {
          LSA_Puffer->SgNameToObj.insert(std::make_pair(wert, sg));
        }
      }
      break;
    }
    default:
      return 0;
  }

  return 1;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int  DoubleToLSAPuffer (long typ, long nummer, double wert)
{
  /* Writes the value <wert> of type <typ>, index <nummer> to <LSA_Puffer>. */

  switch (typ) {
    case SC_DLL_DATA_CYCLE_TIME :
      LSA_Puffer->TU = wert;
      break;

    case SC_DLL_DATA_CYCLE_SECOND :
      LSA_Puffer->Sekunde = wert;
      break;

    case SC_DLL_DATA_SIM_TIME :
      if (wert - SimulationTime > 1E-6) {
        loesche_telegramm_liste (&AlleOEVTele);
      }
      SimulationTime = wert;
      break;

    case SC_DLL_DATA_SG_T_GREEN_MIN :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TGruenMin = wert;
      }
      break;
    }

    case SC_DLL_DATA_SG_T_RED_MIN :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TRotMin = wert;
      }
      break;
    }

    case SC_DLL_DATA_SG_T_RED_AMBER :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TRotgelb = wert;
      }
      break;
    }

    case SC_DLL_DATA_SG_T_AMBER :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TGelb = wert;
      }
      break;
    }

    case SC_DLL_DATA_SG_CURR_DURATION:
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->Laufzeit = wert;
      }
      break;
    }

    default                       :
      /* unerwarteter oder unbekannter Typ => ignorieren! */
      return 0;
  } /* switch (typ) */

  return 1;
} /* DoubleToLSAPuffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int  LongToLSAPuffer (long typ, long nummer, long wert)
{
  /* Writes the value <wert> of type <typ>, index <nummer> to <LSA_Puffer>. */

  TCHAR const * const prozedurname = _T("LSA_PUFF.LongToLSAPuffer");
  unsigned      i;
#ifdef _DEMO_
  BOOLEAN       demo_version = FALSE;
#endif
  EingangListe  el;

  switch (typ) {

    case DATEN_ENDE               :
      /* ende = TRUE; */
      break;

    case UMLAUFZEIT               :
      LSA_Puffer->TU = wert;
      break;

    case UMLAUFSEKUNDE            :
      LSA_Puffer->Sekunde = (double) wert / (double) LSA_Puffer->Schaltfrequenz;
      break;

    case PROGRAMMNUMMER           :
      LSA_Puffer->ProgNr  = (unsigned) wert;
      break;

    case DEBUGMODUS               :
      LSA_Puffer->DebugModus = (BOOLEAN) (wert != 0);
      break;

    case IST_BILD                 :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->Bild = lsa_bild(wert);
      }
      break;
    }

    case T_GRUEN_MIN              :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TGruenMin = wert;
      }
      break;
    }

    case T_ROT_MIN                :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TRotMin = wert;
      }
      break;
    }

    case T_ROTGELB                :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TRotgelb = wert;
      }
      break;
    }

    case T_GELB                   :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->TGelb = wert;
      }
      break;
    }

    case LAUFZEIT                 :
    {
      T_SigGr * sg = sg_nr_to_object(nummer, NULL, true);
      if (sg != nullptr) {
        sg->Laufzeit = wert;
      }
      break;
    }

    case ANWESENHEIT              :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Anwesenheit = (BOOLEAN) (wert != 0);
      }
      break;
    }

    case IMPULSSPEICHER           :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Impulsspeicher = (BOOLEAN) (wert != 0);
      }
      break;
    }

    case DET_TYPE                 :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Type = static_cast<DetectorType>(wert);
      }
      break;
    }

    case VORDERKANTEN             :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Vorderkanten = (unsigned short) wert;
        det->VorderkantenSumme += (unsigned short) wert;
      }
      break;
    }

    case HINTERKANTEN             :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Hinterkanten = (unsigned short) wert;
        det->HinterkantenSumme += (unsigned short) wert;
      }
      break;
    }

    case T_VORDERKANTE            :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        i = 0;
        while (i < MAX_KANTEN_PRO_SI && det->tVorderkante[i] != -1) {
          i++;
        }
        if (i < MAX_KANTEN_PRO_SI) {
          det->tVorderkante[i] = (short) wert;
        }
      }
      break;
    }

    case T_HINTERKANTE            :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        i = 0;
        while (i < MAX_KANTEN_PRO_SI && det->tHinterkante[i] != -1) {
          i++;
        }
        if (i < MAX_KANTEN_PRO_SI) {
          det->tHinterkante[i] = (short) wert;
        }
      }
      break;
    }

    case ANG_ZEITLUECKE           :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->angefangeneZeitluecke = wert;
      }
      break;
    }

    case KOMPL_ZEITLUECKE         :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->abgeschlosseneZeitluecke = (unsigned short) wert;
      }
      break;
    }

    case ANG_BELEGUNG             :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->Belegungsdauer = wert;
      }
      break;
    }

    case AKT_BELEGUNG             :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->aktuelleBelegung = (unsigned short) wert;
      }
      break;
    }

    case GEGL_BELEGUNG            :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->geglaetteteBelegung = (unsigned short) wert;
      }
      break;
    }

    case GESCHWINDIGKEIT          :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->v = wert;
      }
      break;
    }

    case FZLAENGE                 :
    {
      T_Det * det = det_nr_to_object(nummer, NULL, true);
      if (det != nullptr) {
        det->l = wert;
      }
      break;
    }

    case OVT_MP                   :
      neues_oev_telegramm (nummer);
      break;

    case OVT_LINIE                :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->linien_nr = (unsigned) wert;
      }
      break;

    case OVT_KURS                 :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->kurs_nr = (unsigned) wert;
      }
      break;

    case OVT_ROUTE                :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->routen_nr = (unsigned) wert;
      }
      break;

    case OVT_PRIORITAET           :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->prioritaet = (unsigned) wert;
      }
      break;

    case OVT_ZUGLAENGE            :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->zuglaenge = (unsigned) wert;
      }
      break;

    case OVT_R_V_H                :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->richtung_von_hand = (unsigned) wert;
      }
      break;

    case OVT_FAHRPLAN             :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->fahrpl_sek = (int) wert;
      }
      break;

    case OVT_PERSONEN             :
      if (mp_nummer_stimmt (nummer)) {
        AlleOEVTele->inhalt->personen = (unsigned) wert;
      }
      break;

    case LSAKOPPLUNG              :
      el = LSA_Puffer->AlleEingaenge;
      while (el != NULL) {
        if (el->inhalt->nummer == nummer) {
          el->inhalt->wert = wert;
          break;
        }
        el = el->next;
      }
      if (el == NULL) {
        el = new T_EingangListe();
        if (el == NULL) {
          AllgemeinerFehler (prozedurname, kein_speicher);
          return 0;
        }
        el->next = LSA_Puffer->AlleEingaenge;
        LSA_Puffer->AlleEingaenge = el;
        el->inhalt = new T_Eingang();
        if (el->inhalt == NULL) {
          AllgemeinerFehler (prozedurname, kein_speicher);
          return 0;
        }
        el->inhalt->nummer = nummer;
        el->inhalt->wert   = wert;
      }
      break;

    case SIMZEIT                  :
      if ((double) wert / (double) LSA_Puffer->Schaltfrequenz - SimulationTime > 1E-6) {
        loesche_telegramm_liste (&AlleOEVTele);
      }
      SimulationTime = (double) wert / (double) LSA_Puffer->Schaltfrequenz;
      break;
    case SC_DLL_DATA_INDEX:
      LSA_Puffer->Index = wert;
      break;
    default                       :
      /* unerwarteter oder unbekannter Typ => ignorieren! */
      return 0;
  } /* switch (typ) */

  return 1;
} /* LongToLSAPuffer */

/*==========================================================================*/

void  VonLSA (int kommando, int nr, BOOLEAN uebergang)
{
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

  static TCHAR  prozedurname[] = _T("LSA_PUFF.VonLSA");
  T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);

  if (sg == nullptr) {
    return;
  }
 
  if (kommando == 0) {
    sg->Soll = undefiniert;
    return;
  }
  
  sg->Uebergang = uebergang;

  switch (kommando) {
    case  1 :
    case  3 :
    case  5 :
      sg->Soll = gruen;
      break;
    case  2 :
      sg->Soll = rot;
      break;
    case  4 :
    case  6 :
      sg->Soll = aus;
      break;
    case  7 :
      sg->Soll = rotgelb;
      break;
    case  8 :
      sg->Soll = gelb;
      break;
    case  9 :
      sg->Soll = gelb_blink;
      break;
    case 10 :
      sg->Soll = rot_blink;
      break;
    case 11 :
      sg->Soll = gruen_blink;
      break;
    case 12 :
      sg->Soll = rgwb;
      break;
    case 13 :
      sg->Soll = gruengelb;
      break;
    default :
      HoleRessourcenString (IDS_UNZUL_KOMMANDO, res_string, sizeof (res_string));
      InternerFehler (prozedurname, res_string);
      return;
  }
} /* VonLSA */

/*--------------------------------------------------------------------------*/

static double RoundIfAlmostInteger(double value)
{
  int roundedValue = (int)(value + 0.00005);
  return fabs(value - roundedValue) < 0.0001 ? roundedValue : value;
}

/*--------------------------------------------------------------------------*/

double  NachLSA_Real (int kommando, int nr)
{
  /* See NachLSA for a detailed description. The only difference is */
  /* the return type of double.                                     */

  TCHAR        prozedurname[] = _T("LSA_PUFF.NachLSA");
  SignalBild  Bild, Soll;
  double      TGruenMin;
  double      TRotMin;
  double      TRotgelb;
  double      TGelb;
  double      Laufzeit;

  if (kommando <= 14 || kommando >= 17) {
    T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);
    if (sg == nullptr) {
      return 0;
    }
    Bild      = sg->Bild;
    Soll      = sg->Soll;
    TGruenMin = sg->TGruenMin;
    TRotMin   = sg->TRotMin;
    TRotgelb  = sg->TRotgelb;
    TGelb     = sg->TGelb;
    Laufzeit  = sg->Laufzeit;
  }

  switch (kommando) {
    case  1 : /* Mindestrotzeit */
      return RoundIfAlmostInteger(TRotMin);
    case  2 : /* Vorbereitungszeit (Rotgelbzeit) */
      return RoundIfAlmostInteger(TRotgelb);
    case  3 : /* Mindestgruenzeit */
      return RoundIfAlmostInteger(TGruenMin);
    case  4 : /* Gelbzeit */
      return RoundIfAlmostInteger(TGelb);
    case  5 : /* Bild == Rot oder Gelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == rot || Bild == gelb);
    case  6 : /* Bild == Rotgelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == rotgelb);
    case  7 : /* Bild == Gruen oder Rotgelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == gruen || Bild == rotgelb);
    case  8 : /* Bild == Gelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == gelb);
    case  9 : /* Mindestrotzeit abgelaufen? (falls ja: 0, sonst Laufzeit) */
      return Bild == rot && Laufzeit <= TRotMin ? RoundIfAlmostInteger(Laufzeit) : 0.0;
    case 10 : /* Mindestgruenzeit abgelaufen? (falls ja: 0, sonst Laufzeit) */
      return Bild == gruen && Laufzeit <= TGruenMin ? RoundIfAlmostInteger(Laufzeit) : 0.0;
    case 11 : /* aktuelle Rotdauer (incl. Gelb) */
      return Bild == rot
        ? RoundIfAlmostInteger(Laufzeit + TGelb)
        : Bild == gelb
        ? RoundIfAlmostInteger(Laufzeit)
        : 0.0;
    case 12 : /* aktuelle Vorbereitungsdauer (Rotgelbdauer) */
      return Bild == rotgelb
        ? RoundIfAlmostInteger(Laufzeit)
        : 0.0;
    case 13 : /* aktuelle Gruendauer (incl. Rotgelb) */
      return Bild == gruen
        ? (Laufzeit + TRotgelb)
        : Bild == rotgelb
        ? RoundIfAlmostInteger(Laufzeit)
        : 0.0;
    case 14 : /* aktuelle Gelbdauer */
      return Bild == gelb
        ? RoundIfAlmostInteger(Laufzeit)
        : 0.0;
    case 15 : /* aktuelle Sekunde im Umlauf */
      return RoundIfAlmostInteger(LSA_Puffer->Sekunde);
    case 16 : /* Umlaufzeit */
      return RoundIfAlmostInteger(LSA_Puffer->TU);
    case 17 : /* Soll == Rot? (falls ja: 1, sonst 0) */
      return (Soll == rot);
    case 18 : /* Soll == Gruen? (falls ja: 1, sonst 0) */
      return (Soll == gruen);
    case 19 : /* bisherige Dauer des aktuellen Signalbilds */
      return RoundIfAlmostInteger(Laufzeit);
    default :
      HoleRessourcenString (IDS_UNZUL_KOMMANDO, res_string, sizeof (res_string));
      InternerFehler (prozedurname, res_string);
      return 0;
  }
} /* NachLSA_Real */

/*--------------------------------------------------------------------------*/

unsigned short  NachLSA (int kommando, int nr)
{
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

  TCHAR        prozedurname[] = _T("LSA_PUFF.NachLSA");
  SignalBild  Bild, Soll;
  long        TGruenMin;
  long        TRotMin;
  long        TRotgelb;
  long        TGelb;
  long        Laufzeit;

  if (kommando <= 14 || kommando >= 17) {
    T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);
    if (sg == nullptr) {
      return 0;
    }

    Bild      = sg->Bild;
    Soll      = sg->Soll;
    TGruenMin = (long) sg->TGruenMin;
    TRotMin   = (long) sg->TRotMin;
    TRotgelb  = (long) sg->TRotgelb;
    TGelb     = (long) sg->TGelb;
    Laufzeit  = (long) sg->Laufzeit;
  }

  switch (kommando) {
    case  1 : /* Mindestrotzeit */
      return long_to_unsigned_short (TRotMin);
    case  2 : /* Vorbereitungszeit (Rotgelbzeit) */
      return long_to_unsigned_short (TRotgelb);
    case  3 : /* Mindestgruenzeit */
      return long_to_unsigned_short (TGruenMin);
    case  4 : /* Gelbzeit */
      return long_to_unsigned_short (TGelb);
    case  5 : /* Bild == Rot oder Gelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == rot || Bild == gelb);
    case  6 : /* Bild == Rotgelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == rotgelb);
    case  7 : /* Bild == Gruen oder Rotgelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == gruen || Bild == rotgelb);
    case  8 : /* Bild == Gelb? (falls ja: 1, sonst 0) */
      return (unsigned short) (Bild == gelb);
    case  9 : /* Mindestrotzeit abgelaufen? (falls ja: 0, sonst Laufzeit) */
      return Bild == rot && Laufzeit <= TRotMin
               ? long_to_unsigned_short (Laufzeit)
               : (unsigned short) (0);
    case 10 : /* Mindestgruenzeit abgelaufen? (falls ja: 0, sonst Laufzeit) */
      return Bild == gruen && Laufzeit <= TGruenMin
               ? long_to_unsigned_short (Laufzeit)
               : (unsigned short) (0);
    case 11 : /* aktuelle Rotdauer (incl. Gelb) */
      return Bild == rot
               ? long_to_unsigned_short (Laufzeit + TGelb)
               : Bild == gelb
                   ? long_to_unsigned_short (Laufzeit)
                   : (unsigned short) (0);
    case 12 : /* aktuelle Vorbereitungsdauer (Rotgelbdauer) */
      return Bild == rotgelb
               ? long_to_unsigned_short (Laufzeit)
               : (unsigned short) (0);
    case 13 : /* aktuelle Gruendauer (incl. Rotgelb) */
      return Bild == gruen
               ? long_to_unsigned_short (Laufzeit + TRotgelb)
               : Bild == rotgelb
                 ? long_to_unsigned_short (Laufzeit)
                 : (unsigned short) (0);
    case 14 : /* aktuelle Gelbdauer */
      return Bild == gelb
               ? long_to_unsigned_short (Laufzeit)
               : (unsigned short) (0);
    case 15 : /* aktuelle Sekunde im Umlauf */
      return long_to_unsigned_short ((long) LSA_Puffer->Sekunde);
    case 16 : /* Umlaufzeit */
      return long_to_unsigned_short ((long) LSA_Puffer->TU);
    case 17 : /* Soll == Rot? (falls ja: 1, sonst 0) */
      return (unsigned short) (Soll == rot);
    case 18 : /* Soll == Gruen? (falls ja: 1, sonst 0) */
      return (unsigned short) (Soll == gruen);
    case 19 : /* bisherige Dauer des aktuellen Signalbilds */
      return long_to_unsigned_short (Laufzeit);
    default :
      HoleRessourcenString (IDS_UNZUL_KOMMANDO, res_string, sizeof (res_string));
      InternerFehler (prozedurname, res_string);
      return 0;
  }
} /* NachLSA */

/*==========================================================================*/

float  Simulationssekunde (void)
{
  /* Gibt die aktuelle Simulationssekunde zurueck. */

  return (float) SimulationTime;
} /* Simulationssekunde */

/*==========================================================================*/

float  Umlaufsekunde (void)
{
  /* Gibt die aktuelle Umlaufsekunde zurueck. */

  return (float) LSA_Puffer->Sekunde;
} /* Umlaufsekunde */

/*==========================================================================*/

int  ProgrammNummer (void)
{
  /* Gibt die Nummer des laufenden Programms zurueck (Default = 1). */

  return (int) LSA_Puffer->ProgNr;
} /* ProgrammNummer */

/*==========================================================================*/

int  sg_definiert (int nr)
{
  /* Falls die Signalgruppe Nr. <nr> in VISSIM definiert ist, wird 1 */
  /* zurückgegeben, andernfalls 0.                                   */

  T_SigGr * sg = sg_nr_to_object(nr, NULL, false);

  if (sg == nullptr) {
    return 0;
  }
  return 1;
} /* sg_definiert */

/*--------------------------------------------------------------------------*/

tstring sg_name(int nr)
{
  static TCHAR  prozedurname[] = _T("LSA_PUFF.sg_name");
  T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);

  if (sg == nullptr) {
    return 0;
  }
  return sg->Name;
}

/*--------------------------------------------------------------------------*/

int  det_definiert (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM definiert ist, wird 1 */
  /* zurückgegeben, andernfalls 0.                               */

  T_Det * det = det_nr_to_object(nr, NULL, false);

  if (det == nullptr) {
    return 0;
  }
  return 1;
} /* det_definiert */

/*==========================================================================*/

int  d_imp (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
  /* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
  /* Ansonsten wird der Wert des Impulsspeichers des Detektors Nr. <nr>       */
  /* zurueckgegeben: 1, wenn sich der Zustand des Detektors von unbelegt auf  */
  /* belegt geändert hat (wenn also eine Fahrzeugvorderkante erfasst wurde,   */
  /* während nicht schon ein anderes Fahrzeug auf dem Detektor war), sonst 0. */
  /* Der Impulsspeicher wird von VISSIM nicht automatisch geloescht - siehe   */
  /* Funktion d_limp() weiter unten!                                          */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_imp");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return det->Impulsspeicher;
} /* d_imp */

/*--------------------------------------------------------------------------*/

int  d_limp (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert -1).                     */
  /* Ansonsten wird beim Detektor Nr. <nr> vermerkt, dass dessen        */
  /* Impulsspeicher am Ende des Zeitschritts auf 0 gesetzt werden soll, */
  /* und 0 zurueckgegeben.                                              */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_limp");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return -1;
  }
  det->ImpulsspeicherLoeschen = TRUE;
  return 0;
} /* d_limp */

/*--------------------------------------------------------------------------*/

int  d_blgzt (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
  /* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
  /* Ansonsten wird 0 zurueckgegeben, falls sich zum Ende des Zeitschritts    */
  /* kein Fahrzeug auf dem Detektor Nr. <nr> befindet, andernfalls die Zeit   */
  /* seit seinem Eintreffen in 1/10 s.                                        */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_blgzt");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  if (det->Anwesenheit) {
    return (int) MIN (INT_MAX, det->Belegungsdauer / 10);
  }
  else {
    return 0;
  }
} /* d_blgzt */

/*--------------------------------------------------------------------------*/

int  d_belgg (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
  /* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
  /* Ansonsten wird der Wert des exponentiell geglaetteten Belegungsgrads     */
  /* in % des Detektors Nr. <nr> zurueckgegeben (Glaettungsfaktoren gemaess   */
  /* Versorgung dieses Detektors in VISSIM).                                  */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_belgg");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return (int) det->geglaetteteBelegung;
} /* d_belgg */

/*--------------------------------------------------------------------------*/

long  d_ztlkn (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt    */
  /* eine Fehlermeldung (Rueckgabewert LONG_MAX = 2147483647L).            */
  /* Ansonsten wird die angefangene Nettozeitluecke beim Detektor Nr. <nr> */
  /* in 1/100 s zurueckgegeben (aber maximal LONG_MAX).                    */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_ztlkn");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return LONG_MAX;
  }
  return MIN (LONG_MAX, det->angefangeneZeitluecke);
} /* d_ztlkn */

/*--------------------------------------------------------------------------*/

long  d_v (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
  /* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
  /* Ansonsten wird 0 zurueckgegeben, falls seit dem letzten Durchlauf durch  */
  /* die Steuerungslogik kein Fahrzeug vom Detektor Nr. <nr> erfasst wurde,   */
  /* andernfalls die aktuelle Geschwindigkeit des letzten erfassten Fahrzeugs */
  /* in 1/100 m/s.                                                            */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_v");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return MIN (LONG_MAX, det->v);
} /* d_v */

/*--------------------------------------------------------------------------*/

long  d_l (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt eine  */
  /* Laufzeitfehlermeldung (Rueckgabewert 0).                                 */
  /* Ansonsten wird 0 zurueckgegeben, falls seit dem letzten Durchlauf durch  */
  /* die Steuerungslogik kein Fahrzeug vom Detektor Nr. <nr> erfasst wurde,   */
  /* andernfalls die Laenge des letzten erfassten Fahrzeugs in mm.            */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_v");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return MIN (LONG_MAX, det->l);
} /* d_l */

/*--------------------------------------------------------------------------*/

DetectorType d_typ(int nr)
{
  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_typ");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return UndefinedDetector;
  }
  return det->Type;
}

/*--------------------------------------------------------------------------*/

tstring d_name(int nr)
{
  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_name");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return det->Name;
}
/*--------------------------------------------------------------------------*/
unsigned char  anwesenheit (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt    */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                         */
  /* Ansonsten wird 1 zurueckgegeben, falls sich zum Ende des aktuellen    */
  /* Zeitschritts ein Fahrzeug im Erfassungsbereich des Detektors Nr. <nr> */
  /* befindet, andernfalls 0.                                              */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.anwesenheit");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  if (det->Anwesenheit) {
    return 1;
  }
  else {
    return 0;
  }
} /* anwesenheit */

/*--------------------------------------------------------------------------*/

unsigned char  zaehlwert (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der seit dem letzten Durchlauf durch die */
  /* Steuerungslogik erfassten Fahrzeugvorderkanten zurueckgegeben      */
  /* (wobei einander ueberlappende Impulse zu einem zusammengefasst     */
  /* werden).                                                           */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.zaehlwert");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return (unsigned char) det->Vorderkanten;
} /* zaehlwert */

/*--------------------------------------------------------------------------*/

unsigned char  hinterkanten (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der seit dem letzten Durchlauf durch     */
  /* die Steuerungslogik erfassten Fahrzeughinterkanten zurueckgegeben  */
  /* (wobei einander ueberlappende Impulse zu einem zusammengefasst     */
  /* werden).                                                           */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.hinterkanten");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return (unsigned char) det->Hinterkanten;
} /* hinterkanten */

/*--------------------------------------------------------------------------*/

unsigned long  vorderkantensumme (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der seit der letzten Loeschung erfassten */
  /* Fahrzeugvorderkanten zurueckgegeben (wobei einander ueberlappende  */
  /* Impulse zu einem zusammengefasst werden).                          */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.vorderkantensumme");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return det->VorderkantenSumme;
} /* vorderkantensumme */

/*--------------------------------------------------------------------------*/

void  loesche_vorderkantensumme (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der erfassten Fahrzeugvorderkanten       */
  /* geloescht.                                                         */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.loesche_vorderkantensumme");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return;
  }
  det->VorderkantenSumme = 0;
} /* loesche_vorderkantensumme */

/*--------------------------------------------------------------------------*/

unsigned long  hinterkantensumme (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der seit der letzten Loeschung erfassten */
  /* Fahrzeughinterkanten zurueckgegeben (wobei einander ueberlappende  */
  /* Impulse zu einem zusammengefasst werden).                          */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.hinterkantensumme");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return det->HinterkantenSumme;
} /* hinterkantensumme */

/*--------------------------------------------------------------------------*/

void  loesche_hinterkantensumme (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird die Anzahl der erfassten Fahrzeughinterkanten       */
  /* geloescht.                                                         */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.loesche_hinterkantensumme");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return;
  }
  det->HinterkantenSumme = 0;
} /* loesche_hinterkantensumme */

/*--------------------------------------------------------------------------*/

unsigned char  belegung (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                      */
  /* Ansonsten wird der (nicht geglaettete) Belegungsgrad des           */
  /* Detektors Nr. <nr> seit dem letzten Durchlauf durch die            */
  /* Steuerungslogik zurueckgegeben (in %).                             */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.belegung");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return (unsigned char) det->aktuelleBelegung;
} /* belegung */

/*--------------------------------------------------------------------------*/

unsigned char  abgeschl_luecke (int nr)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist, erfolgt  */
  /* eine Laufzeitfehlermeldung (Rueckgabewert 0).                       */
  /* Ansonsten wird der 0 zurueckgegeben, falls seit dem letzten         */
  /* Durchlauf durch die Steuerungslogik keine Zeitluecke begonnen UND   */
  /* beendet wurde, andernfalls die Dauer der letzten solchen Zeitluecke */
  /* in 1/100 s.                                                         */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.abgeschl_luecke");
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return 0;
  }
  return (unsigned char) det->abgeschlosseneZeitluecke;
} /* abgeschl_luecke */

/*--------------------------------------------------------------------------*/

int  d_on (int nr, int intervall_nr, int intervalle_pro_schaltint)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,         */
  /* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).             */
  /* Ansonsten wird 1 zurueckgegeben, falls im Intervall Nummer         */
  /* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint>  */
  /* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf     */
  /* durch die Steuerungslogik eine Fahrzeugvorderkante erfasst wurde,  */
  /* sonst 0.                                                           */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_on");
  int   i;
  long  t_von, t_bis;
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return -1;
  }
  t_von = 100L * intervall_nr       / intervalle_pro_schaltint;
  t_bis = 100L * (intervall_nr + 1) / intervalle_pro_schaltint;
  i = 0;
  while (i < MAX_KANTEN_PRO_SI && det->tVorderkante[i] != -1) {
    if (   det->tVorderkante[i] >= t_von
        && det->tVorderkante[i] <  t_bis) {
      return 1;
    }
    i++;
  }
  return 0;
} /* d_on */

/*--------------------------------------------------------------------------*/

int  d_off (int nr, int intervall_nr, int intervalle_pro_schaltint)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,        */
  /* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).            */
  /* Ansonsten wird 1 zurueckgegeben, falls im Intervall Nummer        */
  /* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint> */
  /* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf    */
  /* durch die Steuerungslogik eine Fahrzeughinterkante erfasst wurde, */
  /* sonst 0.                                                          */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_off");
  int   i;
  long  t_von, t_bis;
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return -1;
  }

  t_von = 100L * intervall_nr       / intervalle_pro_schaltint;
  t_bis = 100L * (intervall_nr + 1) / intervalle_pro_schaltint;
  i = 0;
  while (i < MAX_KANTEN_PRO_SI && det->tHinterkante[i] != -1) {
    if (   det->tHinterkante[i] >= t_von
        && det->tHinterkante[i] <  t_bis) {
      return 1;
    }
    i++;
  }
  return 0;
} /* d_off */

/*--------------------------------------------------------------------------*/

int  d_presence (int nr, int intervall_nr, int intervalle_pro_schaltint)
{
  /* Falls der Detektor Nr. <nr> in VISSIM nicht definiert ist,           */
  /* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert -1).               */
  /* Ansonsten wird 1 zurueckgegeben, falls am Ende des Intervalls Nummer */
  /* <intervall_nr> (gezaehlt von 0 an) von <intervalle_pro_schaltint>    */
  /* ([1..100]) gleichlangen Intervallen seit dem letzten Durchlauf       */
  /* durch die Steuerungslogik ein Fahrzeug über dem Detektor war,        */
  /* sonst 0.                                                             */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.d_presence");
  int   i, t, before;
  T_Det * det = det_nr_to_object(nr, prozedurname, false);

  if (det == nullptr) {
    return -1;
  }

  if (   det->tVorderkante[0] == -1
      && det->tHinterkante[0] == -1) {
    if (det->Anwesenheit) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    t = 100 * (intervall_nr + 1) / intervalle_pro_schaltint;
    for (i = 0; i < MAX_KANTEN_PRO_SI; i++) {
      if (det->tVorderkante[i] == -1) {
        if (det->tHinterkante[i] == -1) {
          break;
        }
        else {
          if (det->tHinterkante[i] <= t) {
            return 0;
          }
          else {
            return 1;
          }
        }
      } /* if (LSA_Puffer->Det[index]->tVorderkante[i] == -1) */
      else {
        if (det->tHinterkante[i] == -1) {
          if (det->tVorderkante[i] <= t) {
            return 1;
          }
          else {
            return 0;
          }
        }
        else {
          if (   det->tVorderkante[i] 
              <= det->tHinterkante[i]) {
            if (det->tVorderkante[i] <= t) {
              before = 1;
            }
            else {
              return 0;
            }
            if (det->tHinterkante[i] <= t) {
              before = 0;
            }
            else {
              return 1;
            }
          } /* if (tVorderkante[i] <= tHinterkante[i]) */
          else {
            if (det->tHinterkante[i] <= t) {
              before = 0;
            }
            else {
              return 1;
            }
            if (det->tVorderkante[i] <= t) {
              before = 1;
            }
            else {
              return 0;
            }
          } /* tVorderkante[i] > tHinterkante[i] */
        } /* LSA_Puffer->Det[index]->tHinterkante[i] != -1 */
      } /* LSA_Puffer->Det[index]->tVorderkante[i] != -1 */
    } /* for (i = 0; i < MAX_KANTEN_PRO_SI; i++) */
    return before;
  }
} /* d_presence */

/*--------------------------------------------------------------------------*/

void  SetzeUmlaufsekunde (float uml_sek)
{
  /* Setzt die aktuelle Umlaufsekunde auf <uml_sek>.                  */
  /* (Dieser Wert wird am Ende des Zeitschritts an VISSIM uebergeben, */
  /* auf ein volles Schaltintervall (Kehrwert der Schaltfrequenz)     */
  /* genau.)                                                          */

  LSA_Puffer->Sekunde = uml_sek;
} /* SetzeUmlaufsekunde */

/*--------------------------------------------------------------------------*/

void  SetzeProgrammnummer (unsigned prg_nr)
{
  /* Setzt die aktuelle Programmnummer auf <prg_nr>.                   */
  /* (Dieser Wert wird am Ende des Zeitschritts an VISSIM uebergeben.) */

  LSA_Puffer->ProgNr = prg_nr;
} /* SetzeProgrammnummer */

/*--------------------------------------------------------------------------*/

BOOLEAN  SchreibeProtTripel (long code, long nummer, long wert)
{
  /* Schreibt ein neues Protokoll-Tripel in den LSA-Puffer.    */
  /* <code>, <nummer> und <wert> sollten in den Wertebereichen */
  /* liegen, die in der WTT-Datei des Steuerungsverfahrens     */
  /* definiert sind. Insbesondere sollte <code> > 9999 sein.   */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE          */
  /* und ein Laufzeitfehler erzeugt.                           */

  if (*LSA_Puffer->pNaechstesTripel == NULL) {
    *LSA_Puffer->pNaechstesTripel = new T_ProtTripel();
    if (*LSA_Puffer->pNaechstesTripel == NULL) {
      AllgemeinerFehler (_T("LSA_PUFF.SchreibeProtTripel"), kein_speicher);
      return FALSE;
    }
    (*LSA_Puffer->pNaechstesTripel)->next = NULL;
  }
  (*LSA_Puffer->pNaechstesTripel)->Code   = code;
  (*LSA_Puffer->pNaechstesTripel)->Nummer = nummer;
  (*LSA_Puffer->pNaechstesTripel)->Wert   = wert;
  LSA_Puffer->pNaechstesTripel = &(*LSA_Puffer->pNaechstesTripel)->next;
  LSA_Puffer->AnzProtTripel++;
  return TRUE;
} /* SchreibeProtTripel */

/*--------------------------------------------------------------------------*/

int  Anzahl_OEV_Tele (void)
{
  /* Gibt die Anzahl der im aktuellen Zeitschritt empfangenen */
  /* OEV-Telegramme zurueck.                                  */

  TelegrammListe  tl = AlleOEVTele;
  int             result = 0;

  while (tl != NULL) {
    result++;
    tl = tl->next;
  }
  return result;
} /* Anzahl_OEV_Tele */

/*--------------------------------------------------------------------------*/

static  TelegrammListe  oev_tele_nr (unsigned index)
{
  /* Gibt einen Zeiger auf das Telegramm Nummer <index> (erstes: 1) */
  /* aus dem aktuellen Zeitschritt zurueck, bzw. NULL, wenn nicht   */
  /* soviele empfangen wurden.                                      */

  TelegrammListe  result = AlleOEVTele;
  unsigned        zaehler = 1;

  while (result != NULL && zaehler != index) {
    zaehler++;
    result = result->next;
  }
  return result;
} /* oev_tele_nr */

/*--------------------------------------------------------------------------*/

long  OEV_Tele_MP (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
  /* Meldepunkt-Nummer aus dem Telegramm Nummer <index> (erstes: 1).        */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->mp_nr;
  }
  else {
    return -1L;
  }
} /* OEV_Tele_MP */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Linie (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
  /* Liniennummer aus dem Telegramm Nummer <index> (erstes: 1).             */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->linien_nr;
  }
  else {
    return -1;
  }
} /* OEV_Tele_Linie */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Kurs (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
  /* Kursnummer aus dem Telegramm Nummer <index> (erstes: 1).               */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->kurs_nr;
  }
  else {
    return -1;
  }
} /* OEV_Tele_Kurs */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Route (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
  /* Routennummer aus dem Telegramm Nummer <index> (erstes: 1).             */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->routen_nr;
  }
  else {
    return -1;
  }
} /* OEV_Tele_Route */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Prio (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls der    */
  /* Prioritaetswert aus dem Telegramm Nummer <index> (erstes: 1).          */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->prioritaet;
  }
  else {
    return -1;
  }
} /* OEV_Tele_Prio */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Zuglaenge (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls die    */
  /* Zuglaenge aus dem Telegramm Nummer <index> (erstes: 1).                */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->zuglaenge;
  }
  else {
    return -1;
  }
} /* OEV_Tele_Zuglaenge */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_RVH (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird -1 zurueckgegeben, andernfalls der    */
  /* Code "Richtung von Hand" aus dem Telegramm Nummer <index> (erstes: 1). */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->richtung_von_hand;
  }
  else {
    return -1;
  }
} /* OEV_Tele_RVH */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Fahrplanlage (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird 0 zurueckgegeben, andernfalls die     */
  /* Fahrplanlage [s] aus dem Telegramm Nummer <index> (erstes: 1).         */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->fahrpl_sek;
  }
  else {
    return 0;
  }
} /* OEV_Tele_Fahrplanlage */

/*--------------------------------------------------------------------------*/

int  OEV_Tele_Personen (unsigned index)
{
  /* Falls <index> groesser ist als die Anzahl der im aktuellen Zeitschritt */
  /* empfangenen OEV-Telegramme, wird 0 zurueckgegeben, andernfalls die     */
  /* Fahrgastanzahl aus dem Telegramm Nummer <index> (erstes: 1).           */

  TelegrammListe  tl = oev_tele_nr (index);

  if (tl != NULL) {
    return tl->inhalt->personen;
  }
  else {
    return 0;
  }
} /* OEV_Tele_Personen */

/*--------------------------------------------------------------------------*/

long  LiesEingang (long nummer)
{
  /* Gibt den am LSA-Kopplungs-Eingang Nummer <nummer> zuletzt eingegangen */
  /* Wert zurueck, bzw. -1, falls dieser Eingang nicht verkabelt ist.      */

  EingangListe  el = LSA_Puffer->AlleEingaenge;

  while (el != NULL) {
    if (el->inhalt->nummer == nummer) {
      return el->inhalt->wert;
    }
    el = el->next;
  }
  return -1;
} /* LiesEingang */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeAusgang (long nummer, long wert)
{
  /* Setzt dem LSA-Kopplungs-Ausgang Nummer <nummer> auf <wert>. */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE            */
  /* und ein Laufzeitfehler erzeugt.                             */

  EingangListe  al = LSA_Puffer->AlleAusgaenge;
  TCHAR          prozedurname[] = _T("SetzeAusgang");

  while (al != NULL) {
    if (al->inhalt->nummer == nummer) {
      al->inhalt->wert = wert;
      return TRUE;
    }
    al = al->next;
  }
  al = new T_EingangListe();
  if (al == NULL) {
    AllgemeinerFehler (prozedurname, kein_speicher);
    return FALSE;
  }
  al->next = LSA_Puffer->AlleAusgaenge;
  LSA_Puffer->AlleAusgaenge = al;
  al->inhalt = new T_Eingang();
  if (al->inhalt == NULL) {
    AllgemeinerFehler (prozedurname, kein_speicher);
    return FALSE;
  }
  al->inhalt->nummer = nummer;
  al->inhalt->wert   = wert;
  return TRUE;
} /* SetzeAusgang */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeRoute (long rout_ent, long route, long rel_bel)
{
  /* Setzt die relative Belastung der Route <route> der */
  /* Routenentscheidung <rout_ent> auf <rel_bel>.       */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE   */
  /* und ein Laufzeitfehler erzeugt.                    */

  if (!SchreibeProtTripel (SETZE_ROUTE, rout_ent, 0)) {
    return FALSE;
  }
  if (!SchreibeProtTripel (SETZE_ROUT_BEL, route, rel_bel)) {
    return FALSE;
  }
  return TRUE;
} /* SetzeRoute */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetzeVWunsch (long vwunsch_ent, long fz_kl, long vwunsch_vert)
{
  /* Ordnet in der Wunschgeschwindigkeitsentscheidung    */
  /* <vwunsch_ent>  der Fahrzeugklasse <fz_kl> die       */
  /* Wunschgeschwindigkeitsverteilung <vwunsch_vert> zu. */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE    */
  /* und ein Laufzeitfehler erzeugt.                     */

  if (!SchreibeProtTripel (SETZE_VWUNSCH, vwunsch_ent, 0)) {
    return FALSE;
  }
  if (!SchreibeProtTripel (SETZE_VWU_VERT, fz_kl, vwunsch_vert)) {
    return FALSE;
  }
  return TRUE;
} /* SetzeVWunsch */

/*--------------------------------------------------------------------------*/

BOOLEAN  LiesDebugModus (void)
{
  /* Gibt den von VISSIM uebergebenen Debug-Modus zurueck - */
  /* wenn nichts uebergeben wurde, FALSE.                   */

  return LSA_Puffer->DebugModus;
} /* LiesDebugModus */

/*--------------------------------------------------------------------------*/

SignalBild  SigGrBild (int nr)
{
  /* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist,    */
  /* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert <undefiniert>). */
  /* Ansonsten wird das aktuelle Signalbild der Signalgruppe Nummer    */
  /* <nr> zurueckgegeben.                                              */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.SigGrBild");
  T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);

  if (sg == nullptr) {
    return undefiniert;
  }
  return sg->Bild;
} /* SigGrBild */

/*--------------------------------------------------------------------------*/

SignalBild  SigGrSoll (int nr)
{
  /* Falls die Signalgruppe Nr. <nr> in VISSIM nicht definiert ist,    */
  /* erfolgt eine Laufzeitfehlermeldung (Rueckgabewert <undefiniert>). */
  /* Ansonsten wird das letzte von der Steuerung gesetzte Sollbild     */
  /* der Signalgruppe Nummer <nr> zurueckgegeben.                      */

  static TCHAR  prozedurname[] = _T("LSA_PUFF.SigGrSoll");
  T_SigGr * sg = sg_nr_to_object(nr, prozedurname, false);

  if (sg == nullptr) {
    return undefiniert;
  }
  if (sg->Soll == undefiniert) {
    return sg->AltSoll;
  }
  else {
    return sg->Soll;
  }
} /* SigGrSoll */

/*--------------------------------------------------------------------------*/

void  SetzeSchaltfrequenz (unsigned freq)
{
  /* Setzt die Schaltfrequenz der LSA auf <freq> [1..10] Durchlaeufe pro   */
  /* Sekunde.                                                              */
  /* Dieser Wert wird nach der Initialisierung der Steuerung an VISSIM     */
  /* uebergeben. Die Funktion muss also schon waehrend der Initialisierung */
  /* aufgerufen werden, wenn die Schaltfrequenz hoeher als der             */
  /* Standardwert 1 ist.                                                   */

  if (freq >= 1 && freq <= 10) {
    LSA_Puffer->Schaltfrequenz = freq;
  }
} /* SetzeSchaltfrequenz */

/*--------------------------------------------------------------------------*/

BOOLEAN  SetStopTimeDist (long stop, long fz_kl, long vwunsch_vert)
{
  /* Ordnet im Stopschild <stop> der Fahrzeugklasse <fz_kl>  */
  /* die Wunschgeschwindigkeitsverteilung <vwunsch_vert> zu. */
  /* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE        */
  /* und ein Laufzeitfehler erzeugt.                         */
  /*                                                         */
  /* Assigns the desired speed distribution <vwunsch_vert>   */
  /* to the vehicle class <fz_kl> in the stop sign <stop>.   */
  /* If that doesn't work, a runtime error occurs (return    */
  /* value FALSE). Otherwise the return value is TRUE.       */

  if (!SchreibeProtTripel (SETZE_STOP, stop, 0)) {
    return FALSE;
  }
  if (!SchreibeProtTripel (SETZE_STOP_ZVERT, fz_kl, vwunsch_vert)) {
    return FALSE;
  }
  return TRUE;
} /* SetStopTimeDist */

/*==========================================================================*/
/*  Ende von LSA_PUFF.CPP                                                   */
/*==========================================================================*/
