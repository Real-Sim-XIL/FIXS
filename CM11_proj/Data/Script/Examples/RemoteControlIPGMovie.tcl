### Helper functions
proc IPGMovie_Count {} {
    set Number [llength [WInfoInterps "*MOVIE"]]
    return $Number
}


### Main functions to be called from the simulation
SigHandler SetMovie_CreateWindow {args} {
    if {[IPGMovie_Count] <= 0} {
        Movie start
    }
    Movie window create {*}$args
}


# close Movie
SigHandler SetMovie_CloseWindow {args} {
    if {[IPGMovie_Count] > 0} {
	Movie window delete {*}$args
    }
}


# e.g. for TestRun ActiveLights
SigHandler SetMovie_Background {backgr} {
    if {[IPGMovie_Count] <= 0} {
        Movie start
    }
    Movie background select $backgr
}



SigHandler SetMovie_Camera {args} {
    if {[IPGMovie_Count] <= 0} {
        Movie start
    }
    Movie camera select {*}$args
}


