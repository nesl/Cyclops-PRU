from bottle import route, get, put, view, post, run, template, request, redirect, static_file
import re
import os
from multiprocessing import Process, Manager, Queue
from pru_process import pru_process

@get('/pru')
@get('/pru/<pru_num>')
@view('editor')
def route_editor(pru_num=0):
	return dict(pru_num=pru_num)

@route('/static/<filepath:path>')
def server_static(filepath):
    return static_file(filepath, root='static')

@get('/pru/<pru_num>/out')
def route_output(pru_num):
	if int(pru_num) == 0:
		buf_out = pru_0_out
	else:
		buf_out = pru_1_out
	output = ""
	for i in range(10):
		try:
			output += buf_out.get_nowait() + "\n"
		except:
			break
	return output

@post('/pru/<pru_num>/assemble')
def route_assemble(pru_num):
	src = request.forms.getunicode('src')
	shared['req' + str(pru_num)] = 'assemble'
	shared['src' + str(pru_num)] = src	

@post('/pru/<pru_num>/run')
def route_run(pru_num):
	src = request.forms.getunicode('src')
	shared['req' + str(pru_num)] = 'run'
	shared['src' + str(pru_num)] = src	

@put('/pru/<pru_num>/stop')
def route_stop(pru_num):
	shared['req' + str(pru_num)] = 'stop'
	shared['src' + str(pru_num)] = None

manager = Manager()
shared = manager.dict()
pru_0_out = manager.Queue()
pru_1_out = manager.Queue()
pru_0_out.put_nowait("PRU0 Enabled\n")
pru_1_out.put_nowait("PRU1 Enabled\n")
process_0 = Process(target=pru_process, args=(0, pru_0_out, shared))
process_0.start()
process_1 = Process(target=pru_process, args=(1, pru_1_out, shared))
process_1.start()

run(host='192.168.7.2', port=8081, quiet=True)
