# BLE Routine
#  
# https://ianharvey.github.io/bluepy-doc/
# https://github.com/TrinaryLabs/blepy
#
# Python installation: pip3 install bluepy

from bluepy.btle import Peripheral
import struct

# BLE Routine

def init_ble_routine(driver):
	'''Establish connection to device 'mac_address', its 'service UUID' and 'resource UUID'.'''

	carrier_server = None
	carrier_service = None
	carrier_resource = None

	try:
		carrier_server = Peripheral(driver.configs.source_mac_address)
		_ = carrier_server.services

		carrier_service = carrier_server.getServiceByUUID(
				driver.configs.source_service_UUID)
		_ = carrier_service.getCharacteristics()

		carrier_resource = carrier_service.getCharacteristics(
				driver.configs.source_resource_UUID)[0]

		# The 'bluepy' module isn't capable of reading and interpret GATT correctly. Below there's
		# quite some Python magic going on for solving this issue.
		# 
		# Incoming value from the resource (BLE lingo: 'characteristic') is HEX and converted into
		# a bytearray.

		byte_sample = carrier_resource.read()
		byte_sample = bytearray(byte_sample)

		# Furthermore, It happens to be I'm relaying float values within my BLE network but this
		# precision isn't really necessary. I want an int, and this 'struct.unpack' stuff
		# I stumbled upon does this under the hood by somehow.

		[float_value] = struct.unpack('f', byte_sample)
		driver.curr_vessel_value = int(float_value)

		carrier_server.disconnect()

		print('ble routine\t\t- connect\t\t-- succ -- "{}"'.format(driver.configs.source_server_name))
		print("ble routine\t\t- read value\t\t-- succ -- value: {}".format(driver.curr_vessel_value))
		
		driver.central_state = driver.STATE_SCPP
		
	except:
		# If the routine could't find and/or connect to the credentials given in 'configs' about the
		# Carrier Device, 'STATE_BLEP' (for consistency) will be reassigned 'STATE_BLEP' and retry
		# to connect in the next cycle.   
		if (carrier_server):
			carrier_server.disconnect()
		driver.central_state = driver.STATE_BLEP
