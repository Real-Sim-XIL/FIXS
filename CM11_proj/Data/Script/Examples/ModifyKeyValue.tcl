 
proc GetKeyValue {name optVal} {
	set val [KeyValue get $name]
   if {[llength $val] == 0} {
       set val [IFileRead $name $optVal]
   }
   return $val
}

proc ModSeedKey {key name args} {
    switch $key {
        TestRun {
			# sk is the SeedKey from Test Manager
            set sk [GetKeyValue VISSIM.KeySeed 0]
            # open Vissim 11 with COM
            set vissim [::tcom::ref createobject "Vissim.Vissim.2022"]
			# get path of inpx file
            set fname [IFileRead TestRun "VissimIF.FPath"]
            if { [string index $fname 1] ne ":" } {
                set replaced_string1 [string map {"/" "\\"} [pwd]]
                set replaced_string2 [string map {"/" "\\"} $fname]
                set fn2 $replaced_string1\\$replaced_string2
            } else {
                set fn2 $fn
            }
			# end: get path of inpx file
			
			# load inpx file in Vissim
            $vissim LoadNet $fn2
            
            # setting the random seed attribute
            [$vissim Simulation] AttValue RandSeed [::tcom::variant int $sk]
			
			# save inpx file with new attribute
            $vissim SaveNetAs $fn2
        }
    }
}


