/*==========================================================================*/
/*  SC_DLL_MAIN.H                                                           */
/*                                                                          */
/*  Main module of a signal controller DLL for VISSIM.                      */
/*                                                                          */
/*  Version of 2005-01-26                                   Lukas Kautzsch  */
/*==========================================================================*/

#ifndef  __SC_DLL_MAIN_H
#define  __SC_DLL_MAIN_H

/*--------------------------------------------------------------------------*/

void  SC_DLL_ReadDataFiles(unsigned long sc_no, const TCHAR * filename1, const TCHAR * filename2);

/* Reads the data file(s) of the signal controller number <sc_no>. */
/* The filenames (as user defined in VISSIM) are <filename1> and   */
/* <filename2> (NULL if empty in VISSIM, i.e. not used).           */
/* This function is called from VISSIM once per SC at the start of */
/* a simulation (or test) run, before SC_DLL_Init() is called for  */
/* any SC.                                                         */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Init (unsigned long sc_no);

/* Initializes the controller logic of SC number <sc_no>.          */
/* This function is called from VISSIM once per SC at the start of */
/* a simulation (or test) run, after SC_DLL_ReadDataFiles() has    */
/* been called for all SC's.                                       */
/* It should call SC_SetFrequency() if controller <sc_no> doesn't  */
/* run at one controller time step per simulation second.          */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Calculate (unsigned long sc_no);

/* Executes one pass through the controller logic of SC no. <sc_no>.  */
/* This function is called from VISSIM once per SC at the end of each */
/* signal control interval, after the (detector) data for all SC's    */
/* has been passed to the controller DLL.                             */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Cleanup (unsigned long sc_no);

/* Closes the controller logic of SC number <sc_no>.             */
/* This function is called from VISSIM once per SC at the end of */
/* a simulation (or test) run.                                   */

/*--------------------------------------------------------------------------*/

void  SC_DLL_WriteSnapshotFile(unsigned long sc_no, const TCHAR * filename);

/* Writes the current internal data of SC number <sc_no> to the file */
/* <filename> in the current directory.                              */
/* This function is called from VISSIM once per SC when the user     */
/* selects Save Snapshot from the Simulation menu during a           */
/* simulation run.                                                   */

/*--------------------------------------------------------------------------*/

void  SC_DLL_ReadSnapshotFile(unsigned long sc_no, const TCHAR * filename);

/* Reads the current internal data of SC number <sc_no> from the file */
/* <filename> in the current directory.                               */
/* This function is called from VISSIM once per SC when the user      */
/* selects Load Snapshot from the Simulation menu.                    */

/*--------------------------------------------------------------------------*/

#endif /* __SC_DLL_MAIN_H */

/*==========================================================================*/
/*  End of SC_DLL_MAIN.H                                                    */
/*==========================================================================*/
