#!/usr/local/bin/micropython

from sys import stdin
from array import array

print("""HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Connection: close\r\n\
\r\n""")

buf = array('B', [0])
data = ''

while stdin.readinto(buf):
	data += chr(buf[0])

print("nuuul")
print("<pre>" + data + "</pre>")
