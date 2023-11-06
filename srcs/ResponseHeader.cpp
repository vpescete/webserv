#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(Server *_server, RequestHeader *_request, Configuration *_config)
	: _server(_server), _request(_request), _config(_config)
{
	setStatusCodeMap();
	setDefaultHeaders();
}

void ResponseHeader::setStatusCodeMap()
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

void ResponseHeader::setDefaultHeaders()
{
	setStatusCode("0");
	setContentType("text/html");
	setContentLenght("0");
	setDate(getDate());
	_headers["Server"] = _server->getHost(); //è il nome del Server?
	setConnection("keep-alive");
}

std::string ResponseHeader::makeResponse(int code) const
{
	std::string response("HTTP/1.1 ");
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it->first != _headers.at("Date"); ++it)
	{
		response.append(it->first);
		response.append(": ");
		response.append(it->second);
		response.append("\r\n");
	}

	return response;
}

std::string ResponseHeader::getResponseCode(int code) const
{
	if (_statusCodeMap.find(code) != _statusCodeMap.end())
		return _statusCodeMap.find(code)->second;
	return _statusCodeMap.find(500)->second;
}

void ResponseHeader::setStatusCode(const std::string& code)
{
	_headers["Status"] = code + " " + getResponseCode(std::atoi(code.c_str()));
}

void ResponseHeader::setCookies(const std::string& name, const std::string& value)
{
	std::string cookie = name + "=" + value;
	_headers["Set-Cookie"] = cookie;
}

void ResponseHeader::setContent(const std::string& content)
{
	_headers["Content"] = content;
	_headers["Content-Length"] = std::to_string(content.length());
}

void ResponseHeader::setContentLenght(const std::string& content)
{
	_headers["Content-Length"] = content;
}

void ResponseHeader::setContentType(const std::string& content_type)
{
	_headers["Content-Type"] = content_type;
}

void ResponseHeader::setDate(const std::string& date)
{
	_headers["Date"] = date;
}

void ResponseHeader::setServer(const std::string& date)
{
	_headers["Server"] = _server->getHost();
}

//response.set_connection("keep-alive");  Enable persistent connections
//response.set_connection("close");  Disable persistent connections
void ResponseHeader::setConnection(const std::string& connection)
{
	_headers["Connection"] = connection;
}


