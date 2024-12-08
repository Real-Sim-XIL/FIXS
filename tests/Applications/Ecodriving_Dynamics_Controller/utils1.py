import numpy as np
import matplotlib.pyplot as plt

def plot_learning_curve(x, scores, figure_file):
    running_avg = np.zeros(len(scores))
    for i in range(len(running_avg)):
        running_avg[i] = np.mean(scores[max(0, i-100):(i+1)])
    plt.plot(x, running_avg)
    plt.title('Running average of previous 100 scores')
    plt.savefig(figure_file)

def vtcpmf_fuel_model(v_speed, v_accel):

    """
    VT-CPFM Fuel Model
    """

    r_f = (
        1.23 * 0.6 * 0.98 * 3.28 * (v_speed**2)
        + 9.8066 * 3152 * (1.75 / 1000) * 0.033 * v_speed
        + 9.8066 * 3152 * (1.75 / 1000) * 0.033
        + 9.8066 * 3152 * 0
    )

    power = ((r_f + 1.04 * 3152 * v_accel) / (3600 * 0.92)) * v_speed

    if power >= 0:
        return 0.00078 + 0.000019556 * power + 0.000001 * (power**2)
    else:
        return 0.00078

def process_spat_for_gen_speed_update(tl_light, spat_statuses):
    x1, x2, x3, x4, x5, x6 = (spat_statuses['2'], spat_statuses['3'], spat_statuses['10'], spat_statuses['8'], spat_statuses['9'], spat_statuses['12'])

    if tl_light[0][0] == '2':

        green_max = 24
        if x1 == None:
            t1s = 0
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'green'
        # time to green, currently red
        elif x1[0] == 'g':
            t1s = x1[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x1[0] == 'r':
            t1s = 0
            t1e = x1[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x1[1]
            curr_status = 'green'

    elif tl_light[0][0] == '3':

        green_max = 64
        # time to green, currently red
        if x2[0] == 'g':
            t1s = x2[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x2[0] == 'r':
            t1s = 0
            t1e = x2[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x2[1]
            curr_status = 'green'

    elif tl_light[0][0] == '10':

        green_max = 46
        # time to green, currently red
        if x3[0] == 'g':
            t1s = x3[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x3[0] == 'r':
            t1s = 0
            t1e = x3[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x3[1]
            curr_status = 'green'

    elif tl_light[0][0] == '8':

        green_max = 38
        # time to green, currently red
        if x4[0] == 'g':
            t1s = x4[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x4[0] == 'r':
            t1s = 0
            t1e = x4[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x4[1]
            curr_status = 'green'

    elif tl_light[0][0] == '9':

        green_max = 68
        # time to green, currently red
        if x5[0] == 'g':
            t1s = x5[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x5[0] == 'r':
            t1s = 0
            t1e = x5[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x5[1]
            curr_status = 'green'

    elif tl_light[0][0] == '12':

        green_max = 51
        # time to green, currently red
        if x6[0] == 'g':
            t1s = x6[1]
            t1e = t1s + green_max
            t2s = t1s + 100
            t2e = t2s + green_max
            r1s = 0
            curr_status = 'red'
        # time to red, currently green
        elif x6[0] == 'r':
            t1s = 0
            t1e = x6[1]
            t2s = t1e + (100 - green_max)
            t2e = t2s + green_max
            r1s = x6[1]
            curr_status = 'green'

    return t1s, t1e, t2s, t2e, r1s, curr_status

def process_spat_for_gen_speed(tl_light,x1,x2,x3,x4,x5,x6):
                
        if tl_light[0][0] == '2':
                                
            green_max = 24
            if x1 == None:
                t1s = 0
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'green'
            # time to green, currently red
            elif x1[0] == 'g':
                t1s = x1[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x1[0] == 'r':
                t1s = 0
                t1e = x1[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x1[1]
                curr_status = 'green' 
            
        elif tl_light[0][0] == '3':

            green_max = 64
            # time to green, currently red
            if x2[0] == 'g':
                t1s = x2[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x2[0] == 'r':
                t1s = 0
                t1e = x2[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x2[1]
                curr_status = 'green' 

        elif tl_light[0][0] == '10':

            green_max = 46
            # time to green, currently red
            if x3[0] == 'g':
                t1s = x3[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x3[0] == 'r':
                t1s = 0
                t1e = x3[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x3[1]
                curr_status = 'green' 
        
        elif tl_light[0][0] == '8':

            green_max = 38
            # time to green, currently red
            if x4[0] == 'g':
                t1s = x4[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x4[0] == 'r':
                t1s = 0
                t1e = x4[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x4[1]
                curr_status = 'green' 

        elif tl_light[0][0] == '9':

            green_max = 68    
            # time to green, currently red
            if x5[0] == 'g':
                t1s = x5[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x5[0] == 'r':
                t1s = 0
                t1e = x5[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x5[1]
                curr_status = 'green' 

        elif tl_light[0][0] == '12':

            green_max = 51
            # time to green, currently red
            if x6[0] == 'g':
                t1s = x6[1]
                t1e = t1s + green_max
                t2s = t1s + 100
                t2e = t2s + green_max
                r1s = 0
                curr_status = 'red'
            # time to red, currently green
            elif x6[0] == 'r':
                t1s = 0
                t1e = x6[1]
                t2s = t1e + (100 - green_max)
                t2e = t2s + green_max 
                r1s = x6[1]
                curr_status = 'green' 

        return t1s, t1e, t2s, t2e, r1s, curr_status
    