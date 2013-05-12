# -*- coding: iso-8859-1 -*-
import httplib
import time
from sys import stdout

conn = httplib.HTTPConnection('127.0.0.1', 8001)
conn.request("GET", "/5178eab8")
r1 = conn.getresponse()
print(r1.status, r1.reason)
default_timer = time.time
deadline = 0
data_size = 0
while True:
	r1.read(1) # bytes
