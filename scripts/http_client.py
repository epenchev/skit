# -*- coding: iso-8859-1 -*-
import httplib
import time
from sys import stdout

conn = httplib.HTTPConnection('84.40.124.230', 8009)
conn.request("GET", "/btv_action")
r1 = conn.getresponse()
print(r1.status, r1.reason)
default_timer = time.time
deadline = 0
data_size = 0
while True:
	start = default_timer()
	r1.read(1024) # bytes
	finish = default_timer()
	deadline += (finish - start)
	data_size += 1024
	#print('%.3f' % (finish - start))
	if deadline >= 1.0:
		#print('%.3f' % deadline)
		#print('size is %.d' % data_size)
		#print("speed %.d Kb/s" % (data_size * 8 / 1024))
		stdout.write("%04d\b\b\b\b\b\b\b " % (data_size * 8 / 1024) )
		#stdout.write("speed %.d Kb/s" % (data_size * 8 / 1024))
		#stdout.write("\033[K")
		stdout.flush()
		deadline = 0
		data_size = 0
