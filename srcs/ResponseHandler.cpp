#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(Server *_server, RequestHandler *_request, int _cs)
	: _server(_server), _request(_request), _clientSocket(_cs)
{
	setStatusCodeMap();
	setDefaultHeaders();
			std::cout << "ciao" << std::endl;
	setPath(_request->getPath(), _request->getMethod());
				std::cout << "gesù figlio di troia" << std::endl;

	setContentType(_path);
	setContent();
}

ResponseHandler::~ResponseHandler()
{
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

void ResponseHandler::handleCGI(const std::string& scriptPath) {
	int pipefd[2];
	pid_t pid;
	char buf;
	std::string output;

	// Create a pipe
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	// Fork a new process
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {    // This is the child process
		close(pipefd[0]);  // Close read end of the pipe

		// Redirect stdout to the write end of the pipe
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);  // Close write end of the pipe

		// Execute the CGI script
		execl(scriptPath.c_str(), scriptPath.c_str(), (char*)NULL);
		exit(EXIT_FAILURE);
	} else {  // This is the parent process
		close(pipefd[1]);  // Close write end of the pipe

		// Read the output of the CGI script from the pipe and add it to the response content
		while (read(pipefd[0], &buf, 1) > 0) {
			_content += buf;
		}
		close(pipefd[0]);  // Close read end of the pipe

		// Wait for the child process to finish
		wait(NULL);
	}
}

#pragma region GET
std::string ResponseHandler::getPath() const
{
	return _path;
}

std::string ResponseHandler::getContent() const
{
	return _headers.at("Content");
}

std::string ResponseHandler::getContentType() const
{
	return _headers.at("Content-Type");
}

std::string ResponseHandler::getContentLenght() const
{
	return _headers.at("Content-Length");
}

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
	std::map<std::string, LocationPath> locationPaths = _server->getLocationPathMap();
	for (std::map<std::string, LocationPath>::iterator it = locationPaths.begin(); it != locationPaths.end(); ++it)
	{
		if (path.find(it->first, 0) == 0)
			return it->second;
	}
	return LocationPath();
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

std::string ResponseHandler::getStatusCode() const
{
	return _headers.at("Status");
}

std::string ResponseHandler::getErrorPath() {
	return ("/errors/" + getStatusCode() + ".html");
}

#pragma endregion region for get method

#pragma region SET
void ResponseHandler::setPath(const std::string& requestPath, const std::string& requestMethod)
{
	std::string method = _request->extractPath(requestPath);
	LocationPath path = getLocationPath(requestPath);
	// char cwd[9999];

	// Controlla se è stato trovato un path: se non lo trova, imposta status di errore.
	if (path.getLocationPath().empty()) {
		setStatusCode("404");
		_path = getErrorPath();
		return;
	}
	// Controlla se il percorso corrisponde a una rotta: se non lo trova, imposta status di errore.
	if (path.getMethods().find(requestMethod) == std::string::npos)
	{
		setStatusCode("404");
		_path = getErrorPath();
		return;
	}

	// Modifica il percorso in base alla rotta corrispondente
	std::string responsePath = getModifyPath(requestPath, path);


	// Controlla se il percorso modificato corrisponde a una nuova rotta
	LocationPath newRoute = getLocationPath(responsePath);
	if (newRoute.getMethods().find(requestMethod) == std::string::npos)
	{
		setStatusCode("404");
		_path = getErrorPath();
		return;
	}

	_path = responsePath;
	// _fullPath = getcwd(cwd, sizeof(cwd)) + _path;
}

void ResponseHandler::sendResponse()
{
	std::string status = getStatusCode();
	std::stringstream ss;
	int statuscode;

	ss << status;
	ss >> statuscode;
	if (status != "0" && status != "200")
		_path = getErrorPath();
	std::ifstream file(_path);
	if (_path == "/") {
		std::ifstream file(_server->getIndex());
		// if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		std::string response = "HTTP/1.1 " + getStatusCode() + " " + _statusCodeMap.at(statuscode) + "\nContent-type:" + getContentType() + "\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
		send(_clientSocket, response.c_str(), response.length(), 0);
		// }
	}
	else {
		char cwd[999];
		std::ifstream file(getcwd(cwd, sizeof(cwd)) + _path);
		// if (file.is_open()) {
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string content = buffer.str();
			std::string response = "HTTP/1.1 " + getStatusCode() + " " + _statusCodeMap.at(statuscode) + "\nContent-type:" + getContentType() + "\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
			send(_clientSocket, response.c_str(), response.length(), 0);
		// }
	}
}

void ResponseHandler::setContent()
{
	std::ifstream file;

	if (_path == "/")
		file.open(_server->getIndex());
	else
		file.open(_fullPath);
	struct stat s;
	std::string _content;
	if (_path != "/" && stat(_fullPath.c_str(), &s) == -1) // file doesn't exist
	{
		file.close();
		setStatusCode("404");
	}
	if ((file.is_open() && !(s.st_mode & S_IFDIR)) || _path == "/") // check if file is open or is a directory
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
		// else if (type == "py") // file has to go through cgi
		// {
		// 	handleCGI(_path);
		// }
		// else // file is probably an html
		// {
		// 	int fileLen;
		// 	std::string tmp = std::string("\r\n");

		// 	file.seekg(0, std::ios::end); // move the file iter to the end of the file
		// 	fileLen = file.tellg();
		// 	if (fileLen != -1)
		// 		_content.reserve(file.tellg()); // "allocate" enough memory in _content (tellg indicates current position in the file)
		// 	else
		// 	{
		// 		file.close();
		// 		setStatusCode("400");
		// 	}
		// 	file.seekg(0, std::ios::beg); // go back to the start of the file
		// 	std::stringstream buffer;
		// 	buffer << file.rdbuf(); // read the entire file through stringstream
		// 	_content = buffer.str(); // convert what has been read to std::string and assign it to _content
		// 	_content = tmp.append(_content, 0, _content.size());
		// 	std::stringstream ss;
		// 	ss << (_content.size() - 2);
		// 	setContentLenght(ss.str());
		// }
	}
	else
	{
		file.close();
		setStatusCode("404");
		// return ;
	}
	file.close();
	sendResponse();
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

void ResponseHandler::setEnv() {
	std::map<std::string, std::string>	headers = _headers;
	std::map<std::string, std::string>	env;
	char cwd[9999];
	getcwd(cwd, sizeof(cwd));

	env["REDIRECT_STATUS"] = "200";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SCRIPT_NAME"] = _path;
	env["SCRIPT_FILENAME"] = _path;
	env["REQUEST_METHOD"] = _request->getMethod();
	env["CONTENT_LENGTH"] = headers["Content-Length"];
	env["CONTENT_TYPE"] = headers["Content-Type"];
	env["PATH_INFO"] = _path;
	env["PATH_TRANSLATED"] =_path;
	env["QUERY_STRING"] = _path;
	env["REMOTEaddr"] = _server->getHost();
	env["UPLOAD_PATH"] = "/upload";
	if (headers.find("Hostname") != headers.end())
		env["SERVER_NAME"] = headers["Hostname"];
	else
		env["SERVER_NAME"] = env["REMOTEaddr"];
	env["SERVER_PORT"] = std::to_string(_server->getPort());
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
	env["HTTP_COOKIE"] = headers["Cookie"];

	_env = new char*[env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		std::string tmp = it->first + "=" + it->second;
		_env[i] = new char[tmp.size() + 1];
		std::strcpy(_env[i], tmp.c_str());
		++i;
	}
}

void ResponseHandler::setStatusCodeMap()
{
	_statusCodeMap[0] = "OK";
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
	setStatusCode("200");
	_headers["Path"] = "";
	_headers["Content"] = "";
	_headers["Content-Type"] = "text/html";
	setContentLenght("0");
	setConnection("keep-alive");
}

void ResponseHandler::setStatusCode(const std::string& code)
{
	_headers["Status"] = code;
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
