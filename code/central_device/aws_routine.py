# AWS Routine
#
# Using AWS python SDK for this one. Nifty stuff. I've followed their own example how to use
# it (link 1) and how to deal with credentials (link 2).
#
# 1) https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html
# 2) https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-raspi-setup.html
#
# Python installation: pip3 install AWSIoTPythonSDK
#
# --------------------------------------------------------------------------------------------
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import json

def init_aws_routine(driver):
	'''Establish connection with AWS IoT Core and publish 'mqtt_mesg' to 'mqtt_topic'.'''

	try:

		client = AWSIoTMQTTClient('central_test')
		print('aws routine\t\t- client\t\t-- succ -- create')

		client.configureEndpoint(driver.configs.target_aws_endpoint, 8883)
		print('aws routine\t\t- endpoint\t\t-- succ -- config')

		client.configureCredentials(
			driver.configs.secret_root_c1_crt, driver.configs.secret_private_key,
			driver.configs.secret_private_crt
		)

		print('aws routine\t\t- secrets\t\t-- succ -- config')
		client.connect()

		# Quite rendundant to set topic over and over but I've plans for making it
		# possible to change this in runtime ... later on.
		driver.configs.set_mqtt_topic()
		driver.set_mqtt_mesg()	

		client.publish(driver.configs.mqtt_topic, json.dumps(driver.mqtt_mesg), 0)
		print('aws routine\t\t- publish\t\t-- succ -- message sent')

		client.disconnect()
		driver.attempt = 0

		driver.central_state = driver.STATE_SCPP

	except:

		if (driver.attempt == driver.configs.ATTEMPT_LIMIT):
			print('aws routine\t\t- execution\t\t-- fail -- Terminating\n'.format(
				driver.attempt))
			driver.central_state = driver.ERROR_MODE

		else:
			print('aws routine\t\t- execution\t-- fail -- attempt: {}\n'.format(
				driver.attempt))
			driver.attempt += 1
			driver.central_state = driver.STATE_AWSP