#!/usr/local/bin/micropython

from sys import stdin

print("""HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Connection: close\r\n\
\r\n""")

data = stdin.read()

print("Hello World !")
print("<pre>" + data + "</pre>")
