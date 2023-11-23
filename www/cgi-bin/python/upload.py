import cgi
import os

form = cgi.FieldStorage()
destination_directory = "/tmp/"
upload_statuses = []

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
print("Content-Type: text/plain\r")
print("\r")
if upload_statuses:
    for status in upload_statuses:
        print(status)
else:
    print("No files uploaded.")
