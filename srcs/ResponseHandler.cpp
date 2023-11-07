#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(Server *_server, RequestHandler *_request, Configuration *_config)
	: _server(_server), _request(_request), _config(_config)
{
	setStatusCodeMap();
	setDefaultHeaders();
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
		if (path.find(it->first) != std::string::npos)
			return it->second;
	}
}
#pragma endregion region for get method

#pragma region Creation
void ResponseHandler::setPath()
{
	std::string path = _request->getPath();
	LocationPath locationPath = getLocationPath(path);
}

void ResponseHandler::setContent()
{
	//std::string = _request->getContent();
}

void ResponseHandler::setContentType(std::string path, std::string type)
{
	(void)path;
	(void)type;
}
#pragma endregion region for creation

#pragma region SET
void ResponseHandler::setStatusCodeMap()
{
	_statusCodeMap[200] = "OK";
	_statusCodeMap[201] = "Created";
	_statusCodeMap[204] = "No Content";
	_statusCodeMap[400] = "Bad Request";
	_statusCodeMap[401] = "Unauthorized";
	_statusCodeMap[403] = "Forbidden";
	_statusCodeMap[404] = "Not Found";
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

void ResponseHandler::setServer()
{
	_headers["Server"] = _server->getHost();
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


void ResponseHandler::setDate(const std::string& date)
{
	_headers["Date"] = date;
}

//response.set_connection("keep-alive");  Enable persistent connections
//response.set_connection("close");  Disable persistent connections
void ResponseHandler::setConnection(const std::string& connection)
{
	_headers["Connection"] = connection;
}
#pragma endregion region for set method


