#!/usr/bin/python
import cgi
import os
import datetime
form = cgi.FieldStorage()
name = form.getvalue('name')

user_cookie = os.environ.get('HTTP_COOKIE', '')

def get_username_from_cookie(cookie):
    username = ""
    cookie_list = cookie.split('; ')
    for item in cookie_list:
        if item.startswith('user='):
            username = item[5:]
    return username

if name:
    expiration_date = datetime.datetime.now() + datetime.timedelta(days=30)  # Expires in 30 days
    expiration_str = expiration_date.strftime("%a, %d-%b-%Y %H:%M:%S GMT")
    cookie_str = f"user={name}; expires={expiration_str};"
    print(f"Set-Cookie: {cookie_str}")
print("Content-type: text/html\n")
username = get_username_from_cookie(user_cookie)
print("<html>")
print("<head><title>Cookie Example</title></head>")
print("<body>")
print("<h1>Cookie Example</h1>")

if username:
    print("<p>Welcome back, %s!</p>" % username)
else:
    print("<p>No cookie found.</p>")

print("<form method='get' action=''>")
print("<label for='name'>Enter your name: </label>")
print("<input type='text' name='name' id='name' />")
print("<input type='submit' value='Submit' />")
print("</form>")
print("</body>")
print("</html>")
