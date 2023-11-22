import cgi, os

form = cgi.FieldStorage()

fileitem = form['file1']

path = "./uploads/"

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
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      font-family: 'Arial', sans-serif;
      background-color: #B7ADCF; /* Verde chiaro */
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100vh;
      margin: 0;
      text-align: center; /* Centra l'intero contenuto del corpo */
    }

    #content-container {
      background-color: #fff; /* Colore sfondo del container */
      padding: 20px; /* Spaziatura interna del container */
      border-radius: 8px; /* Bordo arrotondato */
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Ombra */
    }

    p {
      font-size: 1.5em;
      color: #333;
    }

    button {
      background-color: #27ae60; /* Verde scuro */
      color: #fff;
      padding: 10px 20px;
      font-size: 1em;
      border: none;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: #219653; /* Tonalità più scura del verde */
    }
  </style>
</head>
<body>
  <div id="content-container">
    <p>%s</p>
    <button onclick="window.location.href='upload.html'">Torna alla pagina di upload per caricare un altro fantastico file!!</button>
  </div>
</body>
</html>
""" % (message,))
