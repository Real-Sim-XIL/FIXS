#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
This is the implementation of vehicle-based speed control algorithm, could be deployed on vehicle or mobile app.
"""
import pandas as pd
import numpy as np
import time
import itertools


def queue_update_acc(radar_detection_range, queue_len_per_vehicle, queue_length_w1, distance2stopbar, vehicle_current_speed, Speed_base, dist, SigState):
    """

    :param queue_length_w1:
    :param distance2stopbar:
    :param Speed_base:
    :param dist:
    :param SigState:
    :return:
    """
    if np.isnan(dist):
        d_front = 0
    else:
        d_front = distance2stopbar - dist - 15

    if d_front <= 0:
        queue_length_w1 = 0
    else:
        if distance2stopbar < radar_detection_range:
            if np.isnan(Speed_base):
                queue_length_w1 = 0
            elif 'green' in SigState:
                if Speed_base > 3.1: # two possible situations: arrive at intersection and departure intersection, it's hard to distinguish.
                    queue_length_w1 = np.minimum(queue_length_w1, d_front)
                else:
                    queue_length_w1 = d_front + 15
            elif 'red' in SigState:
                if Speed_base <= 3.1:
                    queue_length_w1 = d_front + 15
                else:
                    queue_length_w1 = np.minimum(queue_length_w1, d_front)

        else:
            if np.isnan(Speed_base):
                queue_length_w1 = np.minimum(queue_length_w1, distance2stopbar - radar_detection_range)
            elif Speed_base <= 3.1:
                queue_length_w1 = d_front
            else:
                queue_length_w1 = np.minimum(queue_length_w1, d_front)

        # if the ego vehicle is in the queue
        if 3.1 >= vehicle_current_speed > 0:
            queue_length_w1 = np.maximum(np.minimum(queue_length_w1, d_front + 15), 0)

    return queue_length_w1


def red_window_adjust(r1s, r1e, r2s, r2e, distance2stopbar, orginal_desire_spd, queue_discharge_speed, queue_length_w1, queue_length_w2):

    r1s_adjust = r1s - distance2stopbar/(orginal_desire_spd * 1.46667)
    r1e_adjust = r1e + queue_length_w1/queue_discharge_speed - (distance2stopbar - queue_length_w1)/(orginal_desire_spd * 1.46667)
    r2s_adjust = r2s - distance2stopbar/(orginal_desire_spd * 1.46667)
    r2e_adjust = r2e + queue_length_w2/queue_discharge_speed - (distance2stopbar - queue_length_w2)/(orginal_desire_spd * 1.46667)

    return r1s_adjust, r1e_adjust, r2s_adjust, r2e_adjust


def queue_update_adjust_red_window(r1s, r1e, r2s, r2e, distance2stopbar, orginal_desire_spd, queue_discharge_speed, queue_length_w1, queue_length_w2, maximum_dischargeable_queue_w1, simsec):

    # calculate the estimated two red window
    r1s_adjust, r1e_adjust, r2s_adjust, r2e_adjust = red_window_adjust(r1s, r1e, r2s, r2e, distance2stopbar, orginal_desire_spd, queue_discharge_speed, queue_length_w1, queue_length_w2)

    if r2s_adjust < r1e_adjust or queue_length_w1 > maximum_dischargeable_queue_w1:  # oversaturated
        queue_length_w2 = queue_length_w2 + queue_length_w1 - maximum_dischargeable_queue_w1
        r1s_adjust, r1e_adjust, r2s_adjust, r2e_adjust = red_window_adjust(r1s, r1e, r2s, r2e, distance2stopbar,
                                                                           orginal_desire_spd, queue_discharge_speed, queue_length_w1,
                                                                           queue_length_w2)
        if simsec > r2e_adjust:
            queue_length_w2 = 0
            queue_length_w1 = 0
    else:
        if (r1e_adjust + 3) < simsec < (r2s_adjust - 3):
            queue_length_w1 = 0
        elif simsec > r2e_adjust:
            queue_length_w2 = 0
            queue_length_w1 = 0

    return queue_length_w1, queue_length_w2


def green_window_adjust(queue_discharge_speed, departure_speed, queue_length_w1, queue_length_w2, t1s, t1e, t2s, t2e):
    """

    :param queue_length_w1:
    :param queue_length_w2:
    :param green_window:
    :return:
    """
    green_window_start1 = queue_length_w1 / queue_discharge_speed + t1s  # add 2 seconds buffer time
    green_window_end1 = t1e - queue_length_w1 / departure_speed - 1

    green_window_start2 = queue_length_w2 / queue_discharge_speed + t2s  # add 2 seconds buffer time
    green_window_end2 = t2e - queue_length_w2 / departure_speed - 1

    return green_window_start1, green_window_end1, green_window_start2, green_window_end2


def desire_speed4green_window_w_downstream(simsec, orginal_desire_spd, distance2stopbar, queue_length_w1,
                                           queue_length_w2, green_window_start1, green_window_end1,
                                           green_window_start2, green_window_end2, t1s,
                                           distance2stopbar_next, t21s, t21e, t22s, t22e):
    """

    :param simsec:
    :param orginal_desire_spd:
    :param queue_length_w1:
    :param queue_length_w2:
    :param green_window_start1:
    :param green_window_end1:
    :param green_window_start2:
    :param green_window_end2:
    :return:
    """
    # address the outlier that the distance d is less than the current queue length, which results in negative value
    if simsec >= green_window_start1 or distance2stopbar < queue_length_w1:
        v1s = float('inf')
    else:
        v1s = ((distance2stopbar - queue_length_w1) / (green_window_start1 - simsec)) * 0.681818  # ft/s to mph

    if simsec >= green_window_end1 or distance2stopbar < queue_length_w1:
        v1e = float('inf')
    else:
        v1e = ((distance2stopbar - queue_length_w1) / (green_window_end1 - simsec)) * 0.681818  # ft/s to mph

    # in some cases, green_window - simsec is too small, which will generate large value
    if v1s > orginal_desire_spd:
        v1s = float('inf')
    if v1e > orginal_desire_spd:
        v1e = float('inf')

    # this will work for both undersaturated and oversaturated cases
    v2s = ((distance2stopbar - queue_length_w2) / (green_window_start2 - simsec)) * 0.681818  # ft/s to mph
    v2e = ((distance2stopbar - queue_length_w2) / (green_window_end2 - simsec)) * 0.681818  # ft/s to mph

    # generate updated v1s, v1e, v2s, v2e by considering two downstream intersections
    # print(t21s)
    if distance2stopbar_next is None or t21s == '':
        v1s_ = v1s
        v1e_ = v1e
        v2s_ = v2s
        v2e_ = v2e

    elif (distance2stopbar_next - distance2stopbar) <= 820:  # 250 meter
        # calculate the desired space mean speed for the second intersection
        if simsec >= t21s:
            v21s = float('inf')
        else:
            v21s = (distance2stopbar_next / (t21s - simsec)) * 0.681818

        v21e = (distance2stopbar_next / max((t21e - simsec),1) ) * 0.681818  # ft/s to mph
        v22s = (distance2stopbar_next / (t22s - simsec)) * 0.681818  # ft/s to mph
        v22e = (distance2stopbar_next / max((t22e - simsec),1) ) * 0.681818  # ft/s to mph

        # in some cases, green_window - simsec is too small, which will generate large value
        if v21s > orginal_desire_spd:
            v21s = float('inf')
        if v21e > orginal_desire_spd:
            v21e = float('inf')

        if np.minimum(v1s, v21s) > np.maximum(v1e, v21e):
            v1s_ = np.minimum(v1s, v21s)
            v1e_ = np.maximum(v1e, v21e)
        elif np.minimum(v1s, v22s) > np.maximum(v1e, v22e):
            v1s_ = np.minimum(v1s, v22s)
            v1e_ = np.maximum(v1e, v22e)
        else:
            v1s_ = v1s
            v1e_ = v1e

        if np.minimum(v2s, v22s) > np.maximum(v2e, v22e):
            v2s_ = np.minimum(v2s, v22s)
            v2e_ = np.maximum(v2e, v22e)
        else:
            v2s_ = v2s
            v2e_ = v2e

    else:
        v1s_ = v1s
        v1e_ = v1e
        v2s_ = v2s
        v2e_ = v2e

    # determine the control scenario (1: stop, 2: slow down, 3: constant speed, 4: speed up)
    if orginal_desire_spd >= v1s_:
        desired_speed = v1s_
        minimum_desired_speed = v1e_
        queue_coast = queue_length_w1
        green_window4coast = green_window_start1
    elif orginal_desire_spd >= v1e_:
        desired_speed = orginal_desire_spd
        minimum_desired_speed = v1e_
        queue_coast = 0
        green_window4coast = t1s
    elif orginal_desire_spd >= v2s_:
        desired_speed = v2s_
        minimum_desired_speed = v2e_
        queue_coast = queue_length_w2
        green_window4coast = green_window_start2
    else:
        desired_speed = orginal_desire_spd
        minimum_desired_speed = v2e_
        queue_coast = 0
        green_window4coast = green_window_start2

    return desired_speed, minimum_desired_speed, queue_coast, green_window4coast


def cal_coasting_dist(v1, v2, A, B, C, M):
    """
    v1 is greater than v2, the unit for speed is m/s
    :param v1:
    :param v2:
    :return: unit-feet
    """
    # −𝑀/2𝐶(ln⁡(𝐴+𝐵×𝑣+𝐶×𝑣^2 ) |𝑣_2¦𝑣_1 ┤−  𝐵/(𝐶√(𝐴/𝐶−𝐵^2/〖4𝐶〗^2 ))  tan^(−1)⁡〖((𝑣+ 𝐵/2𝐶)/√(𝐴/𝐶−𝐵^2/〖4𝐶〗^2 ))〗 |𝑣_2¦𝑣_1 ┤)
    part1 = (np.log(A + B * v1 + C * (v1**2)) - np.log(A + B * v2 + C * (v2**2)))
    part2 = (B/(C * np.sqrt((A/C) - (B**2)/(4 * (C**2))))) * np.arctan((v1 + B/(2*C))/np.sqrt((A/C) - (B**2)/(4 * (C**2))))
    part3 = (B/(C * np.sqrt((A/C) - (B**2)/(4 * (C**2))))) * np.arctan((v2 + B/(2*C))/np.sqrt((A/C) - (B**2)/(4 * (C**2))))

    coasting_dist = 3.28084 * ((M/(2*C)) * (part1 - part2 + part3))  # meter to feet

    return coasting_dist


def cal_coasting_time(v1, v2, A, B, C, M):
    """
    v1 is greater than v2, the unit for speed is m/s
    :param v1:
    :param v2:
    :return: second
    """
    part1 = (M/(C * np.sqrt((A/C) - (B**2)/(4 * (C**2))))) * np.arctan((v1 + B/(2*C))/np.sqrt((A/C) - (B**2)/(4 * (C**2))))
    part2 = (M/(C * np.sqrt((A/C) - (B**2)/(4 * (C**2))))) * np.arctan((v2 + B/(2*C))/np.sqrt((A/C) - (B**2)/(4 * (C**2))))

    coasting_time = part1 - part2

    return coasting_time


def estimate_coasting_speed(v1, dist, example_coasting_profile):
    test = example_coasting_profile[example_coasting_profile['v_instant'] <= v1].reset_index(drop=True)
    test = test[test['dist_2'] <= (test.dist_2.min() + dist)]

    return test.v_instant.min(), len(test)


def braking_tractive_energy(A, B, C, M, v_start, v_end, braking_a, regen):
    # braking_time = (v_start * 0.44704 - v_end * 0.44704)/braking_a
    # v_end = round(v_start,1) - braking_a * round(braking_time,1) * 2.23694
    if regen == True:
        df_energy = pd.DataFrame({'speed': np.arange(round(v_start,3), round(v_end,3), -round(braking_a * 0.1 * 2.23694,3))})

        df_energy['trac_power'] = A * (df_energy['speed'] * 0.44704) + \
                                  B * np.power((df_energy['speed'] * 0.44704), 2) + \
                                  C * np.power((df_energy['speed'] * 0.44704), 3) - M * braking_a * df_energy['speed'] * 0.44704

        df_energy['trac_power_regen'] = np.where(df_energy['trac_power'] < 0, df_energy['trac_power'] * 0.7, df_energy['trac_power'])
        df_energy['trac_power_regen'] = df_energy['trac_power_regen'] * 0.1

        trac_energy_consumption = df_energy['trac_power_regen'].sum()
    else:
        trac_energy_consumption = 0

    kinetic_energy_origin = 0.5 * M * np.power(v_start * 0.44704, 2)
    kinetic_energy_dest = 0.5 * M * np.power(v_end * 0.44704, 2)

    total_energy_consumption = trac_energy_consumption + kinetic_energy_origin - kinetic_energy_dest

    return total_energy_consumption


def cruising_tractive_energy(A, B, C, M, v_cruise, cruising_time):
    v_cruise = v_cruise * 0.44704  # mph to m/s
    trac_power = A * v_cruise + B * np.power(v_cruise, 2) + C * np.power(v_cruise, 3)

    return trac_power * cruising_time


def coasting_energy(M, v_start, v_end):
    return 0.5 * M * np.power(v_start * 0.44704, 2) - 0.5 * M * np.power(v_end * 0.44704, 2)


def tract_enenrgy_integration(A, B, C, M, a_comfort, v):
    integration = 0.5 * (A + M * a_comfort) * np.power((v * 0.44704), 2) \
                  + (1/3) * B * np.power((v * 0.44704), 3) + 0.25 * C * np.power((v * 0.44704), 4)

    return integration

def departure_energy(A, B, C, M, v_start, v_desire, a_comfort):
    # df_energy = pd.DataFrame({'speed': np.arange(round(v_start, 3), round(v_desire, 3), round(a_comfort * 0.1 * 2.23694, 3))})
    #
    # df_energy['trac_power'] = A * (df_energy['speed'] * 0.44704) + \
    #                           B * np.power((df_energy['speed'] * 0.44704), 2) + \
    #                           C * np.power((df_energy['speed'] * 0.44704), 3) + M * a_comfort * df_energy['speed'] * 0.44704
    #
    # df_energy['trac_power'] = df_energy['trac_power'] * 0.1
    #
    # trac_energy_consumption = df_energy['trac_power'].sum()

    # integration method
    trac_energy_consumption_1 = (1/a_comfort) * (tract_enenrgy_integration(A, B, C, M, a_comfort, v_desire) - tract_enenrgy_integration(A, B, C, M, a_comfort, v_start))

    return trac_energy_consumption_1


def coasting_numeric_relax_sol(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, minimum_control_speed,
                         green_window4coast, simsec, A, B, C, M, maximum_deceleration):

    if vehicle_current_speed > minimum_control_speed:
        df = pd.DataFrame(np.arange(0, desired_speed, 0.1), columns=['v_end'])
        df['cruising_time'] = (distance2stopbar - queue_coast + ((df['v_end'] * 0.44704) ** 2 / (2 * a_comfort)) * 3.28084 - \
                           cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/(vehicle_current_speed * 1.46667)
        df['space_mean_speed'] = 0.681818 * (((vehicle_current_speed * 1.46667) * df['cruising_time'] + cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/
                                             (df['cruising_time'] + cal_coasting_time(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M)))
        df = df[(df['space_mean_speed'] >= minimum_desired_speed) & (df['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

        if len(df) > 0 and df.cruising_time.max() > 5: # cruising + coasting
            # select the smallest error that is greater than 0
            # min_dist_error = df.dist_error.min()
            # v_end_sol = df.loc[df.dist_error == min_dist_error, 'v_end'].values[0]
            braking_a = 0
            # t_sol = df.loc[df.dist_error == min_dist_error, 'cruising_time'].values[0]
            mode = 'Cruising'
        elif len(df) > 0 and df.cruising_time.max() >= -1:
            braking_a = 0
            mode = 'Coasting'

        else: # the first trajectory is too fast, need to be braking + cruising + coasting
            # print('braking + cruising + coasting')
            # get all possible combinations of v_0 and v_end
            df_2 = pd.DataFrame(list(itertools.product(np.arange(minimum_control_speed, vehicle_current_speed, 0.1), np.arange(0, desired_speed, 0.1))), columns=['v_0', 'v_end'])
            df_2 = df_2[df_2['v_0'] > df_2['v_end']].reset_index(drop=True)
            # braking_a's unit is m/s2
            df_2['braking_a'] = ((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(0.3048 * 2 *
                                                                                                  (distance2stopbar - queue_coast +
                                                                                                   (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 -
                                                                                                   cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))
            df_2['space_mean_speed'] = 0.681818 * (((((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['braking_a'])) * 3.28084 + cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M))/
                                                   ((vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704)/df_2['braking_a'] + cal_coasting_time(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

            # satisfy the constraints for braking + cruising + coasting
            df_2 = df_2[(df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed) &
                        (df_2['braking_a'] <= maximum_deceleration) &
                        (df_2['braking_a'] > 0)].reset_index(drop=True)

            if len(df_2) > 0:
                # calculate the tractive energy for all available trajectory
                df_2['braking_energy'] = np.vectorize(braking_tractive_energy)(A, B, C, M, vehicle_current_speed, df_2['v_0'], df_2['braking_a'], False)
                df_2['coasting_energy'] = np.vectorize(coasting_energy)(M, df_2['v_0'], df_2['v_end'])
                df_2['total_energy'] = df_2['braking_energy'] + df_2['coasting_energy']

                df_2 = df_2.loc[df_2.total_energy == df_2.total_energy.min()]
                v_end_sol = df_2.v_end.max()
                braking_a = df_2.loc[df_2.v_end == v_end_sol, 'braking_a'].values[0]
                # t_sol = df_2.loc[df_2.v_end == v_end_sol, 'cruising_time'].values[0]
                mode = 'Braking'

            else: # braking_a is out of bound, which means that we cannot make it, we have to gave it back to VISSIM control
                # v_end_sol = 0
                braking_a = 0
                # t_sol = 0
                mode = 'VISSIM_Coast'

    else: # coasting
        df = pd.DataFrame(np.arange(0, desired_speed, 0.1), columns=['v_end'])
        df['v_end_feet'] = df['v_end'] * 1.46667
        df['dist_error'] = distance2stopbar - queue_coast + (df['v_end_feet'] ** 2 / (2 * a_comfort * 3.28084)) - \
                                 cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M)

        df['space_mean_speed'] = 0.681818 * (cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M)/cal_coasting_time(
            vehicle_current_speed * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M))
        df = df[(df['dist_error'] >= 0) & (df['dist_error'] <= 100) & (df['space_mean_speed'] <= desired_speed) & (df['space_mean_speed'] >= minimum_desired_speed)]

        if len(df) > 0:
            # select the smallest error that is greater than 0
            # min_dist_error = df.dist_error.min()
            # v_end_sol = df.loc[df.dist_error == min_dist_error, 'v_end'].values[0]
            braking_a = 0
            mode = 'Coasting'
        else:
            braking_a = 0
            mode = 'VISSIM_Coast'

    return mode, braking_a


def minimum_space_mean_spd_case3(vehicle_current_speed, distance2stopbar, queue_coast, example_coasting_profile):
    v_0, t_0 = estimate_coasting_speed(vehicle_current_speed, distance2stopbar - queue_coast, example_coasting_profile)
    minimum_space_mean_spd = ((distance2stopbar - queue_coast)/(t_0)) * 0.681818

    return minimum_space_mean_spd


def minimum_space_mean_spd_case4(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed, maximum_deceleration, example_coasting_profile):
    t_1 = ((vehicle_current_speed - minimum_control_speed) * 0.44704)/maximum_deceleration
    d_1 = (((vehicle_current_speed * 0.44704) ** 2 - (minimum_control_speed * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084

    if d_1 < (distance2stopbar - queue_coast):
        v_2, t_2 = estimate_coasting_speed(minimum_control_speed, distance2stopbar - queue_coast - d_1, example_coasting_profile)
        minimum_space_mean_spd = ((distance2stopbar - queue_coast)/(t_1 + t_2)) * 0.681818
    else:
        v_2 = np.sqrt((vehicle_current_speed * 0.44704) ** 2 - 2 * maximum_deceleration * (distance2stopbar - queue_coast) * 0.3048) * 2.23694
        minimum_space_mean_spd = (vehicle_current_speed + v_2)/2

    return minimum_space_mean_spd


def coasting_numeric_relax_sol_r(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, minimum_control_speed,
                         green_window4coast, simsec, A, B, C, M, maximum_deceleration, example_coasting_profile):

    if vehicle_current_speed > minimum_control_speed:
        # calculate the minimum feasible desired speed to reduce the computation load using lookup table
        minimum_space_mean_spd_3 = minimum_space_mean_spd_case3(vehicle_current_speed, distance2stopbar, queue_coast, example_coasting_profile)
        minimum_space_mean_spd_4 = minimum_space_mean_spd_case4(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed, maximum_deceleration, example_coasting_profile)

        if desired_speed < minimum_space_mean_spd_4:
            braking_a = maximum_deceleration
            mode = 'Braking'

        elif desired_speed < minimum_space_mean_spd_3:
            # print('braking + coasting')
            # get all possible combinations of v_0 and v_end
            df_2 = pd.DataFrame(list(itertools.product(np.arange(minimum_control_speed, vehicle_current_speed, 0.2), np.arange(0, desired_speed, 0.2))), columns=['v_0', 'v_end'])
            df_2 = df_2[df_2['v_0'] > df_2['v_end']].reset_index(drop=True)
            # braking_a's unit is m/s2
            df_2['braking_a'] = ((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(0.3048 * 2 *
                                                                                                  (distance2stopbar - queue_coast +
                                                                                                   (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 -
                                                                                                   cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))
            df_2['space_mean_speed'] = 0.681818 * (((((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['braking_a'])) * 3.28084 + cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M))/
                                                   ((vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704)/df_2['braking_a'] + cal_coasting_time(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

            # satisfy the constraints for braking + cruising + coasting
            df_2 = df_2[(df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed) &
                        (df_2['braking_a'] <= 4.572) &
                        (df_2['braking_a'] > 0)].reset_index(drop=True)

            if len(df_2) > 0:
                # calculate the tractive energy for all available trajectory
                df_2['total_energy'] = np.vectorize(departure_energy)(A, B, C, M, df_2['v_end'], 50, a_comfort)

                df_2 = df_2.loc[df_2.total_energy <= (df_2.total_energy.min() + df_2.total_energy.min() * 0.01)].reset_index(drop=True)
                braking_a = df_2.loc[df_2.space_mean_speed == df_2.space_mean_speed.max(), 'braking_a'].values[0]
                mode = 'Braking'

            else: # braking_a is out of bound, which means that we cannot make it, we have to gave it back to VISSIM control
                # decelerate + coast to stop
                braking_a = maximum_deceleration
                mode = 'Braking'
        else:
            df = pd.DataFrame(np.arange(0, desired_speed, 0.2), columns=['v_end'])
            df['cruising_time'] = (distance2stopbar - queue_coast + ((df['v_end'] * 0.44704) ** 2 / (2 * a_comfort)) * 3.28084 - \
                               cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/(vehicle_current_speed * 1.46667)
            df['space_mean_speed'] = 0.681818 * (((vehicle_current_speed * 1.46667) * df['cruising_time'] + cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/
                                                 (df['cruising_time'] + cal_coasting_time(vehicle_current_speed * 0.44704, df['v_end'] * 0.44704, A, B, C, M)))
            df = df[(df['space_mean_speed'] >= minimum_desired_speed)
                    & (df['space_mean_speed'] <= desired_speed)
                    & (df['cruising_time'] >= -1)].reset_index(drop=True)

            if len(df) > 0:
                if df.cruising_time.max() > 1: # cruising + coasting
                    # select the smallest error that is greater than 0
                    # min_dist_error = df.dist_error.min()
                    # v_end_sol = df.loc[df.dist_error == min_dist_error, 'v_end'].values[0]
                    braking_a = None
                    # t_sol = df.loc[df.dist_error == min_dist_error, 'cruising_time'].values[0]
                    mode = 'Cruising'
                else:
                    braking_a = None
                    mode = 'Coasting'

            else: # the first trajectory is too fast, need to be braking + coasting
                # print('braking + coasting')
                # get all possible combinations of v_0 and v_end
                df_2 = pd.DataFrame(list(itertools.product(np.arange(minimum_control_speed, vehicle_current_speed, 0.2), np.arange(0, desired_speed, 0.2))), columns=['v_0', 'v_end'])
                df_2 = df_2[df_2['v_0'] > df_2['v_end']].reset_index(drop=True)
                # braking_a's unit is m/s2
                df_2['braking_a'] = ((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(0.3048 * 2 *
                                                                                                      (distance2stopbar - queue_coast +
                                                                                                       (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 -
                                                                                                       cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))
                df_2['space_mean_speed'] = 0.681818 * (((((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['braking_a'])) * 3.28084 + cal_coasting_dist(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M))/
                                                       ((vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704)/df_2['braking_a'] + cal_coasting_time(df_2['v_0'] * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

                # satisfy the constraints for braking + cruising + coasting
                df_2 = df_2[(df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed) &
                            (df_2['braking_a'] <= 4.572) &
                            (df_2['braking_a'] > 0)].reset_index(drop=True)

                if len(df_2) > 0:
                    # calculate the tractive energy for all available trajectory
                    # df_2['braking_energy'] = np.vectorize(braking_tractive_energy)(A, B, C, M, vehicle_current_speed, df_2['v_0'], df_2['braking_a'], False)
                    # df_2['coasting_energy'] = np.vectorize(coasting_energy)(M, df_2['v_0'], df_2['v_end'])
                    # df_2['total_energy'] = df_2['braking_energy'] + df_2['coasting_energy']
                    df_2['total_energy'] = np.vectorize(departure_energy)(A, B, C, M, df_2['v_end'], 50, a_comfort)

                    df_2 = df_2.loc[df_2.total_energy <= (df_2.total_energy.min() + df_2.total_energy.min() * 0.01)].reset_index(drop=True)
                    braking_a = df_2.loc[df_2.space_mean_speed == df_2.space_mean_speed.max(), 'braking_a'].values[0]

                    # t_sol = df_2.loc[df_2.v_end == v_end_sol, 'cruising_time'].values[0]
                    mode = 'Braking'

                else: # braking_a is out of bound, which means that we cannot make it, we have to gave it back to VISSIM control
                    # decelerate + coast to stop
                    # v_end_sol = 0
                    braking_a = maximum_deceleration
                    # t_sol = 0
                    # mode = 'Coasting'
                    mode = 'Braking'

    else: # coasting through the green light (Braking + Cruising + Coasting)
        # calculate the minimum feasible desired speed to reduce the computation load using lookup table
        minimum_space_mean_spd_5 = minimum_space_mean_spd_case4(vehicle_current_speed, distance2stopbar, queue_coast, desired_speed, maximum_deceleration, example_coasting_profile)
        a_min = ((vehicle_current_speed * 0.44704) ** 2 - (desired_speed * 0.44704) ** 2)/(2 * (distance2stopbar - queue_coast) * 0.3048)

        if a_min > 4.572:
            braking_a = a_min
            mode = 'AEB'

        elif desired_speed < minimum_space_mean_spd_5: # braking to stop
            df_2 = pd.DataFrame(np.arange(0, vehicle_current_speed, 0.2), columns=['v_0'])
            df_2['braking_a'] = ((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(0.3048 * 2 * (distance2stopbar - queue_coast - cal_coasting_dist(df_2['v_0'] * 0.44704, 0, A, B, C, M)))
            df_2['braking_time'] = ((vehicle_current_speed * 0.44704) - (df_2['v_0'] * 0.44704))/df_2['braking_a']
            df_2['time_ahead'] = green_window4coast - simsec - df_2['braking_time'] - cal_coasting_time(df_2['v_0'] * 0.44704, 0, A, B, C, M)
            df_2['coasting_time'] = cal_coasting_time(df_2['v_0'] * 0.44704, 0, A, B, C, M)
            # satisfy the constraints for braking + coasting to stop
            # (Many safety experts use 15 ft/sec2 (0.47 g's) as the maximum deceleration that is safe for the average driver to maintain control,)
            df_2 = df_2[(df_2['braking_a'] <= 4.572) & (df_2['braking_a'] > 0) & (df_2['braking_time'] >= 1) & (df_2['time_ahead'] >= 0)].reset_index(drop=True)

            if len(df_2) > 0:
                braking_a = df_2.loc[df_2.braking_time == df_2.braking_time.min(), 'braking_a'].values[0]
                # braking_a = df_2.braking_a.min()
                mode = 'Braking'

            # elif len(df_2) > 0 and df_2.braking_time.min() < 1:
            #     braking_a = None
            #     mode = 'Coasting'
            else:
                braking_a = a_min
                mode = 'AEB'

        else: # Braking + Cruising + Coasting
            df = pd.DataFrame(list(itertools.product(np.arange(desired_speed, vehicle_current_speed, 0.2), np.arange(0, desired_speed, 0.2))), columns=['v_0', 'v_end'])
            df = df[df['v_0'] > df['v_end']].reset_index(drop=True)

            df['v_end_feet'] = df['v_end'] * 1.46667
            df['deceleration_dist'] = (((vehicle_current_speed * 0.44704) ** 2 - (df['v_0'] * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084
            df['deceleration_time'] = ((vehicle_current_speed * 0.44704) - (df['v_0'] * 0.44704))/maximum_deceleration

            df['cruising_time'] = (distance2stopbar - queue_coast + ((df['v_end'] * 0.44704) ** 2 / (2 * a_comfort)) * 3.28084 - \
                               cal_coasting_dist(df['v_0'] * 0.44704, df['v_end'] * 0.44704, A, B, C, M) - df['deceleration_dist'])/(df['v_0'] * 1.46667)

            # df['dist_error'] = distance2stopbar - queue_coast + (df['v_end_feet'] ** 2 / (2 * a_comfort * 3.28084)) - \
            #                          cal_coasting_dist(df['v_0'] * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M) - df['deceleration_dist']

            df['space_mean_speed'] = 0.681818 * ((df['deceleration_dist'] + df['cruising_time'] * df['v_0'] * 1.46667 + cal_coasting_dist(df['v_0'] * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M))/
                                                 (df['deceleration_time'] + df['cruising_time'] + cal_coasting_time(df['v_0'] * 0.44704, df['v_end_feet'] * 0.3048, A, B, C, M)))

            df = df[(df['cruising_time'] >= 0) & (df['space_mean_speed'] <= desired_speed) & (df['space_mean_speed'] >= minimum_desired_speed)]

            if len(df) > 0:
                df['cruising_energy'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, df['v_0'], df['cruising_time'])
                df['departure_energy'] = np.vectorize(departure_energy)(A, B, C, M, df['v_end'], 50, a_comfort)

                df['total_energy'] = df['cruising_energy'] + df['departure_energy']

                df = df.loc[df.total_energy <= (df.total_energy.min() + df.total_energy.min() * 0.01)].reset_index(drop=True)

                deceleration_time = df.loc[df.space_mean_speed == df.space_mean_speed.max(), 'deceleration_time'].values[0]
                cruising_time = df.loc[df.space_mean_speed == df.space_mean_speed.max(), 'cruising_time'].values[0]
                v_0 = df.loc[df.space_mean_speed == df.space_mean_speed.max(), 'v_0'].values[0]

                if deceleration_time >= 1:
                    braking_a = maximum_deceleration
                    mode = 'Braking'
                elif vehicle_current_speed * 0.44704 - v_0 * 0.44704 >= 0.5:
                    braking_a = vehicle_current_speed * 0.44704 - v_0 * 0.44704
                    mode = 'Braking'
                elif cruising_time >= 1:
                    braking_a = None
                    mode = 'Cruising'
                else:
                    braking_a = None
                    mode = 'Coasting'

            else: # braking + Coasting to stop at red light.
                # braking_a = None
                df_2 = pd.DataFrame(np.arange(0, vehicle_current_speed, 0.2), columns=['v_0'])
                df_2['braking_a'] = ((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(0.3048 * 2 * (distance2stopbar - queue_coast - cal_coasting_dist(df_2['v_0'] * 0.44704, 0, A, B, C, M)))
                df_2['braking_time'] = ((vehicle_current_speed * 0.44704) - (df_2['v_0'] * 0.44704))/df_2['braking_a']
                df_2['time_ahead'] = green_window4coast - simsec - df_2['braking_time'] - cal_coasting_time(df_2['v_0'] * 0.44704, 0, A, B, C, M)

                # satisfy the constraints for braking + coasting to stop
                # (Many safety experts use 15 ft/sec2 (0.47 g's) as the maximum deceleration that is safe for the average driver to maintain control,)
                df_2 = df_2[(df_2['braking_a'] <= 4.572) &
                            (df_2['braking_a'] > 0) & (df_2['time_ahead'] >= 0) & (df_2['braking_time'] >= 1)].reset_index(drop=True)

                if len(df_2) > 0:
                    braking_a = df_2.loc[df_2.braking_time == df_2.braking_time.min(), 'braking_a'].values[0]
                    # braking_a = df_2.braking_a.min()
                    mode = 'Braking'
                # elif len(df_2) > 0 and df_2.braking_time.min() < 1:
                #     braking_a = None
                #     mode = 'Coasting'

                else:
                    braking_a = a_min
                    mode = 'AEB'

    return mode, braking_a


def coasting_numeric_relax_sol_700m(dist_threshold4speed_limit_2, distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, minimum_control_speed_1, minimum_control_speed_2,
                         green_window4coast, simsec, A, B, C, M, maximum_deceleration):

    if vehicle_current_speed > minimum_control_speed_1:
        df = pd.DataFrame(list(itertools.product(np.arange(minimum_control_speed_1, vehicle_current_speed, 0.1),
                                                   np.arange(0, np.minimum(desired_speed, minimum_control_speed_2),
                                                             0.1))), columns=['v_0', 'v_end'])
        df['t1'] = (distance2stopbar - queue_coast - dist_threshold4speed_limit_2 - cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_0'] * 0.44704, A, B, C, M))/(vehicle_current_speed * 1.46667)
        df['t2'] = (dist_threshold4speed_limit_2 + (((df['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 - \
                   (((df['v_0'] * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084 - \
                   cal_coasting_dist(minimum_control_speed_2 * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/(minimum_control_speed_2 * 1.46667)
        df['space_mean_speed'] = 0.681818 * ((distance2stopbar - queue_coast + (((df['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084)/
                                             (df['t1'] + cal_coasting_time(vehicle_current_speed * 0.44704, df['v_0'] * 0.44704, A, B, C, M) +
                                              (df['v_0'] * 0.44704 - minimum_control_speed_2 * 0.44704)/maximum_deceleration + df['t2'] +
                                              cal_coasting_time(minimum_control_speed_2 * 0.44704, df['v_end'] * 0.44704, A, B, C, M)))

        df = df[(df['t2'] >= 0) & (df['space_mean_speed'] >= minimum_desired_speed) & (df['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

        if len(df) > 0 and df.t1.max() > 5: # cruising + coasting
            braking_a = None
            mode = 'Cruising'
        elif len(df) > 0 and df.t1.max() >= -1:
            braking_a = None
            mode = 'Coasting'

        else: # the first trajectory is too fast, need to be braking + cruising + coasting
            df_2 = pd.DataFrame(list(itertools.product(np.arange(0.1, maximum_deceleration, 0.1), np.arange(minimum_control_speed_1, vehicle_current_speed, 0.1), np.arange(0, np.minimum(desired_speed, minimum_control_speed_2), 0.1))), columns=['a_1', 'v_0', 'v_end'])
            df_2['t1'] = (distance2stopbar - queue_coast - dist_threshold4speed_limit_2 - (((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['a_1'])) * 3.28084) / (df_2['v_0'] * 1.46667)
            df_2['t2'] = (dist_threshold4speed_limit_2 + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 - \
                        (((df_2['v_0'] * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2) / (
                                    2 * maximum_deceleration)) * 3.28084 - \
                        cal_coasting_dist(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)) / (
                                   minimum_control_speed_2 * 1.46667)
            df_2['space_mean_speed'] = 0.681818 * ((distance2stopbar - queue_coast + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084) /
                                                 (df_2['t1'] + (vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704) / df_2['a_1'] + (df_2['v_0'] * 0.44704 - minimum_control_speed_2 * 0.44704)/maximum_deceleration + df_2['t2'] + cal_coasting_time(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

            # satisfy the constraints for braking + cruising + coasting
            df_2 = df_2[(df_2['t2'] >= 0) & (df_2['t1'] >= 0) & (df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

            if len(df_2) > 0:
                # calculate the tractive energy for all available trajectory
                df_2['cruising_energy1'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_1, df_2['t1'])
                df_2['cruising_energy2'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_2, df_2['t2'])
                df_2['kinetic_energy'] = np.vectorize(coasting_energy)(M, vehicle_current_speed, df_2['v_end'])
                df_2['total_energy'] = df_2['cruising_energy1'] + df_2['cruising_energy2'] + df_2['kinetic_energy']

                df_2 = df_2.loc[df_2.total_energy == df_2.total_energy.min()]
                v_end_sol = df_2.v_end.max()
                braking_a = df_2.loc[df_2.v_end == v_end_sol, 'a_1'].values[0]
                mode = 'Braking'

            else: # braking_a is out of bound, which means that we cannot make it, we have to gave it back to VISSIM control
                braking_a = None
                mode = 'VISSIM_Coast'  # VISSIM1 indicates that vehicle has to stop

    else: # coasting
        braking_a = None
        mode = 'VISSIM_Coast'

    return mode, braking_a


def minimum_space_mean_spd_case1(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed_2, maximum_deceleration, example_coasting_profile):
    v_0, t_0 = estimate_coasting_speed(vehicle_current_speed, distance2stopbar - queue_coast - 400 * 3.28084, example_coasting_profile)
    t_1 = ((v_0 - minimum_control_speed_2) * 0.44704)/maximum_deceleration
    d_1 = (((v_0 * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084
    v_2, t_2 = estimate_coasting_speed(minimum_control_speed_2, 400 * 3.28084 - d_1, example_coasting_profile)

    minimum_space_mean_spd = ((distance2stopbar - queue_coast)/(t_0 + t_1 + t_2)) * 0.681818

    return minimum_space_mean_spd


def minimum_space_mean_spd_case2(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed_1, minimum_control_speed_2, maximum_deceleration, example_coasting_profile):
    t_0 = ((vehicle_current_speed - minimum_control_speed_1) * 0.44704)/maximum_deceleration
    d_0 = (((vehicle_current_speed * 0.44704) ** 2 - (minimum_control_speed_1 * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084

    t_1 = np.maximum(0, (distance2stopbar - queue_coast - 400 * 3.28084 - d_0)/(minimum_control_speed_1 * 1.46667))

    t_2 = ((minimum_control_speed_1 - minimum_control_speed_2) * 0.44704)/maximum_deceleration
    d_2 = (((minimum_control_speed_1 * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084

    v_3, t_3 = estimate_coasting_speed(minimum_control_speed_2, 400 * 3.28084 - d_2, example_coasting_profile)

    minimum_space_mean_spd = ((distance2stopbar - queue_coast)/(t_0 + t_1 + t_2 + t_3)) * 0.681818

    return minimum_space_mean_spd


def coasting_numeric_relax_sol_700m_r(dist_threshold4speed_limit_2, distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, minimum_control_speed_1, minimum_control_speed_2,
                         green_window4coast, simsec, A, B, C, M, maximum_deceleration, example_coasting_profile):

    if vehicle_current_speed > minimum_control_speed_1:
        # calculate the minimum feasible desired speed to reduce the computation load
        minimum_space_mean_spd_1 = minimum_space_mean_spd_case1(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed_2, maximum_deceleration, example_coasting_profile)

        minimum_space_mean_spd_2 = minimum_space_mean_spd_case2(vehicle_current_speed, distance2stopbar, queue_coast, minimum_control_speed_1, minimum_control_speed_2, maximum_deceleration, example_coasting_profile)

        if desired_speed < minimum_space_mean_spd_2:
            braking_a = None
            # mode = 'IDM', vehicle cannot go through signals during green based on the current conditions, coasting for saving energy
            mode = 'Coasting'

        elif desired_speed < minimum_space_mean_spd_1:
            df_2 = pd.DataFrame(list(itertools.product(np.arange(0.1, maximum_deceleration, 0.1), np.arange(minimum_control_speed_1, vehicle_current_speed, 0.5), np.arange(0, np.minimum(desired_speed, minimum_control_speed_2), 0.5))), columns=['a_1', 'v_0', 'v_end'])
            df_2['t1'] = (distance2stopbar - queue_coast - dist_threshold4speed_limit_2 - (((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['a_1'])) * 3.28084) / (df_2['v_0'] * 1.46667)
            df_2['t2'] = (dist_threshold4speed_limit_2 + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 - \
                        (((df_2['v_0'] * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2) / (
                                    2 * maximum_deceleration)) * 3.28084 - \
                        cal_coasting_dist(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)) / (
                                   minimum_control_speed_2 * 1.46667)
            df_2['space_mean_speed'] = 0.681818 * ((distance2stopbar - queue_coast + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084) /
                                                 (df_2['t1'] + (vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704) / df_2['a_1'] + (df_2['v_0'] * 0.44704 - minimum_control_speed_2 * 0.44704)/maximum_deceleration + df_2['t2'] + cal_coasting_time(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

            # satisfy the constraints for braking + cruising + coasting
            df_2 = df_2[(df_2['t2'] >= 0) & (df_2['t1'] >= 0) & (df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

            if len(df_2) > 0:
                # calculate the tractive energy for all available trajectory
                df_2['cruising_energy1'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_1, df_2['t1'])
                df_2['cruising_energy2'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_2, df_2['t2'])
                # df_2['kinetic_energy'] = np.vectorize(coasting_energy)(M, vehicle_current_speed, df_2['v_end'])
                df_2['departure_energy'] = np.vectorize(departure_energy)(A, B, C, M, df_2['v_end'], 50, a_comfort)

                df_2['total_energy'] = df_2['cruising_energy1'] + df_2['cruising_energy2'] + df_2['departure_energy']

                df_2 = df_2.loc[df_2.total_energy <= (df_2.total_energy.min() + df_2.total_energy.min() * 0.01)].reset_index(drop=True)
                braking_a = df_2.loc[df_2.space_mean_speed == df_2.space_mean_speed.max(), 'a_1'].values[0]
                mode = 'Braking'

            else: # braking_a is out of bound, which means that we cannot make it, then we do coasting at this moment.
                braking_a = None
                # mode = 'IDM', vehicle cannot go through signals during green based on the current conditions, coasting for saving energy
                mode = 'Coasting'
        else:
            df = pd.DataFrame(list(itertools.product(np.arange(minimum_control_speed_1, vehicle_current_speed, 0.5),
                                                       np.arange(0, np.minimum(desired_speed, minimum_control_speed_2),
                                                                 0.5))), columns=['v_0', 'v_end'])
            df['t1'] = (distance2stopbar - queue_coast - dist_threshold4speed_limit_2 - cal_coasting_dist(vehicle_current_speed * 0.44704, df['v_0'] * 0.44704, A, B, C, M))/(vehicle_current_speed * 1.46667)
            df['t2'] = (dist_threshold4speed_limit_2 + (((df['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 - \
                       (((df['v_0'] * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2)/(2 * maximum_deceleration)) * 3.28084 - \
                       cal_coasting_dist(minimum_control_speed_2 * 0.44704, df['v_end'] * 0.44704, A, B, C, M))/(minimum_control_speed_2 * 1.46667)
            df['space_mean_speed'] = 0.681818 * ((distance2stopbar - queue_coast + (((df['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084)/
                                                 (df['t1'] + cal_coasting_time(vehicle_current_speed * 0.44704, df['v_0'] * 0.44704, A, B, C, M) +
                                                  (df['v_0'] * 0.44704 - minimum_control_speed_2 * 0.44704)/maximum_deceleration + df['t2'] +
                                                  cal_coasting_time(minimum_control_speed_2 * 0.44704, df['v_end'] * 0.44704, A, B, C, M)))

            df = df[(df['t2'] >= 0) & (df['space_mean_speed'] >= minimum_desired_speed) & (df['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

            if len(df) > 0 and df.t1.max() > 5: # cruising + coasting
                braking_a = None
                mode = 'Cruising'
            elif len(df) > 0 and df.t1.max() >= -1:
                braking_a = None
                mode = 'Coasting'

            else: # the first trajectory is too fast, need to be braking + cruising + coasting
                df_2 = pd.DataFrame(list(itertools.product(np.arange(0.1, maximum_deceleration, 0.1), np.arange(minimum_control_speed_1, vehicle_current_speed, 0.5), np.arange(0, np.minimum(desired_speed, minimum_control_speed_2), 0.5))), columns=['a_1', 'v_0', 'v_end'])
                df_2['t1'] = (distance2stopbar - queue_coast - dist_threshold4speed_limit_2 - (((vehicle_current_speed * 0.44704) ** 2 - (df_2['v_0'] * 0.44704) ** 2)/(2 * df_2['a_1'])) * 3.28084) / (df_2['v_0'] * 1.46667)
                df_2['t2'] = (dist_threshold4speed_limit_2 + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084 - \
                            (((df_2['v_0'] * 0.44704) ** 2 - (minimum_control_speed_2 * 0.44704) ** 2) / (
                                        2 * maximum_deceleration)) * 3.28084 - \
                            cal_coasting_dist(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)) / (
                                       minimum_control_speed_2 * 1.46667)
                df_2['space_mean_speed'] = 0.681818 * ((distance2stopbar - queue_coast + (((df_2['v_end'] * 0.44704) ** 2) / (2 * a_comfort)) * 3.28084) /
                                                     (df_2['t1'] + (vehicle_current_speed * 0.44704 - df_2['v_0'] * 0.44704) / df_2['a_1'] + (df_2['v_0'] * 0.44704 - minimum_control_speed_2 * 0.44704)/maximum_deceleration + df_2['t2'] + cal_coasting_time(minimum_control_speed_2 * 0.44704, df_2['v_end'] * 0.44704, A, B, C, M)))

                # satisfy the constraints for braking + cruising + coasting
                df_2 = df_2[(df_2['t2'] >= 0) & (df_2['t1'] >= 0) & (df_2['space_mean_speed'] >= minimum_desired_speed) & (df_2['space_mean_speed'] <= desired_speed)].reset_index(drop=True)

                if len(df_2) > 0:
                    # calculate the tractive energy for all available trajectory
                    df_2['cruising_energy1'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_1, df_2['t1'])
                    df_2['cruising_energy2'] = np.vectorize(cruising_tractive_energy)(A, B, C, M, minimum_control_speed_2, df_2['t2'])
                    # df_2['kinetic_energy'] = np.vectorize(coasting_energy)(M, vehicle_current_speed, df_2['v_end'])
                    df_2['departure_energy'] = np.vectorize(departure_energy)(A, B, C, M, df_2['v_end'], 50, a_comfort)

                    df_2['total_energy'] = df_2['cruising_energy1'] + df_2['cruising_energy2'] + df_2['departure_energy']

                    df_2 = df_2.loc[df_2.total_energy <= (df_2.total_energy.min() + df_2.total_energy.min() * 0.01)].reset_index(drop=True)
                    braking_a = df_2.loc[df_2.space_mean_speed == df_2.space_mean_speed.max(), 'a_1'].values[0]

                    mode = 'Braking'

                else: # braking_a is out of bound, which means that we cannot make it, then we do coasting at this moment.
                    braking_a = None
                    # mode = 'IDM', vehicle cannot go through signals during green based on the current conditions, coasting for saving energy
                    mode = 'Coasting'

    else: # coasting
        braking_a = None
        mode = 'IDM'

    return mode, braking_a


def coasting4instant_desired_spd_toyota_v3(A, B, C, M, dist_threshold4speed_limit_1, dist_threshold4speed_limit_2,
                                        speed_limit_ratio_1, speed_limit_ratio_2, vehicle_current_speed,
                                        desired_speed, minimum_desired_speed, queue_coast, distance2stopbar, a_comfort, orginal_desire_spd, green_window4coast, simsec, maximum_deceleration):

    if (distance2stopbar - queue_coast) > dist_threshold4speed_limit_1:
        # if distance to the queue end is greater than 700 meter, no control will be performed
        instant_desired_speed = orginal_desire_spd
        mode = 'VISSIM'
    elif (distance2stopbar - queue_coast) > dist_threshold4speed_limit_2: # d is between 400m to 700m
        # desired_speed = np.maximum(desired_speed, orginal_desire_spd * speed_limit_ratio_1)
        if desired_speed == orginal_desire_spd:
            instant_desired_speed = desired_speed
            mode = 'VISSIM'
        else:
            if desired_speed < vehicle_current_speed:
                mode, braking_decel = coasting_numeric_relax_sol_700m(dist_threshold4speed_limit_2, distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_1, orginal_desire_spd * speed_limit_ratio_2, green_window4coast, simsec, A, B, C, M, maximum_deceleration)
                if mode == 'Cruising':
                    instant_desired_speed = vehicle_current_speed
                elif mode == 'Coasting':
                    a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                            (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                    # print('status: coasting2')
                    instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph
                elif mode == 'Braking':
                    instant_desired_speed = np.maximum(orginal_desire_spd * speed_limit_ratio_1, vehicle_current_speed - braking_decel * 2.23694)  # mph
                elif mode == 'VISSIM_Coast':
                    a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                            (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                    # print('status: coasting2')
                    instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph

            elif minimum_desired_speed < vehicle_current_speed:
                if vehicle_current_speed >= orginal_desire_spd * speed_limit_ratio_1:
                    instant_desired_speed = vehicle_current_speed
                    mode = 'Cruising'
                else:
                    instant_desired_speed = np.minimum(orginal_desire_spd * speed_limit_ratio_1, desired_speed)
                    mode = 'Acceleration'
            else:
                instant_desired_speed = np.maximum(minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_1)
                mode = 'Acceleration'

    else: # d is less than 400m
        if desired_speed == orginal_desire_spd:
            instant_desired_speed = desired_speed
            mode = 'VISSIM'
        else:
            if desired_speed < vehicle_current_speed:
                mode, braking_decel = coasting_numeric_relax_sol(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_2, green_window4coast, simsec, A, B, C, M, maximum_deceleration)
                # mode, cruising_time, coast_end_speed, braking_decel = coasting_numeric_relax_sol(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, 0, green_window4coast, simsec, A, B, C, M, maximum_deceleration)
                if mode == 'Cruising':
                    instant_desired_speed = vehicle_current_speed
                elif mode == 'Coasting':
                    a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                            (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                    # print('status: coasting2')
                    instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph
                elif mode == 'Braking':
                    instant_desired_speed = np.maximum(orginal_desire_spd * speed_limit_ratio_2, vehicle_current_speed - braking_decel * 2.23694)  # mph
                elif mode == 'VISSIM_Coast':
                    a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                            (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                    # print('status: coasting2')
                    instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph

            elif minimum_desired_speed < vehicle_current_speed:
                if vehicle_current_speed >= orginal_desire_spd * speed_limit_ratio_2:
                    instant_desired_speed = vehicle_current_speed
                    mode = 'Cruising'
                else:
                    instant_desired_speed = np.minimum(orginal_desire_spd * speed_limit_ratio_2, desired_speed)
                    mode = 'Acceleration'
            else:
                instant_desired_speed = np.maximum(minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_2)
                mode = 'Acceleration'

    return mode, instant_desired_speed


def GippsSafeSpeed(acc_dist, acc_speed_base, orginal_desire_spd, minimum_gap=2):
    """

    Parameters
    ----------
    acc_dist: feet
    acc_speed_base: mph
    orginal_desire_spd: mph
    minimum_gap: m

    Returns
    -------
    safe_speed: mph

    """
    # unit transformation
    orginal_desire_spd = orginal_desire_spd * 0.44704
    acc_speed_base = acc_speed_base * 0.44704
    acc_dist = acc_dist * 0.3048

    # calculate the safe speed based on Gipps' Model
    if acc_dist >= minimum_gap:
        safe_speed = np.sqrt(2 * (acc_dist - minimum_gap) + acc_speed_base ** 2 + 1.1 ** 2) - 1.1
    else:
        safe_speed = np.sqrt(acc_speed_base ** 2 + 1.1 ** 2) - 1.1

    return np.maximum(safe_speed * 2.23694, 0)

#
# def IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist, minimum_gap=2, time_gap=1, a_max=1.5, b_comfort=2, acceleration_exp=4):
#     """
#
#     Parameters
#     ----------
#     vehicle_current_speed: mph
#     orginal_desire_spd: mph
#     acc_speed_base: mph
#     acc_dist: feet
#     minimum_gap: m
#     time_gap: s
#     a_max: m/s^2
#     b_comfort: m/s^2
#     acceleration_exp: None
#
#     Returns
#     -------
#     a_instant: m/s^2
#     instant_desired_speed: mph
#     """
#     # unit transformation
#     vehicle_current_speed = vehicle_current_speed * 0.44704
#     orginal_desire_spd = orginal_desire_spd * 0.44704
#     acc_speed_base = acc_speed_base * 0.44704
#     acc_dist = acc_dist * 0.3048
#
#     if np.isnan(acc_dist):
#         acc_dist = 2000
#         acc_speed_base = 0
#
#     desiredDistance = minimum_gap + np.maximum(0, (vehicle_current_speed * time_gap) +
#                                                (vehicle_current_speed * (vehicle_current_speed - acc_speed_base))/(2 * np.sqrt(a_max * b_comfort)))
#     a_instant = a_max * (1 - (vehicle_current_speed/orginal_desire_spd) ** acceleration_exp - (desiredDistance/acc_dist) ** 2)
#
#     instant_desired_speed = np.maximum(0, vehicle_current_speed + a_instant)
#
#     return a_instant, instant_desired_speed * 2.23694
#

def IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist, minimum_gap=2, time_gap=1, a_max=1.5, b_comfort=2, acceleration_exp=4):
    """

    Parameters
    ----------
    vehicle_current_speed: mph
    orginal_desire_spd: mph
    acc_speed_base: mph
    acc_dist: feet
    minimum_gap: m
    time_gap: s
    a_max: m/s^2
    b_comfort: m/s^2
    acceleration_exp: None

    Returns
    -------
    a_instant: m/s^2
    instant_desired_speed: mph
    """
    # unit transformation
    vehicle_current_speed = vehicle_current_speed * 0.44704
    orginal_desire_spd = orginal_desire_spd * 0.44704
    acc_speed_base = acc_speed_base * 0.44704
    acc_dist = acc_dist/3.28084

    if np.isnan(acc_dist):
        acc_dist = 2000
        acc_speed_base = 20.1168
    elif np.isnan(acc_speed_base):
        acc_speed_base = 20.1168

    desiredDistance = minimum_gap + np.maximum(0, (vehicle_current_speed * time_gap) +
                                               (vehicle_current_speed * (vehicle_current_speed - acc_speed_base))/(2 * np.sqrt(a_max * b_comfort)))
    z = desiredDistance / acc_dist
    if vehicle_current_speed <= orginal_desire_spd:
        a_free = a_max * (1 - (vehicle_current_speed/orginal_desire_spd) ** acceleration_exp)
        if z >= 1:
            a_instant = a_max * (1 - z ** 2)
        else:
            if a_free > 0:
                a_instant = a_free * (1 - z ** ((2 * a_max)/a_free))
            else:
                a_instant = a_free

    else:
        a_free = (-1) * b_comfort * (1 - (vehicle_current_speed/orginal_desire_spd) ** ((a_max * acceleration_exp)/b_comfort))
        if z >= 1:
            a_instant = a_free + a_max * (1 - z ** 2)
        else:
            a_instant = a_free

    instant_desired_speed = np.maximum(0, vehicle_current_speed + a_instant)

    return a_instant, instant_desired_speed * 2.23694


def IDM4LeadVehicleOrRedLight(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist, distance2stopbar):

    if np.isnan(acc_dist) or np.isnan(acc_speed_base):
        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, 0, distance2stopbar)
    else:
        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist)

    return a_instant, instant_desired_speed


def coasting4instant_desired_spd_toyota_v4(A, B, C, M, dist_threshold4speed_limit_1, dist_threshold4speed_limit_2,
                                        speed_limit_ratio_1, speed_limit_ratio_2, vehicle_current_speed, current_acceleration,
                                        desired_speed, minimum_desired_speed, queue_coast, distance2stopbar,
                                        a_comfort, orginal_desire_spd, green_window4coast, simsec, sig_state, maximum_deceleration, acc_speed_base, acc_dist, example_coasting_profile):

    if 'green' in sig_state and desired_speed <= 45 and distance2stopbar <= 80:
        mode = 'IDM'
        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist)

    else:
        if (distance2stopbar - queue_coast) > dist_threshold4speed_limit_1: # d is greater than 700m
            # if distance to the queue end is greater than 700 meter, no coasting control will be performed. using IDM instead.
            mode = 'IDM'
            a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist)

        elif (distance2stopbar - queue_coast) > dist_threshold4speed_limit_2: # d is between 400m to 700m
            # desired_speed = np.maximum(desired_speed, orginal_desire_spd * speed_limit_ratio_1)
            if desired_speed == orginal_desire_spd:
                mode = 'IDM'
                a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd,
                                                                        acc_speed_base, acc_dist)
            else:
                if desired_speed < vehicle_current_speed:
                    mode, a_deceleration = coasting_numeric_relax_sol_700m_r(dist_threshold4speed_limit_2, distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_1, orginal_desire_spd * speed_limit_ratio_2, green_window4coast, simsec, A, B, C, M, maximum_deceleration, example_coasting_profile)
                    if mode == 'Cruising':
                        instant_desired_speed = vehicle_current_speed
                    elif mode == 'Coasting':
                        a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                                (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                        # print('status: coasting2')
                        instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph
                    elif mode == 'Braking':
                        instant_desired_speed = np.maximum(orginal_desire_spd * speed_limit_ratio_2, vehicle_current_speed - a_deceleration * 2.23694)  # mph
                    elif mode == 'IDM':
                        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd,
                                                                                acc_speed_base, acc_dist)

                elif minimum_desired_speed < vehicle_current_speed:
                    if vehicle_current_speed >= orginal_desire_spd * speed_limit_ratio_1:
                        instant_desired_speed = vehicle_current_speed
                        mode = 'Cruising'
                    else:
                        mode = 'IDM'
                        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, desired_speed,
                                                                                acc_speed_base, acc_dist)
                else:
                    mode = 'IDM'
                    a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, desired_speed,
                                                                            acc_speed_base, acc_dist)

        else: # d is less than 400m
            if desired_speed == orginal_desire_spd:
                mode = 'IDM'
                a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, orginal_desire_spd,
                                                                        acc_speed_base, acc_dist)
            else:
                if desired_speed < vehicle_current_speed:
                    mode, a_deceleration = coasting_numeric_relax_sol_r(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, minimum_desired_speed, orginal_desire_spd * speed_limit_ratio_2, green_window4coast, simsec, A, B, C, M, maximum_deceleration, example_coasting_profile)
                    # mode, cruising_time, coast_end_speed, braking_decel = coasting_numeric_relax_sol(distance2stopbar, queue_coast, a_comfort, vehicle_current_speed, desired_speed, 0, green_window4coast, simsec, A, B, C, M, maximum_deceleration)
                    if mode == 'Cruising':
                        instant_desired_speed = vehicle_current_speed
                    elif mode == 'Coasting':
                        a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                                (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                        # print('status: coasting2')
                        instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph
                    elif mode == 'Braking':
                        instant_desired_speed = np.maximum(desired_speed, vehicle_current_speed - a_deceleration * 2.23694)  # mph
                    elif mode == 'IDM':
                        a_instant, instant_desired_speed = IDM4LeadVehicleOrRedLight(vehicle_current_speed, orginal_desire_spd, acc_speed_base, acc_dist, distance2stopbar)
                    elif mode == 'AEB':
                        instant_desired_speed = np.maximum(desired_speed, vehicle_current_speed - a_deceleration * 2.23694)  # mph

                # elif minimum_desired_speed < vehicle_current_speed:
                #     # if vehicle_current_speed >= orginal_desire_spd * speed_limit_ratio_2:
                #     instant_desired_speed = vehicle_current_speed
                #     mode = 'Cruising'
                    # else:
                    #     mode = 'Coasting'
                    #     a_deceleration = (A + B * (vehicle_current_speed * 0.44704) + C * (
                    #             (vehicle_current_speed * 0.44704) ** 2)) / M  # m/s2
                    #     instant_desired_speed = np.maximum(minimum_desired_speed, vehicle_current_speed - a_deceleration * 2.23694 * 1)  # mph

                # elif vehicle_current_speed == 0:
                else:
                    if 'red' in sig_state and vehicle_current_speed <= 10: # vehicle stop during red light
                        # instant_desired_speed = 0
                        mode = 'Creeping'
                        a_instant, instant_desired_speed = IDM4LeadVehicleOrRedLight(vehicle_current_speed,
                                                                                    desired_speed, acc_speed_base,
                                                                                    acc_dist, distance2stopbar)
                    else:
                        mode = 'IDM'
                        a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, desired_speed,
                                                                                acc_speed_base, acc_dist)

                # else:
                #     mode = 'IDM' # need to accelerate using IDM
                #     a_instant, instant_desired_speed = IntelligentDriverModel(vehicle_current_speed, desired_speed, acc_speed_base, acc_dist)

    # jerk constraint and acceleration, deceleration constraints. +-2
    jerk_constraint = 10
    if mode == 'IDM':
        if abs(a_instant - current_acceleration) > jerk_constraint:
            if a_instant > current_acceleration:
                a_instant = current_acceleration + jerk_constraint
            else:
                a_instant = current_acceleration - jerk_constraint
            instant_desired_speed = np.maximum(vehicle_current_speed + a_instant * 2.23694, 0)
        a_out = a_instant
    elif mode == 'Cruising':
        if abs(current_acceleration) > jerk_constraint:
            if current_acceleration < 0:
                a_instant = current_acceleration + jerk_constraint
            else:
                a_instant = current_acceleration - jerk_constraint
            instant_desired_speed = np.maximum(vehicle_current_speed + a_instant * 2.23694, 0)
        else:
            a_instant = 0
        a_out = a_instant
    elif mode in ['Coasting', 'Braking']:
        if abs(-a_deceleration - current_acceleration) > jerk_constraint:
            if -a_deceleration > current_acceleration:
                a_deceleration = -(current_acceleration + jerk_constraint)
            else:
                a_deceleration = -(current_acceleration - jerk_constraint)
            instant_desired_speed = np.maximum(vehicle_current_speed - a_deceleration * 2.23694, desired_speed)
        a_out = -a_deceleration
    elif mode == 'AEB':
        a_out = -a_deceleration
    else:
        a_out = a_instant

    # safety consideration, smaller than the Gipps' Safe Speed
    if np.isnan(acc_dist) or np.isnan(acc_speed_base):
        safe_speed = 50
    else:
        safe_speed = GippsSafeSpeed(acc_dist, acc_speed_base, orginal_desire_spd, minimum_gap=2)

    if instant_desired_speed > safe_speed:
        instant_desired_speed = safe_speed
        mode = 'Following'
        a_out = (safe_speed - vehicle_current_speed) * 0.44704

    return mode, instant_desired_speed, a_out


def gen_desired_spd(example_coasting_profile, A, B, C, M, orginal_desire_spd, next_movement, vehicle_current_speed, current_acceleration, simsec, distance2stopbar,
                    acc_speed_base, acc_dist, num_through_lanes, upstream_flow_rate, upstream_flow_speed, SigState, 
                    t1s, t1e, t2s, t2e, r1s, distance2stopbar_next=None, t21s=None, t21e=None, t22s=None, t22e=None):
    """
    :param example_coasting_profile: read from local csv file
    :param A: read from local configuration file, shallowford.ini
    :param B: read from local configuration file, shallowford.ini
    :param C: read from local configuration file, shallowford.ini
    :param M: read from local configuration file, shallowford.ini
    :param orginal_desire_spd: read from local configuration file, shallowford.ini
    :param next_movement: read from local configuration file, shallowford.ini
    :param vehicle_current_speed: vehicle current speed in mph
    :param current_acceleration: vehicle current acceleration in m/s2
    :param simsec: current absolute time minus the reference timestamp (datetime.strptime('06:30:00', '%H:%M:%S'))
    :param distance2stopbar: distance to stopbar in feet
    :param acc_speed_base: lead vehicle speed in mph
    :param acc_dist: distance to the lead vehicle in feet
    :param num_through_lanes: read from local configuration file, shallowford.ini
    :param upstream_flow_rate: upstream flow rate in veh/hour, can be a fixed number, e.g., 300
    :param upstream_flow_speed: upstream flow speed in mph, can be a fixed number, e.g., 40
    :param SigState: signal status, green, yellow, red
    :param t1s: the first green window start time, absolute time minus the reference timestamp
    :param t1e: the first green window end time, absolute time minus the reference timestamp
    :param t2s: the second green window start time, absolute time minus the reference timestamp
    :param t2e: the second green window end time, absolute time minus the reference timestamp
    :param r1s: the first red window start time, absolute time minus the reference timestamp
    :param distance2stopbar_next: distance to the second downstream stopbar in feet, default None
    :param t21s: default None
    :param t21e: default None
    :param t22s: default None
    :param t22e: default None

    :return:
    instant_desired_speed: instant desired speed in mph for the next second
    mode: speed mode
    a_out: instant acceleration in m/s2 for the next second
    desired_speed: the maximum desired speed (space-mean speed for the process of approaching intersection)
    minimum_desired_speed : the minimum desired speed (space-mean speed for the process of approaching intersection)
    
    """
    # print(simsec)
    # if simsec >= 1200:
    #     print('break')
    queue_discharge_speed = 14.398  # ft/second
    departure_speed = 40 * 1.46667  # ft/second
    radar_detection_range = 500  # feet
    queue_len_per_vehicle = 25  # feet
    # maximum_deceleration = 3.41376  # m/s2, equals to 11.2 ft/sec2
    # maximum_deceleration = 4.572  # m/s2, equals to 15 ft/sec2
    maximum_deceleration = 3  # m/s2
    a_comfort = 2.5  # m/s²

    r2s = t1e

    # if t1s <= simsec < t1e:
    #     SigState = 'green'
    # else:
    #     SigState = 'red'

    # estimate the queuing shockwave speed
    if upstream_flow_speed != 0 and upstream_flow_rate != 0:
        v_ac = abs(1/((1/upstream_flow_speed) - (211/(upstream_flow_rate/num_through_lanes))))
    else:
        v_ac = 0

    if next_movement == 'TH':

        # predict queue length
        queue_length_w1 = np.maximum(0, (v_ac / (v_ac + orginal_desire_spd)) * (distance2stopbar - orginal_desire_spd * 1.46667 * (r1s - simsec)))
        maximum_dischargeable_queue_w1 = ((t1e - t1s) * queue_discharge_speed * departure_speed) / (queue_discharge_speed + departure_speed)

        queue_length_w2 = np.maximum(0, (v_ac / (v_ac + orginal_desire_spd)) * (distance2stopbar - orginal_desire_spd * 1.46667 * (r2s - simsec)))

        # adjusting the queue length for window 2 considering oversaturation
        # if queue_length_w1 > maximum_dischargeable_queue_w1:
        #     queue_length_w2 = queue_length_w2 + queue_length_w1 - maximum_dischargeable_queue_w1

        # adjusting queue length using ACC radar detection information
        # if not np.isnan(acc_dist):
        if acc_dist != 'None' and not np.isnan(acc_dist): # udpate based on ACC function.
            queue_length_w1 = queue_update_acc(radar_detection_range, queue_len_per_vehicle, queue_length_w1, distance2stopbar, vehicle_current_speed, acc_speed_base, acc_dist, SigState)
            # print('ACC updated')
        else:
            pass

        # adjust queue length for window 1 and 2 based on adjusted red window
        # queue_length_w1, queue_length_w2 = queue_update_adjust_red_window(r1s, t1s, r2s, t2s, distance2stopbar, orginal_desire_spd, queue_discharge_speed, queue_length_w1,
        #                                queue_length_w2, maximum_dischargeable_queue_w1, simsec)

        # estimate the adjusted green window
        green_window_start1, green_window_end1, green_window_start2, green_window_end2 = green_window_adjust(queue_discharge_speed, departure_speed, queue_length_w1, queue_length_w2, t1s, t1e, t2s, t2e)

        # generate desired speed for cav
        # desired_speed, minimum_desired_speed, queue_coast, green_window4coast = desire_speed4green_window(simsec, orginal_desire_spd, distance2stopbar, queue_length_w1, queue_length_w2, green_window_start1,
        #                       green_window_end1, green_window_start2, green_window_end2, t1s)

        # generate desired speed for cav, considering two downstream intersections
        desired_speed, minimum_desired_speed, queue_coast, green_window4coast = desire_speed4green_window_w_downstream(simsec, orginal_desire_spd, distance2stopbar, queue_length_w1,
                                           queue_length_w2, green_window_start1, green_window_end1,
                                           green_window_start2, green_window_end2, t1s,
                                           distance2stopbar_next, t21s, t21e, t22s, t22e)

    else:  # if CAV can freely go through the intersection without stop
        desired_speed = orginal_desire_spd
        minimum_desired_speed = 0
        queue_coast = 0
        green_window4coast = t1s

    # control the available range of minimum desired speed and desired speed
    # speed_desire_range = 0.5
    # minimum_desired_speed = (desired_speed - minimum_desired_speed) * speed_desire_range + minimum_desired_speed

    # control desired speed greater than 3.2 mph
    # if desired_speed < 3.2:
    #     desired_speed = 3.2

    # print('\ndesired speed: ', desired_speed)

    # get instant desired speed (for next second) based on distributed coasting strategy
    # instant_desired_speed = coasting4instant_desired_spd(A, B, C, M, minimum_control_speed, maximum_deceleration, vehicle_current_speed, desired_speed, queue_coast, distance2stopbar, a_comfort, green_window4coast, simsec, orginal_desire_spd)

    # using toyota simplistic instant desired speed
    # mode, instant_desired_speed = coasting4instant_desired_spd_toyota(A, B, C, M, 2296.59, 1312.34,
    #                                     0.75, 0.5, vehicle_current_speed,
    #                                     desired_speed, queue_coast, distance2stopbar, orginal_desire_spd)

    # using toyota simplistic instant desired speed, relax the minimum control speed for d < 400 m
    # mode, instant_desired_speed = coasting4instant_desired_spd_toyota_v3(A, B, C, M, 2296.59, 1312.34,
    #                                     0.75, 0.5, vehicle_current_speed,
    #                                     desired_speed, minimum_desired_speed, queue_coast, distance2stopbar, a_comfort, orginal_desire_spd, green_window4coast, simsec, maximum_deceleration)

    # mode, instant_desired_speed, a_out = coasting4instant_desired_spd_toyota_v4(A, B, C, M, 2296.59, 1312.34,
    #                                     0.75, 0.5, vehicle_current_speed, current_acceleration,
    #                                     desired_speed, max(desired_speed - 1, 0), queue_coast, distance2stopbar, a_comfort, orginal_desire_spd, green_window4coast, simsec, SigState, maximum_deceleration, acc_speed_base, acc_dist, example_coasting_profile)
    mode, instant_desired_speed, a_out = coasting4instant_desired_spd_toyota_v4(A, B, C, M, 2296, 1312,
                                        0.75, 0.5, vehicle_current_speed, current_acceleration,
                                        desired_speed, max(desired_speed - 1, 0), queue_coast, distance2stopbar, a_comfort, orginal_desire_spd, green_window4coast, simsec, SigState, maximum_deceleration, acc_speed_base, acc_dist, example_coasting_profile)

    if np.isnan(instant_desired_speed):
        print('Desired Speed is NaN')
    # print('\ninstant_desired_speed: ', instant_desired_speed)
    # print('\ncurrent time: {}, \ncurrent speed: {}, \nqueue_length: {}, \ninstant_desired_speed: {}, \nmode: {}, \na_out: {}, \ndesired_speed: {}, \nminimum_desired_speed: {}\n'.format(
        # simsec, vehicle_current_speed, queue_coast, instant_desired_speed, mode, a_out, desired_speed, minimum_desired_speed))

    # return queue_coast, float(instant_desired_speed), mode, float(a_out), desired_speed, minimum_desired_speed
    return float(instant_desired_speed), mode, float(a_out), desired_speed, minimum_desired_speed


if __name__ == '__main__':
    """
    using python v3.
    """
    ################################################################################################
    ################################       Global Variables      ##################################
    ################################################################################################
    # set the original desired speed (speed limit: 40 mph)
    # orginal_desire_spd = 40.4357  # mph

    # 1: Hickory; 2: ShallowfordVillage; 3: I-75S; 4: I-75N; 5: Gunbarrel; 6: Lifestype; 7: Hamilton; 8: Lee Hwy
    int_id_list = [1, 2, 3, 4, 5, 6, 7, 8]

    next_movement = 'TH'
    # dist_threshold4speed_limit_1 = 2296.59  # equals to 700 m
    # dist_threshold4speed_limit_2 = 1312.34  # equals to 400 m
    # speed_limit_ratio_1 = 0.75
    # speed_limit_ratio_2 = 0.5
    # speed_desire_range = 0.5

    # coasting parameters based on tractive power
    # A = 0.156461
    # B = 0.002002
    # C = 0.000493
    # M = 1.4788
    A = 0.083698
    B = 0.00385
    C = 0.000278
    M = 1.6443

    # reading sample coasting profile
    example_coasting_profile = pd.read_csv('example_coasting_profile.csv', index_col=0)

    ################################################################################################
    #####################     Read synthetic real-time input data      #############################
    ################################################################################################
    # read the testing data
    tem_input_data = pd.read_csv('bilinear_60%_w_queue_practical_1_archive_data4testing.csv', index_col=0)

    # sort by vehicle no and simsec
    tem_input_data = tem_input_data.sort_values(by=['No', 'current_time']).reset_index(drop=True)
    tem_input_data['duration'] = tem_input_data['current_time'].groupby(tem_input_data['No']).transform('count')
    # keep the vehicle that has the longest trajectory
    # tem_input_data = tem_input_data[tem_input_data['duration'] == tem_input_data['duration'].max()].reset_index(drop=True)
    tem_input_data = tem_input_data[tem_input_data['No'] == 3158].reset_index(drop=True)

    # get the list of SimSec
    simsec_list = list(tem_input_data['current_time'].astype(int))

    ################################################################################################
    ################################       excuting second by second      ##########################
    ################################################################################################
    time_step = 0
    while True:
        starttime = time.time()

        # time_step = int(time.time() - starttime)
        print("update timestep: ", time_step)
        simsec = simsec_list[time_step]
        print('simsec: ', simsec)

        # get real-time input data
        data_row = tem_input_data.loc[tem_input_data['current_time'] == simsec,
                                  ['OrgDesSpeed', 'Speed', 'Acceleration', 'nextSC', 'd', 'num_lanes', 'flow_rate', 'speed_agg',
                                   'SigState', 't1s', 't1e', 't2s', 't2e', 'r1s', 'r2s', 'Speed_base', 'dist',
                                   'distance2stopbar_next', 't1s_second', 't1e_second', 't2s_second', 't2e_second']].squeeze()
        print('real-time input variables:\n', data_row)
        print('original desired speed: ', tem_input_data.loc[tem_input_data['current_time'] == simsec].squeeze().DesSpeed)

        # get the specific variables
        orginal_desire_spd = data_row['OrgDesSpeed'] # vehicle original desired speed
        vehicle_current_speed = data_row['Speed']  # current speed, mph
        current_acceleration = data_row['Acceleration']
        downstream_int_id = data_row['nextSC']  # id of downstream intersection, int
        distance2stopbar = data_row['d']  # distance to downstream stopbar, float
        num_through_lanes = data_row['num_lanes']  # number of through lanes at the current segment, int
        upstream_flow_rate = data_row['flow_rate']  # 5 minutes aggregated flow rate of upstream intersection, vehicle per hour, from GridSmart
        upstream_flow_speed = data_row['speed_agg']  # 5 minutes aggregated speed of upstream traffic, mph, from GridSmart

        # green window of the downstream signal:
        # SigState: 'green', 'red', string
        # t1s, t1e: the start and end time of the closest green, float
        # t2s, t2e: the start and end time of the second green, float
        # r1s, r2s: the start time of first and second red, float
        green_window = data_row[['SigState', 't1s', 't1e', 't2s', 't2e', 'r1s']]

        # the front vehicle's speed and distance to front vehicle based on radar, Speed_base (mph), dist(feet)
        adaptive_cruise_control = data_row[['Speed_base', 'dist']]

        # get the desired speed
        # desired_speed, instant_desired_speed, mode = gen_desired_spd(A, B, C, M, orginal_desire_spd, next_movement, vehicle_current_speed,
        #                                         simsec, downstream_int_id, distance2stopbar, adaptive_cruise_control.Speed_base, adaptive_cruise_control.dist,
        #                                         num_through_lanes, upstream_flow_rate, upstream_flow_speed,
        #                                         green_window.SigState, green_window.t1s, green_window.t1e,
        #                                         green_window.t2s, green_window.t2e, green_window.r1s,
        #                                         distance2stopbar, green_window.t1s, green_window.t1e, green_window.t2s, green_window.t2e)

        # if the second downstream intersection SPaT is considered
        distance2stopbar_next = data_row['distance2stopbar_next']  # distance to downstream stopbar, float
        green_window_next = data_row[['t1s_second', 't1e_second', 't2s_second', 't2e_second']]

        queue_length, instant_desired_speed, mode, a_out = gen_desired_spd(example_coasting_profile, A, B, C, M, orginal_desire_spd, next_movement, vehicle_current_speed, current_acceleration,
                                                simsec, downstream_int_id, distance2stopbar, adaptive_cruise_control.Speed_base, adaptive_cruise_control.dist,
                                                num_through_lanes, upstream_flow_rate, upstream_flow_speed,
                                                green_window.t1s, green_window.t1e,
                                                green_window.t2s, green_window.t2e, green_window.r1s,
                                                distance2stopbar_next, green_window_next.t1s_second,
                                                green_window_next.t1e_second, green_window_next.t2s_second, green_window_next.t2e_second)

        print('current speed: {}, \nqueue_length: {}, \ninstant_desired_speed: {}, \nmode: {}, \na_out: {}\n'.format(vehicle_current_speed, queue_length, instant_desired_speed, mode, a_out))
        print('Computing time: ', time.time() - starttime)
        print('\n----------------------------------------------------------------------------------------------------')
        # excute the main function every second, if processing time is less than 1 second,
        # then sleep for the remaining time in the second.
        # time.sleep(1 - ((time.time() - starttime) % 1))
        time_step = time_step + 1
