#!/usr/bin/python

import cgi
import os

path_info = os.environ.get('PATH_INFO', '/')
form = cgi.FieldStorage()
actions = {
    '/': lambda: 'Welcome to the homepage',
    '/about': lambda: 'This is the about page',
    '/contact': lambda: 'Contact us at contact@example.com',
}
action_function = actions.get(path_info, None)
print("HTTP/1.1 200 OK\r");
print("Content-type: text/html\n")
print("<html>")
print("<head><title>Path Info Form</title></head>")
print("<body>")
print("<h1>Path Info Form</h1>")
if action_function:
    result = action_function()
    print("<p>%s</p>" % result)
else:
    print("<p>Page not found</p>")
print("</body>")
print("</html>")
