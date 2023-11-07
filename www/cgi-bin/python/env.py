#!/usr/bin/python
# print ("HTTP/1.1 200 OK\r");
# print ("HTTP/1.1 200 OK\r");
print("Content-type: text/html\r\n\r")
print("<html>")
print("<head><title>Environment Variables</title></head>")
print("<body>")
print("<h1>Environment Variables</h1>")
print("<ul>")
import os
for key, value in os.environ.items():
    print(f"<li><strong>{key}:</strong> {value}</li>")
print("</ul>")
print("</body>")
print("</html>")
# while(1):
#     print("hello")
