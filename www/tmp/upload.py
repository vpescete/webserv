import cgi, os

form = cgi.FieldStorage()

fileitem = form.getValue('file1')

path = "/uploads/"

isExist = os.path.exists(path)

if not isExist:
	os.makedirs(path)

if fileitem.filename:
	fn = os.path.basename(fileitem.filename)
	open(path + fn, 'wb').write(fileitem.file.read())
	message = "The file '" + fn + "' was uploaded successfully"
else:
	message = "No file was uploaded"

print("""\
<html><body>
<p>%s</p>
</body></html>
""" % (message,))
