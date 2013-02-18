import httplib
conn = httplib.HTTPConnection('127.0.0.1', 9999)
conn.request("GET", "/btv")
r1 = conn.getresponse()
print(r1.status, r1.reason)
while True:
	r1.read(512) # 512 bytes