#!/usr/bin/env python

import sys
import os
import re

def generate_response(post_data):
    # Extract the boundary
    boundary_match = re.search(r'boundary=(.*?)(\r|$)', os.environ.get('CONTENT_TYPE', ''))
    if boundary_match:
        boundary = boundary_match.group(1)
    else:
        boundary = "--------------------------"  # Default boundary

    # Split the post_data into parts using the boundary
    parts = post_data.split('--' + boundary)

    response = "Content-Type: text/plain\r\n\r\n"

    # Extract and process form data
    for part in parts[1:-1]:  # Skip the first and last empty parts
        if "filename" in part:  # File data
            filename_match = re.search(r'name="(.+?)"; filename="(.+?)"', part)
            if filename_match:
                field_name = filename_match.group(1)
                file_name = filename_match.group(2)
                response += f"Received file upload: Field Name: {field_name}, File Name: {file_name}\n"
        else:  # Regular form data
            name_match = re.search(r'name="(.+?)"\s*?\r\n\r\n(.+)', part, re.DOTALL)
            if name_match:
                field_name = name_match.group(1)
                field_value = name_match.group(2)
                response += f"Received form data: Field Name: {field_name}, Field Value:\n{field_value}\n"

    return response

if __name__ == '__main__':
    post_data = sys.stdin.read()
    response = generate_response(post_data)
    sys.stdout.write(response)
