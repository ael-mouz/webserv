#!/usr/bin/python

import cgi
form = cgi.FieldStorage()
your_name = form.getvalue('ayman')
company_name = form.getvalue('1337')
# print ("HTTP/1.1 200 OK");
print("Content-type: text/html\r\n\r")
print("<html>")
print("<head>")
print("<title>First CGI Program</title>")
print("</head>")
print("<body>")
print("<h2>Hello, {} {}</h2>".format("ayman", "weld lbyout"))
print("</body>")
print("</html>")