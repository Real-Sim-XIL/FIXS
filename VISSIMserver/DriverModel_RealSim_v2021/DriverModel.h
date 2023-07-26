/*==========================================================================*/
/*  DriverModel.h                                    DLL Module for VISSIM  */
/*                                                                          */
/*  Interface module for external driver models.                            */
/*                                                                          */
/*  Version of 2018-12-11                                   Lukas Kautzsch  */
/*==========================================================================*/

#ifndef __DRIVERMODEL_H
#define __DRIVERMODEL_H

#ifndef _CONSOLE
#include <windows.h>
#endif

/* In the creation of DriverModel.DLL all files must be compiled */
/* with the preprocessor definition DRIVERMODEL_EXPORTS.         */
/* Programs that use DriverModel.DLL must not be compiled        */
/* with that preprocessor definition.                            */

#ifdef DRIVERMODEL_EXPORTS
#define DRIVERMODEL_API extern "C" __declspec(dllexport)
#else
#define DRIVERMODEL_API extern "C" __declspec(dllimport)
#endif

/*==========================================================================*/

/* general data: */
/* (index1, index2 mostly irrelevant) */
/* #define  DRIVER_DATA_PATH                    101 */
           /* string: absolute path to the model's data files directory */
           /* !!! not used anymore by Vissim 2021 and later !!! */				  
#define  DRIVER_DATA_TIMESTEP                102
           /* double: simulation time step length [s] */
#define  DRIVER_DATA_TIME                    103
           /* double: simulation time [s] at the end of the current time step */
#define  DRIVER_DATA_PARAMETERFILE           104
           /* string: name (including absolute path) of a vehicle type's parameter file */
#define  DRIVER_DATA_STATUS                  105
           /* int:
           /* 0=OK,                                                             */
           /* 1=Info (there's some further information available),              */
           /* 2=Warning (there are warnings available),                         */
           /* 3=Error (an recoverable error occured but simulation can go on),  */
           /* 4=Heavy (an unrecoverable error occured and simulation must stop) */
           /* (used by DriverModelGetValue()!)                                  */
#define  DRIVER_DATA_STATUS_DETAILS          106
           /* string: info/warning/error message for nonzero status          */
           /* (used by DriverModelGetValue()!)                               */
           /* (is retrieved by VISSIM only if DRIVER_DATA_STATUS is nonzero) */
#define  DRIVER_DATA_USE_UDA                 107
           /* string: name of the user-defined attribute                         */
           /* index1 = ID of the user-defined attribute!                         */
           /* DriverModelSetValue() needs to return 1 if this UDA is to be used. */
#define  DRIVER_DATA_WANTS_ALL_SIGNALS       108
           /* int: flag: does driver model want states of all signals in network? */
           /*             (1 = yes, 0 = no (only those seen by vehicles)           */
#define  DRIVER_DATA_MAX_NUM_INDICES         109
           /* int: maximum number of indices accepted by DriverModelSetValue*()   */
           /*       and DriverModelGetValue()* functions. If DriverModelGetValue() */
           /*       returns zero, the maximum number is 2.                         */

/* current vehicle driver unit data (VDU to be moved next): */
/* (index1, index2 usually irrelevant) */
#define  DRIVER_DATA_VEH_ID                  201
           /* int:   vehicle number */
#define  DRIVER_DATA_VEH_LANE                202
           /* int:   current lane number (rightmost = 1) */
#define  DRIVER_DATA_VEH_ODOMETER            203
           /* double: total elapsed distance in the network [m] */
#define  DRIVER_DATA_VEH_LANE_ANGLE          204
           /* double: angle relative to the middle of the lane [rad] */
           /*         (positive = turning left)                      */
#define  DRIVER_DATA_VEH_LATERAL_POSITION    205
           /* double: distance of the front end from the middle of the lane [m] */
           /*         (positive = left of the middle, negative = right)         */
#define  DRIVER_DATA_VEH_VELOCITY            206
           /* double: current speed [m/s] */
#define  DRIVER_DATA_VEH_ACCELERATION        207
           /* double: current acceleration [m/s²] */
#define  DRIVER_DATA_VEH_LENGTH              208
           /* double: vehicle length [m] */
#define  DRIVER_DATA_VEH_WIDTH               209
           /* double: vehicle width [m] */
#define  DRIVER_DATA_VEH_WEIGHT              210
           /* double: vehicle weight [kg] */
#define  DRIVER_DATA_VEH_MAX_ACCELERATION    211
           /* double: maximum possible acceleration [m/s²] */
           /*         (depending on current speed)         */
#define  DRIVER_DATA_VEH_TURNING_INDICATOR   212
           /* int:   left = 1, right = -1, none = 0, both = 2   */
           /*         (also used by DriverModelGetValue()!)      */
           /*         (contains only the value set by the DLL!)  */
#define  DRIVER_DATA_VEH_CATEGORY            213
           /* int:   car = 1, truck = 2, bus = 3, tram = 4, */
           /*         pedestrian = 5, bike = 6               */
#define  DRIVER_DATA_VEH_PREFERRED_REL_LANE  214
           /* int:   positive = left, 0 = current lane, negative = right */
#define  DRIVER_DATA_VEH_USE_PREFERRED_LANE  215
           /* int:   0 = only preferable (e.g. European highway) */
           /*         1 = necessary (e.g. before a connector)     */
#define  DRIVER_DATA_VEH_DESIRED_VELOCITY    216
           /* double: desired speed [m/s]              */
           /*         (also used by DriverModelGetValue()!) */
#define  DRIVER_DATA_VEH_X_COORDINATE        217
           /* double: world coordinate X (vehicle front end) */
#define  DRIVER_DATA_VEH_Y_COORDINATE        218
           /* double: world coordinate Y (vehicle front end) */
#define  DRIVER_DATA_VEH_REAR_X_COORDINATE   808
           /* double: world coordinate X (vehicle rear end) */
#define  DRIVER_DATA_VEH_REAR_Y_COORDINATE   809
           /* double: world coordinate Y (vehicle rear end) */
#define  DRIVER_DATA_VEH_TYPE                219
           /* int:   vehicle type number (user defined) */
#define DRIVER_DATA_VEH_COLOR                220
           /* int:   vehicle color (32 bit ARGB value) */
           /*         (also used by DriverModelGetValue()!) */
#define DRIVER_DATA_VEH_CURRENT_LINK         221
           /* int:   current link number */
#define DRIVER_DATA_VEH_NEXT_LINKS           222
           /* int:   following link number(s) of the vehicle's route        */
           /* This message is sent from VISSIM only if DriverModelSetValue() */
           /* returned 1 for DRIVER_DATA_VEH_CURRENT_LINK.                   */
           /* It is sent once for each link in the route.                    */
#define  DRIVER_DATA_VEH_ACTIVE_LANE_CHANGE  223
           /* int:   direction of an active lane change movement */
           /*         (+1 = to the left, 0 = none, -1 = to the right) */
#define  DRIVER_DATA_VEH_REL_TARGET_LANE     224
           /* int:   target lange */
           /*         (+1 = next one left, 0 = current lane, -1 = next one right) */
#define  DRIVER_DATA_VEH_INTAC_STATE         225
           /* int:   interaction state from internal model                 */
           /*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)   */
           /*         FREE = 1, CLOSEUP = 2, FOLLOW = 3, BRAKEAX = 4,       */
           /*         BRAKEBX = 5,BRAKEZX = 6, BRAKESPW = 7, BRAKEKOOP = 8, */
           /*         PELOPS = 9,PASS = 10, SLEEP = 11, DWELL = 12          */
#define  DRIVER_DATA_VEH_INTAC_TARGET_TYPE   226
           /* int:   type of relevant interaction target from internal model */
           /*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)     */
           /*         no target = 0, real vehicle = 1, signal head = 2,       */
           /*         priority rule = 3, conflict area = 4, reduced speed     */
           /*         area = 5, stop sign = 6, parking lot = 7, PT stop = 8   */
#define  DRIVER_DATA_VEH_INTAC_TARGET_ID     227
           /* int:   number of relevant interaction target from internal model */
           /*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)       */
#define  DRIVER_DATA_VEH_INTAC_HEADWAY       228
           /* double: distance to relevant interaction target from internal model [m],  */
           /*         front bumper to front bumper, including length of leading vehicle */
           /*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)               */
#define  DRIVER_DATA_VEH_Z_COORDINATE        229
           /* double: world coordinate Z (vehicle front end)                       */
           /* Is calculated correctly only if 3D visualization or a vehicle record */
           /* containing 'Coordinate front' or 'Coordinate rear' is active.        */
#define  DRIVER_DATA_VEH_REAR_Z_COORDINATE   230
           /* double: world coordinate Z (vehicle rear end)                        */
           /* Is calculated correctly only if 3D visualization or a vehicle record */
           /* containing 'Coordinate front' or 'Coordinate rear' is active.        */
#define  DRIVER_DATA_VEH_UDA                 231
           /* double, int or string: value of the user-defined attribute    */
           /* index1 = ID of the user-defined attribute!                     */
           /* This message is sent from VISSIM only if DriverModelSetValue() */
           /* returned 1 for DRIVER_DATA_USE_UDA with same index1.           */
           /* (also used by DriverModelGetValue()!)                          */

/* nearby vehicle driver unit data: */
/* (index1 = relative lane: +2 = second to the left, +1 = next one to the left,   */
/*                           0 = current lane,                                    */
/*                          -1 = next one to the right, -2 = second to the right  */
/*           except for DRIVER_DATA_NVEH_UDA!)                                    */                        
/* (index2 = relative position: positive = downstream (+1 next, +2 second next, possibly */
/*                                         more if DRIVER_DATA_WANTS_ALL_NVEHS is set)   */
/*                              negative = upstream (-1 next, -2 second next, possibly   */
/*                                         more if DRIVER_DATA_WANTS_ALL_NVEHS is set))  */
#define  DRIVER_DATA_NVEH_ID                 301
           /* int:   vehicle number */
           /*         (negative = no vehicle at this lane/position) */
#define  DRIVER_DATA_NVEH_LANE_ANGLE         302
           /* double: angle relative to the middle of the lane [rad] */
           /*         (positive = turning left)                      */
#define  DRIVER_DATA_NVEH_LATERAL_POSITION   303
           /* double: distance of the front end from the middle of the lane [m] */
           /*         (positive = left of the middle, negative = right)         */
#define  DRIVER_DATA_NVEH_DISTANCE           304
           /* double: gross distance [m] (front end to front end) */
#define  DRIVER_DATA_NVEH_REL_VELOCITY       305
           /* double: speed difference [m/s] (veh. speed - nveh. speed) */
#define  DRIVER_DATA_NVEH_ACCELERATION       306
           /* double: current acceleration [m/s²] */
#define  DRIVER_DATA_NVEH_LENGTH             307
           /* double: vehicle length [m] */
#define  DRIVER_DATA_NVEH_WIDTH              308
           /* double: vehicle width [m] */
#define  DRIVER_DATA_NVEH_WEIGHT             309
           /* double: vehicle weight [kg] */
#define  DRIVER_DATA_NVEH_TURNING_INDICATOR  310
           /* int:   left = 1, right = -1, none = 0, both = 2                */
           /*         (contains only the value set by the DLL through         */
           /*          DriverModelGetValue(DRIVER_DATA_VEH_TURNING_INDICATOR) */
           /*          in the previous time step!)                            */
#define  DRIVER_DATA_NVEH_CATEGORY           311
           /* int:   car = 1, truck = 2, bus = 3, tram = 4, */
           /*         pedestrian = 5, bike = 6               */
#define  DRIVER_DATA_NVEH_LANE_CHANGE        312
           /* int:   direction of a current lane change              */
           /*         (+1 = to the left, 0 = none, -1 = to the right) */
#define  DRIVER_DATA_NVEH_TYPE               313
           /* int:   vehicle type number (user defined) */
#define  DRIVER_DATA_NVEH_UDA                314
           /* double, int or string: value of the user-defined attribute    */
           /* index1 = ID of the user-defined attribute!!! Not rel. lane!!!  */
           /* This message is sent from VISSIM only if DriverModelSetValue() */
           /* returned 1 for DRIVER_DATA_USE_UDA with same index1.           */
           /* It is sent after DRIVER_DATA_NVEH_ID of the same nearby        */
           /* vehicle and before the DRIVER_DATA_NVEH_ID of the next one.    */
#define  DRIVER_DATA_NVEH_X_COORDINATE       315
           /* double: world coordinate X (vehicle front end) */
#define  DRIVER_DATA_NVEH_Y_COORDINATE       316
           /* double: world coordinate Y (vehicle front end) */
#define  DRIVER_DATA_NVEH_Z_COORDINATE       317
           /* double: world coordinate Z (vehicle front end)                       */
           /* Is calculated correctly only if 3D visualization or a vehicle record */
           /* containing 'Coordinate front' or 'Coordinate rear' is active.        */
#define  DRIVER_DATA_NVEH_REAR_X_COORDINATE  318
           /* double: world coordinate X (vehicle rear end)                        */
#define  DRIVER_DATA_NVEH_REAR_Y_COORDINATE  319
           /* double: world coordinate Y (vehicle rear end)                        */
#define  DRIVER_DATA_NVEH_REAR_Z_COORDINATE  320
           /* double: world coordinate Z (vehicle rear end)                        */
           /* Is calculated correctly only if 3D visualization or a vehicle record */
           /* containing 'Coordinate front' or 'Coordinate rear' is active.        */

/* link data: */
/* (index1, index2 irrelevant) */
#define  DRIVER_DATA_NO_OF_LANES             401

/* lane data: */
/* (index1 = lane number: rightmost = 1) */
/* (index2 irrelevant) */
#define  DRIVER_DATA_LANE_WIDTH              501
           /* double: lane width [m] */
#define  DRIVER_DATA_LANE_END_DISTANCE       502
           /* double: distance to end of lane [m]               */
           /* (can be emergency stop position before connector) */
           /* (negative = no end of lane in visibility range)   */

/* current lane data: */
/* (index1, index2 irrelevant) */
#define  DRIVER_DATA_CURRENT_LANE_POLY_N  551
           /* int: number of downstream lane polygon points within         */
           /*       visibility distance along the route/path of the vehicle */
/* (index1 = point index, starting at 1) */
/* (index2 irrelevant) */
#define  DRIVER_DATA_CURRENT_LANE_POLY_X  552
           /* double: world coordinate X (polygon point, middle of the lane) */
#define  DRIVER_DATA_CURRENT_LANE_POLY_Y  553
           /* double: world coordinate Y (polygon point, middle of the lane) */
#define  DRIVER_DATA_CURRENT_LANE_POLY_Z  554
           /* double: world coordinate Z (polygon point, middle of the lane) */

/* environment data: */
/* (index1, index2 irrelevant) */
#define  DRIVER_DATA_RADIUS                  601
           /* double: current curve radius [m] */
#define  DRIVER_DATA_MIN_RADIUS              602
           /* double: minimum curve radius [m] in visibility range */
#define  DRIVER_DATA_DIST_TO_MIN_RADIUS      603
           /* double: distance [m] to spot of minimum curve radius */
#define  DRIVER_DATA_SLOPE                   604
           /* double: current slope (negative = drop) */
           /* (e.g. -0.026 = -2.6%)                   */
#define  DRIVER_DATA_SLOPE_AHEAD             605
           /* double: slope at end of visibility range */
           /* (negative = drop)                        */

/* 'traffic sign' data: */
/* (index1, index2, index3 irrelevant where not mentioned) */
#define  DRIVER_DATA_SIGNAL_DISTANCE         701
           /* double: distance [m] to next signal head */
           /* (negative = no signal head visible)      */
#define  DRIVER_DATA_SIGNAL_STATE            702
           /* int:   red = 1, amber = 2, green = 3, red/amber = 4, */
           /*         amber flashing = 5, off = 6, green arrow = 7  */
#define  DRIVER_DATA_SIGNAL_STATE_START      703
           /* double: simulation time [s] when signal changed to current state */
#define  DRIVER_DATA_SPEED_LIMIT_DISTANCE    704
           /* double: distance [m] to 'speed limit sign' */
           /*         (reduced speed area: real distance) */
           /*         (desired speed decision: 1.0 m when just passed) */
           /*         (negative: no sign visible) */
#define  DRIVER_DATA_SPEED_LIMIT_VALUE       705
           /* double: speed limit [km/h] */
           /*         (0 = end of reduced speed area) */
#define  DRIVER_DATA_PRIO_RULE_DISTANCE      706
           /* double: distance [m] to next priority rule stop line */
           /* (negative = no priority rule stop line visible)      */
#define  DRIVER_DATA_PRIO_RULE_STATE         707
           /* int:   blocked = 1, free = 3, unknown = 0  */
#define  DRIVER_DATA_ROUTE_SIGNAL_DISTANCE   708
           /* double: distance [m] to next signal head on vehicle route */
           /*         (possibly on a different lane)                    */
           /*         (negative = no signal head visible)               */
           /* index1 = signal controller number                         */
           /* index2 = signal head number                               */
#define  DRIVER_DATA_ROUTE_SIGNAL_STATE      709
           /* int: current signal state of the next signal head  */
           /*       on the current vehicle's route:               */
           /*       red = 1, amber = 2, green = 3, red/amber = 4, */
           /*       flashing amber = 5, off = 6, other = 0        */
           /* index1 = signal controller number                   */
           /* index2 = signal head number                         */
#define  DRIVER_DATA_ROUTE_SIGNAL_SWITCH     710
           /* double: time left until future signal switch of the      */
           /*         next signal head on the current vehicle's route: */
           /* int:   signal state after that switch:                  */
           /*         red = 1, amber = 2, green = 3, red/amber = 4     */
           /*         flashing amber = 5, off = 6, other = 0           */
           /* index1 = signal controller number                        */
           /* index2 = signal head number                              */
           /* index3 = index of switch (0 = next, 1 = second, ..)      */
           /* DriverModelSetValue3() can return 0 to this in order to  */
           /* skip further switch data for the current vehicle.        */
#define  DRIVER_DATA_ROUTE_SIGNAL_CYCLE      711
           /* double: cycle length [s] of the signal controller of the */
           /*         next signal head on the current vehicle's route  */
           /*         (unknown = 0)                                    */
           /* index1 = signal controller number                        */
           /* index2 = signal head number                              */
#define  DRIVER_DATA_CONFL_AREAS_COUNT       712
           /* int:   number of visible lane conflicts                 */
           /* DriverModelSetValue() can return 0 to this in order to   */
           /* skip further conflict area data for the current vehicle. */
#define  DRIVER_DATA_CONFL_AREA_TYPE         713
           /* int:   lane conflict type:                              */
           /*         0 = crossing, 1 = merging, 2 = branching         */
           /* index1 = lane conflict index [0..n-1]                    */
           /* index2 = conflict area number                            */
#define  DRIVER_DATA_CONFL_AREA_YIELD        714
           /* int:   lane conflict priority:                          */
           /*         0 = pass (green/red), 1 = yield (red/green),     */
           /*         2 = undetermined (red/red)                       */
           /* index1 = lane conflict index [0..n-1]                    */
           /* index2 = conflict area number                            */
#define  DRIVER_DATA_CONFL_AREA_DISTANCE     715
           /* double: distance [m] to end (!) of lane conflict         */
           /* index1 = lane conflict index [0..n-1]                    */
#define  DRIVER_DATA_CONFL_AREA_LENGTH       716
           /* double: length [m] of lane conflict                      */
           /* index1 = lane conflict index [0..n-1]                    */
#define  DRIVER_DATA_CONFL_AREA_VEHICLES     717
           /* int:   number of conflicting vehicles                   */
           /* index1 = lane conflict index [0..n-1]                    */
#define  DRIVER_DATA_CONFL_AREA_TIME_ENTER   718
           /* int:   vehicle number of conflicting object     */
           /*         (0 for pedestrian)                       */
           /* double: simulation time when the front end of    */
           /*         the conflicting vehicle arrives/arrived  */
           /*         in the conflict area (< 0 = never)       */
           /* index1 = lane conflict index [0..n-1]            */
           /* index2 = index of conflicting object [0..n-1]    */
#define  DRIVER_DATA_CONFL_AREA_TIME_IN      719
           /* int:   vehicle number of conflicting object     */
           /*         (0 for pedestrian)                       */
           /* double: simulation time when the rear end of the */
           /*         conflicting vehicle arrives/arrived      */
           /*         in the conflict area (< 0 = never)       */
           /*         (always < 0 for crossing conflict)       */
           /* index1 = lane conflict index [0..n-1]            */
           /* index2 = index of conflicting object [0..n-1]    */
#define  DRIVER_DATA_CONFL_AREA_TIME_EXIT    720
           /* int:   vehicle number of conflicting object     */
           /*         (0 for pedestrian)                       */
           /* double: simulation time when the rear end of the */
           /*         conflicting vehicle leaves the conflict  */
           /*         area (< 0 = never)                       */
           /* index1 = lane conflict index [0..n-1]            */
           /* index2 = index of conflicting object [0..n-1]    */

/* driving behavior data [and other data for DriverModelGetValue()]: */
/* (index1, index2 irrelevant) */
/* (must be provided by the driver model after DRIVER_COMMAND_MOVE_DRIVER)            */
/* (usually for DriverModelGetValue(), but will be set by DriverModelSetValue() as    */
/*  suggestion if DriverModelGetValue (DRIVER_DATA_WANTS_SUGGESTION, ...) delivers 1) */
#define  DRIVER_DATA_WANTS_SUGGESTION        801
           /* int:   flag: does driver model want behavior suggestion? */
           /*         (1 = yes, 0 = no) */
#define  DRIVER_DATA_DESIRED_ACCELERATION    802
           /* double: desired acceleration [m/s²] in next time step */
#define  DRIVER_DATA_DESIRED_LANE_ANGLE      803
           /* double: desired angle relative to the middle of the lane [rad] */
           /*         (positive = turning left) */
#define  DRIVER_DATA_ACTIVE_LANE_CHANGE      804
           /* int:   direction of an active lane change movement */
           /*         (+1 = to the left, 0 = none, -1 = to the right) */
           /*         (must be != 0 while lane change is not completed) */
           /*         (will be used for NVEH_LANE_CHANGE) */
#define  DRIVER_DATA_REL_TARGET_LANE         805
           /* int:   target lange */
           /*         (+1 = next one left, 0 = current lane, -1 = next one right) */
#define  DRIVER_DATA_SIMPLE_LANECHANGE       806
           /* int:   flag: does driver model want VISSIM to control the lateral  */
           /*         movement during the lane change (i.e. start lane change     */
           /*         when ACTIVE_LANE_CHANGE != 0 but ignore DESIRED_LANE_ANGLE) */
           /*         and stop the lane change after the vehicle has reached the  */
           /*         middle of the new lane?                                     */
           /*         (1 = yes, 0 = no)                                           */
#define  DRIVER_DATA_USE_INTERNAL_MODEL      807
           /* int:   flag: does driver model just want to use the internal model */
           /*         in this time step?                                          */
           /*         (1 = yes, 0 = no)                                           */
           /*         1 is only possible if DRIVER_DATA_WANTS_SUGGESTION is set!  */
/* !!! 808 and 809 are used by DRIVER_DATA_VEH_REAR_*_COORDINATE, see above !!! */
#define  DRIVER_DATA_WANTS_ALL_NVEHS         810
           /* int:   flag: does driver model want all nearby vehicles (even more than 2    */
           /*               per lane and down-/upstream if set so in the driving behavior)? */
           /*               (1 = yes, 0 = no)                                               */
#define  DRIVER_DATA_ALLOW_MULTITHREADING    811
           /* int:   flag: does driver model allow multithreaded simulation runs? */
           /*               (1 = yes, 0 = no)                                      */

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelSetValue (int   type,
                                           int   index1,
                                           int   index2,
                                           int   int_value,
                                           double double_value,
                                           char   *string_value);

/* Sets the value of a data object of type <type>, selected by <index1> */
/* and possibly <index2>, to <int_value>, <double_value> or            */
/* <*string_value> (object and value selection depending on <type>).    */
/* Return value is 1 on success, otherwise 0.                           */

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelSetValue3 (int   type,
                                            int   index1,
                                            int   index2,
                                            int   index3,
                                            int   int_value,
                                            double double_value,
                                            char   *string_value);

/* Sets the value of a data object of type <type>, selected by <index1>, */
/* <index2> and <index3>, to <int_value>, <double_value> or             */
/* <*string_value> (object and value selection depending on <type>).     */
/* Return value is 1 on success, otherwise 0.                            */
/* DriverModelGetValue (DRIVER_DATA_MAX_NUM_INDICES, ...) needs to set   */
/* *int_value to 3 or greater in order to activate this function!       */

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelGetValue (int   type,
                                           int   index1,
                                           int   index2,
                                           int   *int_value,
                                           double *double_value,
                                           char   **string_value);

/* Gets the value of a data object of type <type>, selected by <index1> */
/* and possibly <index2>, and writes that value to <*int_value>,       */
/* <*double_value> or <**string_value> (object and value selection      */
/* depending on <type>).                                                */
/* Return value is 1 on success, otherwise 0.                           */

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelGetValue3 (int   type,
                                            int   index1,
                                            int   index2,
                                            int   index3,
                                            int   *int_value,
                                            double *double_value,
                                            char   **string_value);

/* Gets the value of a data object of type <type>, selected by <index1>, */
/* <index2> and <index3>, and writes that value to <*int_value>,        */
/* <*double_value> or <**string_value> (object and value selection       */
/* depending on <type>).                                                 */
/* Return value is 1 on success, otherwise 0.                            */
/* DriverModelGetValue (DRIVER_DATA_MAX_NUM_INDICES, ...) needs to set   */
/* *int_value to 3 or greater in order to activate this function!       */

/*==========================================================================*/

#define  DRIVER_COMMAND_INIT            0
           /* called from VISSIM once at the start of a simulation run */
           /* values got before: DRIVER_DATA_STATUS               */
           /*                    DRIVER_DATA_STATUS_DETAILS       */
           /* values set before: DRIVER_DATA_PATH                 */
           /*                    DRIVER_DATA_TIMESTEP             */
           /*                    DRIVER_DATA_TIME                 */
           /*                    DRIVER_DATA_VEH_TYPE             */
           /* values got before: DRIVER_DATA_WANTS_SUGGESTION     */
           /*                    DRIVER_DATA_SIMPLE_LANECHANGE    */
           /*                    DRIVER_DATA_WANTS_ALL_NVEHS      */
           /*                    DRIVER_DATA_ALLOW_MULTITHREADING */
           /*                    DRIVER_DATA_WANTS_ALL_SIGNALS    */
           /*                    DRIVER_DATA_MAX_NUM_INDICES      */
           /* values set before: DRIVER_DATA_USE_UDA              */
           /* values got after:  DRIVER_DATA_STATUS               */

#define  DRIVER_COMMAND_CREATE_DRIVER   1
           /* called from VISSIM once per vehicle entering the network */
           /* values set before: DRIVER_DATA_VEH_ID               */
           /*                    DRIVER_DATA_VEH_DESIRED_VELOCITY */

#define  DRIVER_COMMAND_KILL_DRIVER     2
           /* called from VISSIM once per vehicle leaving the network */
           /* value set before: DRIVER_DATA_VEH_ID */

#define  DRIVER_COMMAND_MOVE_DRIVER     3
           /* called from VISSIM for each vehicle once per time step during a simulation run */
           /* values set before: all values                      */
           /*                    (driving behaviour data only if */
           /*                     DRIVER_DATA_WANTS_SUGGESTION)  */

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelExecuteCommand (int number);

/* Executes the command <number> if that is available in the driver */
/* module. Return value is 1 on success, otherwise 0.               */

/*==========================================================================*/

#endif /* __DRIVERMODEL_H */

/*==========================================================================*/
/*  End of DriverModel.h                                                    */
/*==========================================================================*/
