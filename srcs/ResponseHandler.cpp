#include "ResponseHandler.hpp"
#include <dirent.h>
#include <unistd.h>

ResponseHandler::ResponseHandler(Server *_server, RequestHandler *_request, int _cs, std::string pwd)
	: _server(_server), _request(_request), _clientSocket(_cs)
{
	setStatusCodeMap();
	setDefaultHeaders();
	setPath(_request->getPath(), _request->getMethod());
	_postQuestionMark = "";
	setContent(pwd);
}

ResponseHandler::~ResponseHandler() {
}


std::string ResponseHandler::makeResponse(int code) {
	std::string response("HTTP/1.1 ");

	if (_content.size() - 2 == 0)
	{
		response.append(std::to_string(204));
		response.append(" ");
		response.append(getResponseCode(204));
	}
	else
	{
		response.append(getStatusCode());
		response.append(" ");
		response.append(getResponseCode(code));
	}
	response.append("\r\n");
	response.append("content-type: ");
	response.append(getContentType());
	response.append("\r\n");
	response.append("content-length: ");
	response.append(getContentLenght());
	response.append("\r\n");
	response.append(getDate());
	response.append("\r\n\r\n");
	response.append(_content);
	return response;
}

std::string ResponseHandler::handleCGI(const std::string& scriptPath, std::string envpath) {
	std::string newBody;
	pid_t pid;
	std::string absolutPath = envpath + _path;

	int saveStdin = dup(STDIN_FILENO);
	int saveStdout = dup(STDOUT_FILENO);

	FILE* fileIn = tmpfile();
	FILE* fileOut = tmpfile();
	int fdIn = fileno(fileIn);
	int fdOut = fileno(fileOut);
	int		ret = 1;
	// Scrivi il corpo della richiesta prima di fork
	write(fdIn, _request->getTrueBody().c_str(), _request->getTrueBody().size());
	lseek(fdIn, 0, SEEK_SET);
	// Fork di un nuovo processo
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0) {    // Questo è il processo figlio
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		const char* pyArgs[] = {scriptPath.c_str(), _path.substr(1).c_str(), NULL};
		execve(*pyArgs, const_cast<char **> (pyArgs), _env);
		//std::cout << RED << "Error: execve fail" << RESET << std::endl;
		perror("Error");
		exit(EXIT_FAILURE);
	}
	else
	{
		char	buffer[65536] = {0};
		while (waitpid(-1, NULL, 2) != -1) ;
		lseek(fdOut, 0, SEEK_SET);
		while (ret > 0)
		{
			memset(buffer, 0, 65536);
			ret = read(fdOut, buffer, 65536 - 1);
			newBody += buffer;
		}
	}
	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fileIn);
	fclose(fileOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);
	for (size_t i = 0; _env[i]; i++)
		delete[] _env[i];
	delete[] _env;
	if (!pid)
		exit(0);
	return newBody;
}

#pragma region GET
std::string ResponseHandler::getPath() const
{
	return _path;
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
	std::string date = "date: ";
	time_t t = std::time(nullptr);
	tm* lt = std::localtime(&t);
	char buffer[50];
	std::strftime(buffer, 80, "%a, %d %b %Y %X", lt);
	date.append(std::string(buffer));
	return date;
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

std::string ResponseHandler::getModifyPath(const std::string& requestPath, LocationPath& path) {
	std::string modifiedPath = requestPath;

	// Se la route ha una root specificata, sostituisci il percorso della rotta con la root nel percorso della richiesta
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
	if (_path == "/") {
		std::ifstream file(_server->getIndex());
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		std::string response = makeResponse(statuscode);
		send(_clientSocket, response.c_str(), response.length(), 0);
	}
	else {
		std::ifstream file("." + _path);
		std::string temp;
		int dataSent = 0;
		if (getContentType() == "image/png" || getContentType() == "image/jpeg")
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			_content = buffer.str();
		}
		std::string response = makeResponse(statuscode);
		do {
			temp = response.substr(0, 35000);
			dataSent = send(_clientSocket, temp.c_str(), temp.length(), 0);
			if (dataSent < 0)
				break ;
			response = response.substr(dataSent);
		} while (response.size());
	}
}


void deleteFilesInFolder(const std::string& folderPath) {
	DIR* dir = opendir(folderPath.c_str());
	if (dir != NULL) {
		dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG) {  // Solo file regolari
				std::string filePath = folderPath + "/" + entry->d_name;
				if (unlink(filePath.c_str()) != 0) {
					std::cerr << "Errore durante l'eliminazione del file: " << filePath << std::endl;
				}
			}
		}
		closedir(dir);
	} else {
		std::cerr << "Errore durante l'apertura della cartella: " << folderPath << std::endl;
	}
}

void ResponseHandler::setContent(std::string pwd)
{
	std::ifstream file;
	std::string preQuestion;
	size_t questPosition = _path.rfind('?');
	std::string fullPath;


	if (_path == "/") {
		file.open(_server->getIndex());
		_path = _server->getIndex();
		fullPath = _path;
	}
	else if (questPosition == std::string::npos){
		file.open("." + _path);
		fullPath = "." + _path;

	}
	else {
		_postQuestionMark = _path.substr(questPosition + 1, _path.length() - questPosition);
		preQuestion = _path.substr(0, questPosition);
		file.open("." + preQuestion);
		_path = preQuestion;
		fullPath = "." + _path;
	}
	struct stat s;
	if (fullPath != "/" && stat(fullPath.c_str(), &s) == -1) // file doesn't exist
	{
		file.close();
		setStatusCode("404");
	}
	if ((file.is_open() || !(s.st_mode & S_IFDIR))) // check if file is open or is a directory
	{
		size_t dotPos = fullPath.rfind('.'); // indicates where the dot in the path is located
		std::string type; // extension of the response file
		if (dotPos != std::string::npos) // npos is returned by rfind if there weren't any matches
			type = fullPath.substr(dotPos, fullPath.size() - dotPos); // take the path from the dot onwards
		else type = "";
		if (_request && _request->getMethod() == "DELETE") // method DELETE
		{
			std::string pathToDelete = pwd + _path;
			deleteFilesInFolder(pathToDelete);
			std::stringstream ss;
			ss << (_content.size() - 2);
			setContentLenght(ss.str());
		}
		else if (type == ".py") // file has to go through cgi for the POST Upload
		{
			std::string location = _server->getMap().find("location")->second;
			std::string cgi = location.substr(location.find("cgi_pass"),location.substr(location.find("cgi_pass"), location.find("}")).find("\n"));
			std::string final_cgi =  cgi.substr(cgi.find("/"), cgi.length());
			setEnv(pwd);
			_content = handleCGI(final_cgi, pwd);
			std::stringstream ss;
			ss << (_content.size() - 2);
			setContentLenght(ss.str());
		}
		else // GET METHOD
		{
			std::string tmp = std::string("\r\n");
			file.seekg(0, std::ios::end);
			_content.reserve(file.tellg());
			file.seekg(0, std::ios::beg);
			_content.assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
			_content = tmp.append(_content, 0, _content.size());
			std::stringstream ss;
			ss << (_content.size() - 2);
			setContentLenght(ss.str());
		}
	}
	else
	{
		std::cout << "NON CI VADO" << std::endl;
		file.close();
		setStatusCode("404");
	}
	file.close();
	setContentType(_path);
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
	else{
		type = "";
		_contentType = type;
	}
	// pretty self-explicatory from this point onwards
	if ((type == "html") || _request->getMethod() == "DELETE" || (type == "py") || (type == "ico"))
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
	_headers["Content-Type"] = _contentType;
}

std::string	trimUselessChar(std::string contentType) {
	// Rimuovi spazi dall'inizio della stringa
	size_t startPos = contentType.find_first_not_of(" \r\n");
	if (startPos != std::string::npos) {
		contentType.erase(0, startPos);
	}

	// Rimuovi spazi dalla fine della stringa
	size_t endPos = contentType.find_last_not_of(" \r\n");
	if (endPos != std::string::npos) {
		contentType.erase(endPos + 1);
	}
	return contentType;
}

void ResponseHandler::setEnv(std::string envpwd) {
	std::map<std::string, std::string>	headers = _request->getHeadersMap();
	std::map<std::string, std::string>	env;
	std::string contentType = trimUselessChar(headers["Content-Type"]);
	std::string contentLength = trimUselessChar(headers["Content-Length"]);
	std::string path = _path.substr(1);
	int i=0;

	env["CONTENT_LENGTH"] = contentLength;
	env["CONTENT_TYPE"] = contentType;
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["HTTP_COOKIE"] = "";
	env["PATH_INFO"] = path;
	env["PATH_TRANSLATED"] = path ;
	env["QUERY_STRING"] = "";
	env["REDIRECT_STATUS"] = "200";
	env["REMOTE_IDENT"] = "";
	env["REMOTE_USER"] = "";
	env["REMOTEaddr"] = _server->getHost();
	env["REQUEST_METHOD"] = _request->getMethod();
	env["REQUEST_URI"] =  path;
	env["SCRIPT_FILENAME"] = path;
	env["SCRIPT_NAME"] = path;
	env["SERVER_NAME"] = _server->getHost();
	env["SERVER_PORT"] = std::to_string(_server->getPort());
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
	env["UPLOAD_PATH"] =  envpwd + "/uploads/";

	std::string tmp;
	_env = (char **)malloc(sizeof(char*)*env.size() + 1);
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++)
	{
		tmp = it->first + "=" + it->second;
		_env[i] = new char[tmp.size() + 1];
		std::strcpy(_env[i], tmp.c_str());
		tmp.clear();
		++i;
	}
	tmp.clear();
	_env[i] = NULL;
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
	_headers["Content-Type"] = "text/html";
	setContentLenght("0");
	setConnection("keep-alive");
}

void ResponseHandler::setStatusCode(const std::string& code)
{
	_headers["Status"] = code;
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
