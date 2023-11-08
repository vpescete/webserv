import os
import subprocess
import sys

# OLD NON USARE SE SI RIESCE








# Parse the incoming HTTP request
request = sys.stdin.read()
method, path, query = request.split(' ', 2)
path, _, query = path.partition('?')

# Set up the environment variables required by the CGI specification
os.environ['REQUEST_METHOD'] = method
os.environ['SCRIPT_NAME'] = os.environ.get('SCRIPT_NAME', '')
os.environ['PATH_INFO'] = path
os.environ['QUERY_STRING'] = query
os.environ['CONTENT_TYPE'] = os.environ.get('CONTENT_TYPE', '')
os.environ['CONTENT_LENGTH'] = str(len(request))

# If the request method is POST, read the request body and set the CONTENT_LENGTH environment variable accordingly
if method == 'POST':
    os.environ['CONTENT_LENGTH'] = str(len(sys.stdin.buffer.read()))

# Execute the CGI script as a subprocess, passing the environment variables and request body as input
script_path = '/www/cgi/upload.py'
process = subprocess.Popen(['python', script_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=os.environ)
output, error = process.communicate(input=request.encode())

# Send an HTTP response with the output of the CGI script as the response body
print(output.decode())
