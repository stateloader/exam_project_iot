# Configs
#
# https://linuxize.com/post/how-to-use-scp-command-to-securely-transfer-files/
# https://www.section.io/engineering-education/how-to-execute-linux-commands-in-python/

class CentralConfig(object):
    '''Central Device Configs'''

    ATTEMPT_LIMIT = 16
    SLEEP_TIMER   = 30

    def __init__(self):
        '''Attributes that should cover everything that makes this Central running.'''

        # Thresholds for triggering an event (vessel update) based on distances between
        # sensor and waste.
        self.threshold_red              = 150
        self.threshold_yel              = 200

        # Device. Credentials and data about the (THIS) Central Device.
        self.device_name                = 'central_jacke'
        self.device_UUID                = '13b6f0ed-fbc2-467d-b108-e81a49bdd362'
        
        self.secret_root_c1_crt         = 'secrets/aws-root-c1.pem'
        self.secret_private_key         = 'secrets/device-priv.pem.key'
        self.secret_private_crt         = 'secrets/device-cert.pem.crt'

        # Source. A Carrier Device the (THIS) Central device fetches relayed sensor data from.
        self.source_server_name         = 'floor3_server'
        self.source_mac_address         = '7C:9E:BD:B3:F9:36'
        self.source_service_UUID        = '40f7c1a4-8f97-11ed-a1eb-0242ac120002'
        self.source_resource_UUID       = '4cd1a9fe-8f97-11ed-a1eb-0242ac120002'

        # AWS IoT Core target (for MQTT)
        self.target_aws_endpoint        = 'a1qt5qae1xcxwq-ats.iot.eu-north-1.amazonaws.com'

        # Sensor. Position data regarding the (a) Sensor Device within the mesh. Yes, it's just
        # one (1) sensor device involved in this project. All "ok..." attributes in place for
        # proving a point.
        self.sensor_name                = 'sensor_device'
        self.sensor_regn                = 'stockholm'
        self.sensor_munp                = 'solna_stad'
        self.sensor_dist                = 'frosunda'
        self.sensor_mesh                = 'mesh_13'
        self.sensor_vess                = 'vessel_1'

        # Elements for creating a SCP command where a json item will be sent to a remote server.
        self.export_file                = 'resource/export.json'
        self.remote_user                = # 
        self.remote_ip                  = #
        self.remote_path                = #

    def set_mqtt_topic(self):
        '''Mqtt topic created out of the "Sensor" attributes.'''

        self.mqtt_topic = "{}/{}/{}/{}/{}/{}/data".format(
			self.sensor_name, self.sensor_regn, self.sensor_munp,
            self.sensor_dist, self.sensor_mesh, self.sensor_vess,
		)
