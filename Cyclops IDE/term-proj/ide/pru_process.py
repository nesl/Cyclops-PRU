import subprocess
import os
import time

def pru_process(pru_num, buf_out, shared):
	def check_stop(pru_num):
		if shared.get('req' + str(pru_num)) == 'stop':
			shared['req' + str(pru_num)] = None
			return True
		else:
			return False
	def stop_process(process):
		try:
			if process and process.poll() is None:
				process.terminate()
				sleep(0.15)
				if process.poll() is None:
					process.kill()
		except OSError as e:
			print(e)

	while True:
		time.sleep(0.1)
		check_stop(pru_num)
		src_key = 'src' + str(pru_num)
		src = shared.get(src_key)
		if src:
			shared[src_key] = None
			print("PRU " + str(pru_num) + " compile this: {0}".format(src))
			cur_path = os.path.dirname(os.path.realpath(__file__))	
			tmp_dir = cur_path + "/_tmp/"
			try:
				os.mkdir(tmp_dir)
				print("mkdir " + tmp_dir)
			except OSError as e:
				print(e)
			try:
				src_file = open(tmp_dir + "pru_" + str(pru_num) + ".p", "w")
				src_file.write(src)
				src_file.close()
			except IOError as e:
				print(e)
			
			if shared.get('req' + str(pru_num)) == 'assemble':
				process = subprocess.Popen([cur_path + "/../scripts/compile-pru.sh", str(pru_num)], \
							stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			elif shared.get('req' + str(pru_num)) == 'run':
				process = subprocess.Popen([cur_path + "/../scripts/compile-pru.sh", str(pru_num), 'run'], \
							stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

			while True:
				if check_stop(pru_num):
					break
				output = process.stdout.readline()
				if output == '' and process.poll() is not None:
					break
				else:
					if output:
						print str(pru_num) + '@ ' + output.rstrip()
						buf_out.put_nowait(output.rstrip())
			stop_process(process)
