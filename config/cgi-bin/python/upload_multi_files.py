import cgi
import os
import sys

form = cgi.FieldStorage()
destination_directory = "/Users/ael-mouz/Desktop/webserv/upload"
upload_statuses = []

print(form, file=sys.stderr)

print(sys.stdin.read(), file=sys.stderr)

for key, value in os.environ.items():
    print(f"{key}: {value}", file=sys.stderr)

for field_name in form.keys():
    if isinstance(form[field_name], cgi.FieldStorage) and form[field_name].filename:
        fileitem = form[field_name]
        destination_path = os.path.join(destination_directory, fileitem.filename)
        try:
            with open(destination_path, 'wb') as file:
                file.write(fileitem.file.read())
            upload_statuses.append(f"File '{fileitem.filename}' uploaded successfully.")
        except Exception as e:
            upload_statuses.append(f"Error uploading file '{fileitem.filename}': {str(e)}")
print ("HTTP/1.1 200 OK\r");
print("Content-Type: text/plain\r")
print("\r")
if upload_statuses:
    for status in upload_statuses:
        print(status)
else:
    print("No files uploaded.")