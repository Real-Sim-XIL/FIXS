import traci
import numpy as np

class TrafficLight:

    def __init__(self, tlid):

        self.tlid = tlid
        self.max_cycle_len = 101

        self.green_ct = 0
        self.red_ct = 0

        self.tlid_phase_map = { '2' : ('2+5', '2+6'), '3' : ('2+6'), '10' : ('2+6'), 
                               '8' : ('2+6'), '9' : ('1+6', '2+6'), '12' : ('2+6')}
        
        self.max_green_dict = {'2' : 24, '3' : 64, '10': 46,
                               '8' : 38,  '9' : 68, '12': 51}

        self.initialized = False


    def get_remaining_green(self):
        
        if self.tlid == '2' or self.tlid == '9':

            if traci.trafficlight.getPhaseName(self.tlid) == self.tlid_phase_map[self.tlid][0] or traci.trafficlight.getPhaseName(self.tlid) == self.tlid_phase_map[self.tlid][1]:
                self.initialized = True
                self.green_ct += 1
                
                time_to_red = np.clip((self.max_green_dict[self.tlid] - self.green_ct), 0, self.max_green_dict[self.tlid])
                
                if time_to_red == 0:
                    self.red_ct -= 1
                else:
                    self.red_ct = self.max_cycle_len - self.max_green_dict[self.tlid]
                
                # return ({'t2r' : time_to_red},time_to_red)
                # print('time to red : ', time_to_red)
                return ('r', time_to_red)

            else:
                self.red_ct -= 1 
                self.green_ct = 0

                if self.initialized:
                    # print('Time to green at 2', self.other_ct)
                    # return ({'t2g' : self.red_ct},self.red_ct) 
                    return ('g', self.red_ct)
        
        else:
            
            if traci.trafficlight.getPhaseName(self.tlid) == self.tlid_phase_map[self.tlid]:
                self.initialized = True
                self.green_ct += 1
                
                time_to_red = np.clip((self.max_green_dict[self.tlid] - self.green_ct), 0, self.max_green_dict[self.tlid])
                
                if time_to_red == 0:
                    self.red_ct -= 1
                else:
                    self.red_ct = self.max_cycle_len - self.max_green_dict[self.tlid]
                # print('time to red : ', time_to_red)
                # return ({'t2r' : time_to_red}, time_to_red)
                return ('r',time_to_red)
            
            else:
                self.red_ct -= 1 
                self.green_ct = 0

                if self.initialized:
                    # print('Time to green at 2', self.other_ct)
                    # return ({ 't2g' : self.red_ct},self.red_ct) 
                    return ('g',self.red_ct)
        