#!/usr/bin/python
import cgi

form = cgi.FieldStorage()
name = form.getvalue('name')
email = form.getvalue('email')
print("Content-type: text/html\n")
print("<html>")
print("<head><title>Query String Form</title></head>")
print("<body>")
print("<h1>Query String Form</h1>")
print("<p>Received data:</p>")
print("<ul>")
print("<li><strong>Name:</strong> %s</li>" % (name or "Not provided"))
print("<li><strong>Email:</strong> %s</li>" % (email or "Not provided"))
print("</ul>")
print("</body>")
print("</html>")
