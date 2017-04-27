from bottle import route, get, post, run, template, request, redirect
import re
from pins import P8, P9
import os
import subprocess
import time

BASE_ADDR = 0x44e10800
SLOTS_PATH = '/sys/devices/bone_capemgr.9/slots'

def search_dict(dictionary, func):
	for key in dictionary:
		if func(dictionary[key]):
			return dictionary[key]
	return None

def read_pin_status():
	f = open("/sys/kernel/debug/pinctrl/44e10800.pinmux/pins", "r")
	for line in f:
		m = re.search("pin [0-9]+ \(([0-9A-Fa-f]+)\) ([0-9A-Fa-f]+)", line)
		if m:
			addr = int(m.group(1), base=16)
			cfg = int(m.group(2), base=16)
			offset = addr - BASE_ADDR
			for P in [P8, P9]:
				pin = search_dict(P, \
						lambda pin: pin['offset'] == offset)
				if pin:
					mode = int(cfg & 0b0000111)
					pin['mode'] = mode
					pin['enable_pullupdown'] = bool(cfg & 0b0001000)
					pin['pullup'] = bool(cfg & 0b0010000)
					pin['input'] = bool(cfg & 0b0100000)
					pin['slew'] = bool(cfg & 0b1000000)
					pin['cfg'] = cfg

	f.close()

def pruss_enabled():
	try:
		uio = '/sys/class/uio'
		for dir in os.listdir(uio):
			f = open(uio + "/" + dir + "/name", "r")
			if re.match("pruss.*", f.readline()):
				return True
			f.close()
	except:
		pass

	return False

def unload_slot(name):
	ret = False
	f = open(SLOTS_PATH, "r")
	for line in f:
		m = re.search("([0-9]+): .*" + name + "$", line)
		if m:
			index = int(m.group(1))
			f.close()
			f = open(SLOTS_PATH, "a")
			f.write("-" + str(index))
			f.flush()
			ret = True
			break
	f.close()
	return ret

def load_slot(name):
	f = open(SLOTS_PATH, "a")
	f.write(name)
	f.flush()
	f.close()

@get('/pru/pinmux')
def route_pinmux():
	read_pin_status()
	return template("pinmux", P8=P8, P9=P9, pruss_enabled=pruss_enabled())

@post('/pru/pinmux')
def route_pinmux_post():
	read_pin_status()
	form = request.forms
	dts_name = "PRU-ACTIVATE"
	dts_ver = "00A0"
	enable_pruss = bool(form.getunicode("enable_pruss"))
	status = 'okay' if enable_pruss else 'disabled'
	pins = []

	for P in [P8, P9]:
		if P == P8:
			prefix="P8_"
		else:
			prefix = "P9_"

		for i in range(1, 46):
			bits = 0;
			mode = form.getunicode(prefix + str(i) + "_mode")
			enable_pullupdown = form.getunicode(prefix + str(i) + "_enable_pullupdown")
			pullup = form.getunicode(prefix + str(i) + "_pullup")
			input = form.getunicode(prefix + str(i) + "_input")
			slew = form.getunicode(prefix + str(i) + "_slew")

			if mode != None:
				bits = bits | int(mode)
				if enable_pullupdown:
					bits = bits | 0b0001000
				if pullup:
					bits = bits | 0b0010000
				if input:
					bits = bits | 0b0100000
				if slew:
					bits = bits | 0b1000000

				offset = P[i]['offset']
				pru_assignable = bool(filter(lambda m: "pru" in str(m), \
							P[i]['modes']))
				if pru_assignable and offset != None and bits != P[i]['cfg']:
					pin = {}
					if P == P8:
						pin['header'] = "P8"
					else:
						pin['header'] = "P9"
					pin['pin_num'] = i
					pin['offset'] = offset
					pin['bits'] = bits
					pins.append(pin)

	if pins or pruss_enabled() != enable_pruss:
		unload_slot(dts_name)

		dts = template(dts_name, part_num=dts_name, version=dts_ver, \
						pins=pins, status=status)
		root_firmware_dir = '/lib/firmware/'
		dts_filename = dts_name + "-" + dts_ver + ".dts"
		full_dts_path = root_firmware_dir + dts_filename
		f = open(full_dts_path, "w")
		f.write(dts)
		print dts
		f.flush()
		f.close()
	
		dtbo_filename = dts_name + "-" + dts_ver + ".dtbo"
		full_dtbo_path = root_firmware_dir + dtbo_filename
		subprocess.call(["dtc", "-O", "dtb", "-I", "dts", "-o", full_dtbo_path, "-b", \
						 "0", "-@", full_dts_path])

		load_slot(dts_name)

		time.sleep(0.25)

	redirect('/pru/pinmux')

run(host='192.168.7.2', port=8081)
