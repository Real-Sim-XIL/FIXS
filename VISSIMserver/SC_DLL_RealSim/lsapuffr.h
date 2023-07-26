/*==========================================================================*/
/*  LSAPUFFR.H                                             Teil von VISSIM  */
/*                                                        und ????????/SIM  */
/*                                                                          */
/*  Datenschnittstelle zwischen Simulation und LSA-Steuerung.               */
/*                                                                          */
/*  Version von 2004-12-14                                  Lukas Kautzsch  */
/*==========================================================================*/

#ifndef  __LSAPUFFR_H
#define  __LSAPUFFR_H

/*==========================================================================*/

#define  BILD_AUS           0
#define  BILD_AN            1
#define  BILD_ROT           2
#define  BILD_ROTGELB       3
#define  BILD_GRUEN         4
#define  BILD_GELB          5
#define  BILD_GELB_BLINK    6
#define  BILD_ROT_BLINK     7
#define  BILD_GRUEN_BLINK   8
#define  BILD_RGWB          9 /* rot grün wechselblinken */
#define  BILD_GRUENGELB    10
#define  BILD_UNDEFINIERT  99

#define  RS_KEIN            0
#define  RS_VORZIEHEN       1
#define  RS_ANMELDUNG       2
#define  RS_VERLAENGERN     3

#define  DATEN_ENDE       999

/* LSA-Parameter */
#define  SCHALTFREQUENZ  1000   /* Logikdurchläufe pro Sekunde */
#define  UMLAUFZEIT      1001   /* [s] */
#define  UMLAUFSEKUNDE   1002   /* [s / SCHALTFREQUENZ] */
#define  PROGRAMMNUMMER  1011
#define  LSAKOPPLUNG     1021
#define  DEBUGMODUS      1031

/* Signalgruppen-Parameter */
#define  IST_BILD        2001   /* Werte siehe oben                           */
#define  SOLL_BILD       2002   /* Werte siehe oben                           */
#define  SOLL_BILD_O_UE  2003   /* Ohne Uebergang, Werte siehe oben           */
#define  T_GRUEN_MIN     2011   /* Mindestgruenzeit [s]                       */
#define  T_ROT_MIN       2012   /* Mindestrotzeit   [s]                       */
#define  T_ROTGELB       2013   /* Rotgelbzeit      [s]                       */
#define  T_GELB          2014   /* Gelbzeit         [s]                       */
#define  LAUFZEIT        2021   /* des aktuellen Signalbilds [s]              */

#define  ZZ_RAEUM        2101   /* Erstes Tripel einer Zwischenzeitzeile:     */
                                /* Nummer der raeumenden SG (Wert irrelevant) */
#define  ZZ_EINFAHR      2102   /* Zwischenzeit zwischen der letztgenannten   */
                                /* raeumenden und der diesmal genannten       */
                                /* einfahrenden Signalgruppe (-127 = vertr.)  */

/* Detektor-Parameter */
#define  ANWESENHEIT       3001   /* am Ende dieses Schaltintervalls {0 = FALSE} */
#define  IMPULSSPEICHER    3011   /* Neuanmeldung vorhanden?     {0 = FALSE}     */
                                  /* (muss von der Steuerung geloescht werden!   */
#define  VORDERKANTEN      3012   /* Anzahl in diesem Schaltintervall            */
#define  HINTERKANTEN      3013   /* Anzahl in diesem Schaltintervall            */
#define  T_VORDERKANTE     3014   /* rel. Zeitpunkt in diesem Schaltintervall [0..99 / SCHALTFREQUENZ] */
#define  T_HINTERKANTE     3015   /* rel. Zeitpunkt in diesem Schaltintervall [0..99 / SCHALTFREQUENZ] */
#define  ANG_ZEITLUECKE    3021   /* aktuelle angefangene Zeitluecke     [1/100 s]   */
#define  KOMPL_ZEITLUECKE  3022   /* letzte komplette in diesem SI       [1/100 s]   */
#define  ANG_BELEGUNG      3031   /* laufende Belegungszeit              [1/100 s]   */
#define  AKT_BELEGUNG      3040   /* in diesem Schaltintervall           [% / SCHALTFREQUENZ] */
#define  GEGL_BELEGUNG     3041   /* geglaettete Belegung in diesem SI   [% / SCHALTFREQUENZ] */
#define  GESCHWINDIGKEIT   3050   /* in diesem Schaltintervall (sonst 0) [1/100 m/s] */
#define  FZLAENGE          3051   /* in diesem Schaltintervall (sonst 0) [mm]        */
#define  OVT_MP            3081   /* OEV-Telegramm: Meldepunktsnummer            */
#define  OVT_LINIE         3082   /* OEV-Telegramm: Liniennummer                 */
#define  OVT_KURS          3083   /* OEV-Telegramm: Kursnummer                   */
#define  OVT_ROUTE         3084   /* OEV-Telegramm: Routennummer                 */
#define  OVT_PRIORITAET    3085   /* OEV-Telegramm: Prioritaet                   */
#define  OVT_ZUGLAENGE     3086   /* OEV-Telegramm: Zuglaenge                    */
#define  OVT_R_V_H         3087   /* OEV-Telegramm: Richtung von Hand            */
#define  OVT_FAHRPLAN      3088   /* OEV-Telegramm: Fahrplanlage         [s]     */
#define  OVT_PERSONEN      3089   /* OEV-Telegramm: Anzahl Fahrgaeste            */
#define  DET_WARTEZEIT     3101   /* von VS-PLUS                         [s]     */
#define  DET_TYPE          3102   /* Detektor-Typ                                */
#ifndef _SIMULA_PDMC
#define  MP_OEV_WARTEZEIT  3201   /* von SIEMENS-VS-PLUS                 [s]     */
#else
#define  MP_OEV_WARTEZEIT 10022
#endif

/* Verkehrsstrom-Parameter */
#define  VS_ZUSTAND                4001
#define  VS_WARTEZEIT              4002  /* von VS-PLUS [s] */
#define  VS_STUFE                  4003
#define  VS_KONTROLLZEIT           4004  /* nicht mehr unterstuetzt */
#define  VS_VORBEREITUNGSBEREICH   4011  /* war frueher mal auf der Liste */
#define  VS_ANMELDEBEREICH         4012  /* war frueher mal auf der Liste */
#define  VS_VERLAENGERUNGSBEREICH  4013  /* war frueher mal auf der Liste */
#define  VS_AKTIVFLAG              4101  /* nicht mehr unterstuetzt */
#define  VS_G_MAX                  4102  /* von VS-PLUS ab Version 3.0 */
#define  VS_PRIOKLASSE             4103  /* von VS-PLUS ab Version 3.0 */
#define  VS_PF_WERT                4104  /* von VS-PLUS ab Version 3.0 */
#define  VS_ANFO_GUELTIG           4105  /* von VS-PLUS ab Version 3.0 */
#define  VS_GRUENZEIT              4106  /* nicht mehr unterstuetzt */
#define  VS_ANFO_TYP               4107  /* nicht mehr unterstuetzt */
#define  VS_WEITERGABE             4108  /* nicht mehr unterstuetzt */
#define  VS_WUNSCH                 4109  /* von VS-PLUS ab Version 3.0 */
#define  VS_RAHMENSIGNAL           4110  /* von VS-PLUS ab Version 3.0 */
#define  VS_STATUS                 4111  /* von VS-PLUS ab Version 3.0 */
#define  VS_DT_RAHMEN              4112  /* von VS-PLUS ab Version 3.0 */
#define  VS_RESTFAHRZEIT           4113  /* von VS-PLUS ab Version 3.0 */
#define  VS_OEV_PRIO               4114  /* von VS-PLUS ab Version 3.0 */
#ifndef _SIMULA_PDMC
#define  VS_OEV_WARTEZEIT          4201  /* von SIEMENS-VS-PLUS */
#define  VS_OEV_LINIE              4202  /* von SIEMENS-VS-PLUS */
#define  VS_OEV_ROUTE              4203  /* von SIEMENS-VS-PLUS */
#define  VS_OEV_KURS               4204  /* von SIEMENS-VS-PLUS */
#define  VS_FOLGEZUEGE             4205  /* von SIEMENS-VS-PLUS */
#define  VS_ABMELDEZEIT            4206  /* von SIEMENS-VS-PLUS */
#define  VS_NOTAN_WARTEZEIT        4207  /* von SIEMENS-VS-PLUS */
#else
#define  VS_OEV_WARTEZEIT         10021
#define  VS_OEV_LINIE             10023
#define  VS_OEV_ROUTE             10024
#define  VS_OEV_KURS              10025
#define  VS_FOLGEZUEGE            10026
#define  VS_ABMELDEZEIT           10027
#define  VS_NOTAN_WARTEZEIT       10028
#endif

/* Prioelement-Parameter */
#define  HAUPTZEIGER       5001   /* von VS-PLUS                                */

/* Teilknoten-Parameter */
#define  TK_AKTIVFLAG      6001    /* von VS-PLUS                                */

/* VISSIM-Fernsteuerung */
#define  SETZE_ROUTE       7001
#define  SETZE_ROUT_BEL    7002
#define  SETZE_VWUNSCH     7011
#define  SETZE_VWU_VERT    7012
#define  SETZE_STOP        7101
#define  SETZE_STOP_ZVERT  7102

/* sonstige Parameter */
#define  TESTWERT          9001   /* von VS-PLUS */
#define  SIMZEIT           9101   /* aktueller Simulationszeitpunkt [s / SCHALTFREQUENZ] */
#define  DEMOVERSION       9201   /* Kennung fuer VISSIM-Demoversion */

/*==========================================================================*/

short  SchreibeTripel (long typ, long nummer, long wert);

/* Schreibt <wert> als Wert Nummer <nummer> des Typs <typ> in den LSAPuffer. */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE.                         */

/*==========================================================================*/

short  LiesTripel (long *typ, long *nummer, long *wert);

/* Gibt in <*wert> den naechsten Wert aus dem LSAPuffer zurueck, */
/* in <*typ> seinen Typ und in <*nummer> seine Nummer.           */
/* Bei Erfolg wird TRUE zurueckgegeben, sonst FALSE.             */

/*==========================================================================*/

#endif /* __LSAPUFFR_H */

/*==========================================================================*/
/*  Ende von LSAPUFFR.H                                                     */
/*==========================================================================*/


