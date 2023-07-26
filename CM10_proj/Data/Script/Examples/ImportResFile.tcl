##
## ImportResFile.tcl  --  Import Results File
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## MODIFY THE SCRIPT:
## set the name of the result file to read.
##
## Id
#******************************************************************************

set file "SimOutput/<...>/<result_file_.erg>" 

ImportResFile $file {DM.Gas Vhcl.vx} Var

Log [lindex $Var(Vhcl.vx) 5]
