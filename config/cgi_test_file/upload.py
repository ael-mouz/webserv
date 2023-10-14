#!/usr/bin/env python

import os
import cgi

# Define the path to save uploaded files
upload_dir = "./uploads"

# Ensure the upload directory exists
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()

# Check if "file" is in the form data
if "file" in form:
    fileitem = form["file"]
    if fileitem.filename:
        # Process the uploaded file
        filename = os.path.join(upload_dir, os.path.basename(fileitem.filename))
        with open(filename, "wb") as f:
            f.write(fileitem.file.read())
        print("Content-Type: text/html\n\n")
        print(f"File '{filename}' has been uploaded and saved.")
    else:
        print("Content-Type: text/html\n\n")
        print("No file was uploaded.")
else:
    print("Content-Type: text/html\n\n")
    print("No file was uploaded.")
