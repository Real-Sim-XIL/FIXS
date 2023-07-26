##
## StartStop.tcl
## CarMaker 9.0 ScriptControl Example - IPG Automotive GmbH (www.ipg-automotive.com)
##
## Some meaningless statistics on lateral acceleration, steering wheel
## angle and pylon hits, just to give an idea of how the QuantAudit and
## Scratchpad commands can be used.
##
## Id

LoadTestRun "Examples/VehicleDynamics/Handling/Slalom18m"

# Set up the QuantAudit jobs. Please note that we actually have two
# different jobs on the same Quantity Car.ay. In order to be able to
# access the resulting Scratchpad notes separately, we a tag /lim.
set aylim 6.8
QuantAudit del all
QuantAudit add Car.ay -dist 0-end
QuantAudit add Car.ay/lim       -cond "abs(Car.ay) > $aylim"
QuantAudit add DM.Steer.Ang/lim -cond "abs(Car.ay) > $aylim"

# Run the simulation.
StartSim
WaitForStatus running
WaitForStatus idle

Log "Locations where lateral acceleration exceeded $aylim m/s^2:"
foreach {i sfrom sto} [Scratchpad list QuantAudit-Car.ay/lim dstart dstop] \
        {j ang}       [Scratchpad list QuantAudit-DM.Steer.Ang/lim mean] {
    Log "\t#$i: dist = $sfrom m - $sto m, Steer.Ang ~ $ang rad"
}

# In the case of Car.ay it is clear from the definition, that only a
# single note (i.e. index 0) will be generated, so we can address it directly.
set aymin [Scratchpad get QuantAudit-Car.ay 0 min]
set aymax [Scratchpad get QuantAudit-Car.ay 0 max]
Log "Lateral acceleration range: $aymin m/s^2 - $aymax m/s^2"

# Pylons automatically generate PylonHit notes when being hit by the vehicle,
# so this presents a nice opportunity to show access to non-QuantAudit notes
# as well as a different way of looping over the list of notes.
set notes [Scratchpad list PylonHit]
Log "[llength $notes] pylons were hit:"
foreach i $notes {
    set sroad [Scratchpad get PylonHit $i sRoad]
    set side  [Scratchpad get PylonHit $i side]
    set sidetxt [lindex {left right} $side]
    Log "\t#$i: sRoad = $sroad m ($sidetxt)"
}

# Try also the following commands in the ScriptControl console,
# as they are quite useful for debugging:
#   Scratchpad dump QuantAudit-Car.ay
#   Scratchpad dump PylonHit 0
