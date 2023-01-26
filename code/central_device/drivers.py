import time
from configs import CentralConfig
from ble_routine import init_ble_routine
from aws_routine import init_aws_routine
from scp_routine import init_scp_routine

# Drivers

class DeviceDriver(object):
    ''''''

    STATE_BLEP = 0  # State BLE Pipe
    STATE_AWSP = 1  # State AWS (IoT Core) Pipe
    STATE_SCPP = 2  # State SCP (Secure Copy) Pipe 
    SLEEP_MODE = 3  # Sleep
    ERROR_MODE = 4  # Panic

    def __init__(self):
        
        self.configs = CentralConfig()

        self.curr_vessel_value = 0
        self.prev_vessel_value = 0

        self.curr_vessel_state = 0
        self.prev_vessel_state = 0

        self.flag_vessel_event = 0 
        self.flag_vessel_alarm = 0

        self.sample_stamp = None

        self.attempt = 0
        self.central_state = 0

    def evaluate_data(self):
        '''Evaluate fetched sensor data (relayed from the (a) Carrier Device).'''

        STATUS_RED = 2
        STATUS_YEL = 1
        STATUS_GRN = 0

        if (self.curr_vessel_value == self.prev_vessel_value):
            # No changes detected, return.
            print("central driver\t\t- evaluate\t\t-- succ -- no changes detected")
            return

        print("central driver\t\t- evaluate\t\t-- succ -- vessel change detected")
        self.prev_vessel_value = self.curr_vessel_value

        if (self.curr_vessel_value <= self.configs.threshold_red):
            # 'curr_vessel_value' lesser than or equal to 'threshold_red' means
            # low distance (vessel full).
            print("central driver\t\t- evaluate\t\t-- succ -- vessel full")
            self.curr_vessel_state = STATUS_RED
            self.flag_vessel_alarm == 1

        elif (self.curr_vessel_value > self.configs.threshold_yel):
            # 'curr_vessel_value' larger 'yel' means high distance, "empty".
            self.curr_vessel_state = STATUS_GRN
            self.flag_vessel_alarm == 0

        else:
            # 'curr_vessel_state' be STATUS_YEL (~ half-full vessel).
            self.curr_vessel_state = STATUS_YEL
            self.flag_vessel_alarm == 0

        if (self.curr_vessel_state != self.prev_vessel_state):
            # 'curr_vessel' differs from 'last_vessel' means changed state.

            print("central driver\t\t- evaluate\t\t-- succ -- vessel state updated")
            self.prev_vessel_state = self.curr_vessel_state
            self.flag_vessel_event = 1
        else:
            self.flag_vessel_alarm == 0

    def set_mqtt_mesg(self):
        '''Set mqtt message to be sent, created out of latest sensor values'''

        self.mqtt_mesg = {
			'vessel_value': self.curr_vessel_value,
			'vessel_state': self.curr_vessel_state,
			'vessel_event': self.flag_vessel_event,
			'vessel_alarm': self.flag_vessel_alarm,
		}

    def set_export_item(self):

        '''Set an export item based on latest vessel values + configs/meta.'''

        self.export_item = {
            
            # Because it will be an "import stamp" server side.
            'import_stamp': time.strftime("%d-%m-%Y %H:%M:%S"),
                
            'device_name': self.configs.device_name,
            'device_UUID': self.configs.device_UUID,

            'sensor_name': self.configs.sensor_name,
            'sensor_regn': self.configs.sensor_regn,
            'sensor_munp': self.configs.sensor_munp,
            'sensor_dist': self.configs.sensor_dist,
            'sensor_mesh': self.configs.sensor_mesh,
            'sensor_vess': self.configs.sensor_vess,
            
            'vessel_value': self.curr_vessel_value,
            'vessel_state': self.curr_vessel_state,
            'vessel_event': self.flag_vessel_event,
            'vessel_alarm': self.flag_vessel_alarm,
        }

def device_drivers():
    '''Central Device Statemachine.'''
    
    print("central driver\t\t- begin\t\t-- succ -- let's do this thing.")
    driver = DeviceDriver()

    while (driver.central_state != driver.ERROR_MODE):

        if (driver.central_state == driver.STATE_BLEP):
            # Try establish connection with the (a) "Carrier".
            init_ble_routine(driver)

        elif (driver.central_state == driver.STATE_AWSP):
            # Try to establish connection with AWS IoT Core.
            driver.evaluate_data()
            init_aws_routine(driver)

        elif (driver.central_state == driver.STATE_SCPP):
            driver.evaluate_data()
            init_scp_routine(driver)

        else:
            print("\ncentral driver\t\t- sleep mode\t\t-- init --")
            time.sleep((driver.configs.SLEEP_TIMER))

            print("central driver\t\t- sleep mode\t\t-- exit --\n")
            driver.central_state = driver.STATE_BLEP

    print("central driver\t\t- error mode\t\t-- FAIL -- Terminating")    
