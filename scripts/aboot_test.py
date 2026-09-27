# Copyright (c) 2013, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of The Linux Foundation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#!/usr/bin/python

import os
import sys
import time
import re
import subprocess
import getopt

#
# Erase routine, erase a parition & print the time taken for erase
#
def fastboot_erase(partition):
	start_time = time.time()
	exe = subprocess.Popen(['fastboot', 'erase', partition], stdout=subprocess.PIPE,stderr=subprocess.STDOUT).stdout.read()
	print exe
	print "Time Taken for erase:", partition, ":", time.time() - start_time, "seconds"
	print("")
	return

#
# Flash routine, flash a parition & print the time taken to flash the image
#
def fastboot_flash(image_name, partition):
	start_time = time.time()
	exe = subprocess.Popen(['fastboot', 'flash', partition, image_name], stdout=subprocess.PIPE,stderr=subprocess.STDOUT).stdout.read()
	print exe
	print "Time Taken for flashing:", partition, ":", time.time() - start_time, "seconds"
	print("")
	return

#
# Execute any other fasboot command & print the time taken
#
def fastboot_exec(command):
	start_time = time.time()
	exe = subprocess.Popen(['fastboot', command], stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
	print exe
	print "Time Taken for fastboot:", command, time.time() - start_time, "seconds"
	print("")
	return

#
# Aboot test, Test aboot with different use cases
#
def test_aboot(iteration, input_path):
	system=''
	userdata=''
	boot=''

	print ("ABOOT TEST START")
	t0 = time.clock()

	boot = os.path.join(input_path, 'boot.img')
	system = os.path.join(input_path, 'system.img')
	userdata = os.path.join(input_path, 'userdata.img')

	print("")
	getstate = subprocess.Popen(["fastboot", "devices"], stdout=subprocess.PIPE).communicate()[0]
	if(re.search("fastboot",getstate) == None):
		print("Device is not in fastboot, please make sure device is in fastboot mode ... [FAIL]")
		sys.exit(-1)
	else:
		print ("fastboot devices ... [OKAY]")
		print ("Executing other fastboot tests ...")
		print("")

	fastboot_erase("boot")
	time.sleep(2)

	fastboot_exec("reboot")
	time.sleep(2)
	fastboot_exec("devices")
	time.sleep(2)
	getstate = subprocess.Popen(["fastboot", "devices"], stdout=subprocess.PIPE).communicate()[0]
	if(re.search("fastboot",getstate) == None):
		print("fastboot reboot ... [FAIL]")
		sys.exit(-1)

	iteration = int(iteration)

	# Flash images in a loop
	i = 0
	while i < iteration:
		print "Iteration ", i
		print ("fastboot flash boot boot.img...")
		fastboot_flash(boot, 'boot')
		print("")
		print ("fastboot flash system system.img ...")
		fastboot_flash(system, 'system')
		print("")
		print ("fastboot flash userdata userdata.img ...")
		fastboot_flash(userdata, 'userdata')
		print("")
		i+=1

	fastboot_exec("reboot")
	print ("fastboot reboot ... [OKAY]")
	print("")
	time.sleep(1)
	print("Waiting for adb to come up ...")
	print("")
	i = 0
	while i < 10:
		getstate = subprocess.Popen(["adb", "get-state"], stdout=subprocess.PIPE).communicate()[0]
		if(re.search("device",getstate) == None):
			i+=1
			time.sleep(2)
		else:
			print("Device Online")
			print("")
			break

	os.system("adb reboot-bootloader")
	time.sleep(4)
	getstate = subprocess.Popen(["fastboot", "devices"], stdout=subprocess.PIPE).communicate()[0]
	if(re.search("fastboot",getstate) == None):
		print ("adb reboot-bootloader ... [FAIL]")
		sys.exit(-1)
	else:
		print ("adb reboot-bootloader ... [PASS]")

	print("")
	fastboot_exec("devices")
	print ("fastboot devices ... [OKAY]")
	print("")

	fastboot_erase("system")
	fastboot_erase("userdata")

	fastboot_exec("continue")
	print ("fastboot continue ... [OKAY]")
	print("")

	print ("ABOOT TEST DONE")
	return

# Main function to parse i/p args
def main(argv):
	input_path = ''
	iteration = ''
	if len(sys.argv) < 2:
		print "aboot_test.py -i <iterations> -p <Binary Image Path>"
		sys.exit(2)
	try:
		opts, args = getopt.getopt(argv, "hi:p:",["iter=","opath="])
	except getopt.GetoptError:
		print "aboot_test.py -i <iterations> -p <Binary Image Path>"
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print "aboot_test.py -i <iterations> -o <Binary Image Path>"
			sys.exit(2)
		elif opt in ("-i", "--iter"):
			iteration = arg
		elif opt in ("-p", "--opath"):
			input_path = arg
	test_aboot(iteration, input_path)

if __name__ == "__main__":
   main(sys.argv[1:])
