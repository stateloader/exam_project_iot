import json
import os

# SCP Routine
#
# At frist I tried to create a shell-script utilizing crontab + commands but it didn't worked
# out (for me). Instead, this rutine using the Secure Copy command to export latest export
# item to a remote server.
# --------------------------------------------------------------------------------------------

def create_export_file(driver):
	'''Create a json file and dumps the export item in it.'''

	driver.set_export_item()

	try:
		with open(driver.configs.export_file, 'w') as json_file:
			json.dump(driver.export_item, json_file, indent = 4)

		print('scp routine\t\t- create\t\t-- succ -- item')
		return True

	except:
		print('scp routine\t\t- create\t\t-- fail -- item')
		return False


def export_export_file(driver):
	'''Export it to the remote path (In "Effektivt soprum" a simple django site/app.)'''

	bash_cmnd = "scp {} {}@{}:{}".format(
		driver.configs.export_file, driver.configs.remote_user, driver.configs.remote_ip,
		driver.configs.remote_path
	)

	try:
		os.system(bash_cmnd)
		print('scp routine\t\t- terminal\t\t-- succ -- file exported')

	except:
		print('scp routine\t\t- terminal\t\t-- fail -- exporting file')

def init_scp_routine(driver):
	'''kkk'''

	driver.central_state = driver.SLEEP_MODE
	if (not create_export_file(driver)):
		return