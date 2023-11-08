#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(Server *_server, RequestHandler *_request, Configuration *_config)
	: _server(_server), _request(_request), _config(_config)
{
	setStatusCodeMap();
	setDefaultHeaders();
	setPath(_request->getPath(), _request->getMethod());
}

ResponseHandler::~ResponseHandler()
{
}

std::string ResponseHandler::makeResponse() const
{
	std::string response("HTTP/1.1 ");
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it->first != _headers.at("Date"); ++it)
	{
		response.append(it->first);
		response.append(": ");
		response.append(it->second);
		response.append("\r\n");
	}
	response.append(getDate());
	return response;
}

bool ResponseHandler::isDirectory(const std::string& path) {
	struct stat info;

	if (stat(path.c_str(), &info) != 0) {
		// Non è possibile accedere alle informazioni del file/directory, quindi ritorna false
		return false;
	} else if (info.st_mode & S_IFDIR) {
		// Il path corrisponde a una directory, quindi ritorna true
		return true;
	} else {
		// Il path non corrisponde a una directory, quindi ritorna false
		return false;
	}
}


#pragma region GET
std::string ResponseHandler::getResponseCode(int code) const
{
	if (_statusCodeMap.find(code) != _statusCodeMap.end())
		return _statusCodeMap.find(code)->second;
	return _statusCodeMap.find(500)->second;
}

std::string ResponseHandler::getDate() const
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
	std::string str(buffer);

	return str;
}

LocationPath ResponseHandler::getLocationPath(std::string path)
{
	std::map<std::string, LocationPath> locationPaths = _config->getLocationPath();
	for (std::map<std::string, LocationPath>::iterator it = locationPaths.begin(); it != locationPaths.end(); ++it)
	{
		if (path.find(it->first, 0) == 0)
			return it->second;
	}
	std::map<std::string, LocationPath> secondTry = _server->getConf().getLocationPath();
	if (secondTry.find("/") != secondTry.end())
	{
		return LocationPath();
	}
	return secondTry.find("/")->second;

}

std::string ResponseHandler::getCurrentPath() const {
	char buffer[FILENAME_MAX];
	if (getcwd(buffer, FILENAME_MAX) != NULL) {
		return std::string(buffer);
	} else {
		// Inserire pagina generica d'errore
		return "";
	}
}

std::string ResponseHandler::getModifyPath(const std::string& requestPath, LocationPath& path) {
	std::string modifiedPath = requestPath;

	// Se la rotta ha una root specificata, sostituisci il percorso della rotta con la root nel percorso della richiesta
	if (!path.getRoot().empty()) {
		size_t pos = modifiedPath.find(path.getLocationPath());
		if (pos != std::string::npos) {
			modifiedPath.replace(pos, path.getLocationPath().length(), path.getRoot());
		}
	}

	return modifiedPath;
}
#pragma endregion region for get method

#pragma region SET

void ResponseHandler::setPath(const std::string& requestPath, const std::string& requestMethod)
{
	std::string method = _request->extractPath(requestPath);
	LocationPath path = getLocationPath(requestPath);

	// Controlla se è stato trovato un path
	if (path.getLocationPath().empty()) {
		setStatusCode("404");
		_path = _config->getErrorPath(getResponseCode(404));
		return;
	}
	// Controlla se il percorso corrisponde a una rotta
	if (path.getMethods().find(requestMethod) == std::string::npos)
	{
		setStatusCode("405");
		_path = _config->getErrorPath(getResponseCode(405));
		return;
	}
	// Modifica il percorso in base alla rotta corrispondente
	std::string responsePath = getModifyPath(requestPath, path);

	// Controlla se il percorso modificato corrisponde a una nuova rotta
	LocationPath newRoute = getLocationPath(_path);
	if (newRoute.getMethods().find(requestMethod) == std::string::npos)
	{
		setStatusCode("405");
		_path = _config->getErrorPath(getResponseCode(405));
		return;
	}

	// Rimuove eventuali slash iniziali o finali dal percorso
	if (responsePath.front() == '/')
		responsePath.erase(responsePath.begin());
	if (responsePath.back() == '/')
		responsePath.erase(responsePath.end() - 1);

	// Se il percorso corrisponde esattamente al percorso della nuova rotta, aggiunge l'indice della nuova rotta al percorso
	if (newRoute.getMethods().find(requestMethod) == std::string::npos && responsePath == newRoute.getLocationPath()) {
		responsePath += newRoute.getIndex();
	}

	// Se il percorso corrisponde esattamente al percorso della rotta originale o della nuova rotta, controlla se il percorso corrisponde a una directory
	if ((responsePath == path.getMethods()
		|| (newRoute.getMethods().find(requestMethod) != std::string::npos && responsePath == newRoute.getMethods()))
		&& isDirectory(responsePath)) {
		// aggiungta di "/.index.html" al percorso
		responsePath = path.getMethods();
		responsePath += "/.index.html";
		_path = responsePath;
	}
}

void ResponseHandler::setContent()
{
	std::ifstream file;
	file.open(_path, std::ios::in);
	struct stat s;
	std::string _content;


	if (stat(_path.c_str(), &s) == -1) // file doesn't exist
	{
		file.close();
		setStatusCode("404");
		return ;
	}
	if (file.is_open() && !(s.st_mode & S_IFDIR)) // check if file is open or is a directory
	{
		size_t dotPos = _path.rfind('.'); // indicates where the dot in the path is located
		std::string type; // extension of the response file

		if (dotPos != std::string::npos) // npos is returned by rfind if there weren't any matches
			_path.substr(dotPos, _path.size() - dotPos); // take the path from the dot onwards
		else type = "";
		if (_request && _request->getMethod() == "DELETE")
		{
			if (remove(_path.c_str()) == 0) // try to delete the file inside of the filesys
				_content = "\r\n<h1>File deleted successfully</h1>";
			else
				_content = "\r\n<h1>Unable to delete the file</h1>";
			std::stringstream ss;
			ss << (_content.size() - 2);
			setContentLenght(ss.str());
		}
		else if (type == "py" || type == "php") // file has to go through cgi
		{
			; // no clue about how the cgi works
		}
		else // file is probably an html
		{
			int fileLen;
			std::string tmp = std::string("\r\n");

			file.seekg(0, std::ios::end); // move the file iter to the end of the file
			fileLen = file.tellg();
			if (fileLen != -1)
				_content.reserve(file.tellg()); // "allocate" enough memory in _content (tellg indicates current position in the file)
			else
			{
				file.close();
				setStatusCode("500");
				return ;
			}
			file.seekg(0, std::ios::beg); // go back to the start of the file
			std::stringstream buffer;
    		buffer << file.rdbuf(); // read the entire file through stringstream
			_content = buffer.str(); // convert what has been read to std::string and assign it to _content
			_content = tmp.append(_content, 0, _content.size());
			std::stringstream ss;
			ss << (_content.size() - 2);
			setContentLenght(ss.str());
		}
	}
	else
	{
		file.close();
		setStatusCode("404");
		return ;
	}
	file.close();
}

void ResponseHandler::setContentType(std::string path, std::string type)
{
	std::string _contentType;

	if (type != "")
	{
		_contentType = type;
		return ;
	}
	size_t dotPos = path.rfind(".");
	if (dotPos != std::string::npos)
		type = path.substr(dotPos + 1, path.size() - dotPos); // get the end of the path from "."
	else
		type = "";
	// pretty self-explicatory from this point onwards
	if ((type == "html") || _request->getMethod() == "DELETE") // maybe cgi???
		_contentType = "text/html";
	else if (type == "css")
		_contentType = "text/css";
	else if (type == "js")
		_contentType = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
	else if (type == "bmp")
		_contentType = "image/bmp";
	else
		_contentType = "text/plain";
}

void ResponseHandler::setStatusCodeMap()
{
	_statusCodeMap[200] = "OK";
	_statusCodeMap[201] = "Created";
	_statusCodeMap[204] = "No Content";
	_statusCodeMap[400] = "Bad Request";
	_statusCodeMap[401] = "Unauthorized";
	_statusCodeMap[403] = "Forbidden";
	_statusCodeMap[404] = "Not Found";
	_statusCodeMap[405] = "Method Not Allowed";
	_statusCodeMap[500] = "Internal Server Error";
	_statusCodeMap[501] = "Not Implemented";
	_statusCodeMap[502] = "Bad Gateway";
	_statusCodeMap[503] = "Service Unavailable";
}

void ResponseHandler::setDefaultHeaders()
{
	setStatusCode("0");
	_headers["Path"] = "";
	_headers["Content"] = "";
	_headers["Content-Type"] = "text/html";
	setContentLenght("0");
	setConnection("keep-alive");
}

void ResponseHandler::setStatusCode(const std::string& code)
{
	_headers["Status"] = code + " " + getResponseCode(std::atoi(code.c_str()));
}

void ResponseHandler::setCookies(const std::string& name, const std::string& value)
{
	std::string cookie = name + "=" + value;
	_headers["Set-Cookie"] = cookie;
}

void ResponseHandler::setContentLenght(const std::string& content)
{
	_headers["Content-Length"] = content;
}

//"keep-alive" Enable persistent connections / "close" Disable persistent connections
void ResponseHandler::setConnection(const std::string& connection)
{
	_headers["Connection"] = connection;
}
#pragma endregion region for set method

