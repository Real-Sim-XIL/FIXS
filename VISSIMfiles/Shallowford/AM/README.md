 
### Shallowford_AM_calibrated_ForSignalControl_v2.inpx
 - Modify the signal head 10 and 11 to 8 and 9 for Lee Highway
 - Add queue counters

### Shallowford_AM_calibrated_ForSignalControl.inpx
 - Add delay measurements id 33-64 for bilinear control.
 - Modified the signal controllers/groups No. to 1-8.
 - Modified vehicle travel times interval to 100s; modified From time and To time of delay and vehicle travel time from 900s-4500s to 0s-99999s (need delay info for bilinear control).
 - Modified the simulation parameter “Number of runs” from 10 to 1.


### For integrated speed and signal control
 - Modify the Evaluation Configuration -> Results Attributes -> Queue Counters -> 99 - 9999 (interval 1)
 - Also, uncheck the option of "Consider adjacent lanes"



