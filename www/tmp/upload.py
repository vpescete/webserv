import cgi
import os

# Parse the incoming HTTP request
form = cgi.FieldStorage()

if os.environ['REQUEST_METHOD'] == 'GET':
    # If the request method is GET, generate an HTML form that allows the user to select a file to upload
    print('Content-Type: text/html')
    print()
    print('<html>')
    print('<body>')
    print('<form enctype="multipart/form-data" method="post">')
    print('<input type="file" name="file">')
    print('<input type="submit" value="Upload">')
    print('</form>')
    print('</body>')
    print('</html>')
elif os.environ['REQUEST_METHOD'] == 'POST':
    # If the request method is POST, read the uploaded file from the request body and save it to a file on the server
    fileitem = form['file']
if fileitem.filename:
	filename = os.path.basename(fileitem.filename)
	with open(f'/uploads/{filename}', 'wb') as f:
		f.write(fileitem.file.read())
	message = 'Upload successful'
else:
	message = 'No file uploaded'

# Send an HTTP response with a message indicating whether the upload was successful
print('Content-Type: text/html')
print()
print('<html>')
print( "<head>")
print("<title>CGI UPLOAD FORM</title>")
print("</head>")
print('<body>')
print(f'<p>{message}</p>')
print('</body>')
print('</html>')
