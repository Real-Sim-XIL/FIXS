##
## SessionLog.tcl 
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## The example shows the functions used to write to the CarMaker 
## Session Log file. 
## Please note: without a running application, the use of these functions
## does not make sense.
##  
## SessionLogMsg  <msg>
##      <msg>      is the message to be written to the Log 
##  
## SessionLogWarn <warn_msg>
##      <warn_msg> is the warning message to send to the Log
##
## SessionLogError <err_msg> 
##      <err_msg>  is the error message to send to the Log 
##
## Id


# Write a log message
SessionLogMsg "Hello World"

# Write a warning message
SessionLogWarn "Objects are closer than they appear!"

# Write an error message
SessionLogError "Your socks don't match!"

Log "Take a look at the Session Log!"
Log "The messages should be shown within a few seconds."

