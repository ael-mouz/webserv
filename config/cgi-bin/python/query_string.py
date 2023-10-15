#!/usr/bin/python
import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
name = form.getvalue('name')
email = form.getvalue('email')

# Print the HTML content with the received data
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