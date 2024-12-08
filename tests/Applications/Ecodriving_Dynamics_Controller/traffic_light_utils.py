import traci
import numpy as np

class TrafficLight:

    def __init__(self, tlid, sumoSignalConfig):

        self.tlid = tlid
        self.cycle_len = 100

        # get the phase config for the intersection
        self.tlid_phase_config = sumoSignalConfig[sumoSignalConfig['id'] == tlid][['minDur', 'maxDur', 'vehext',
                                                                 'yellow', 'red', 'name',
                                                                 'approach_direction']].set_index('name').to_dict('index')

        self.tracking_phase_list = list(self.tlid_phase_config.keys())

        self.green_ct = {phase: 0 for phase in self.tracking_phase_list}
        self.red_ct = {phase: self.cycle_len - self.tlid_phase_config[phase]['maxDur'] for phase in self.tracking_phase_list}
        self.spat_status = {}
        # self.initialized = False
        self.initialized = {phase: False for phase in self.tracking_phase_list}
        self.step_length = traci.simulation.getDeltaT()

    def get_remaining_green(self):

        # self.red_ct is for red countdown
        # self.green_ct is for current time in green
        # print('Test', traci.trafficlight.getPhaseName(self.tlid))
        if any(phase in traci.trafficlight.getPhaseName(self.tlid) for phase in self.tracking_phase_list):
            current_green_phases = traci.trafficlight.getPhaseName(self.tlid).split("+")
            for phase in self.tracking_phase_list:
                if phase in current_green_phases:
                    self.initialized[phase] = True
                    self.green_ct[phase] += self.step_length

                    time_to_red = np.clip((self.tlid_phase_config[phase]['maxDur'] - self.green_ct[phase]),
                                          0,
                                          self.tlid_phase_config[phase]['maxDur'])

                    if time_to_red == 0:
                        time_to_red += self.tlid_phase_config[phase]['vehext']
                        self.red_ct[phase] = self.cycle_len - self.green_ct[phase]
                    else:
                        self.red_ct[phase] = self.cycle_len - self.tlid_phase_config[phase]['maxDur']

                    self.spat_status[phase] = ('r', time_to_red)
                else:
                    self.green_ct[phase] = 0
                    if self.red_ct[phase] > 0:
                        self.red_ct[phase] -= self.step_length
                    else:
                        self.red_ct[phase] += self.tlid_phase_config[phase]['vehext']

                    if self.initialized[phase]:
                        # print('Time to green at 2', self.other_ct)
                        # return ({'t2g' : self.red_ct},self.red_ct)
                        self.spat_status[phase] = ('g', self.red_ct[phase])
        else:
            for phase in self.tracking_phase_list:
                self.green_ct[phase] = 0

                if self.red_ct[phase] > 0:
                    self.red_ct[phase] -= self.step_length
                else:
                    self.red_ct[phase] += self.tlid_phase_config[phase]['vehext']

                if self.initialized[phase]:
                    # print('Time to green at 2', self.other_ct)
                    # return ({'t2g' : self.red_ct},self.red_ct)
                    self.spat_status[phase] = ('g', self.red_ct[phase])