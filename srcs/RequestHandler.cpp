#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

std::string RequestHandler::getTrueBody() {
	return _headerMap["Body"];
}

std::string RequestHandler::getMethod() {
	return (_method);
}

void	RequestHandler::parsereq(std::string buffer, size_t size) {
	unsigned long i = 0;
	std::string temp;
	while (buffer[i] != '\n' && i < buffer.length()) {
		while (buffer[i] != ' ' && i < buffer.length()) {
			temp += buffer[i];
			i++;
		}
		_method = temp;
		temp.clear();
		i++;
		while (buffer[i] != ' ' && i < buffer.length()) {
			temp += buffer[i];
			i++;
		}
		_path = temp;
		temp.clear();
		i++;
		while (buffer[i] != '\n' && i < buffer.length()) {
			temp += buffer[i];
			i++;
		}
		_protocol = temp;
		temp.clear();
	}

	std::istringstream resp(buffer);
	std::string header;
	std::string::size_type index;
	size_t n = 2;
	while (std::getline(resp, header) && header != "\r") {
		size_t pos = 0;
		while ((pos = header.find(13)) != std::string::npos)
			header.erase(pos);
		index = header.find(':', 0);
		if(index != std::string::npos)
		{
			std::pair<std::string, std::string> tmp = std::make_pair(header.substr(0, index), header.substr(index + 2));
			if (_headerMap.find("Cookie") != _headerMap.end()
				&& tmp.first == "Cookie")
				_headerMap["Cookie"].append("; " + tmp.second);
			else
				_headerMap.insert(tmp);
		}
		else
			_headerMap.insert(std::make_pair("Method", header));
		n += header.size() + 2;
	}
	if (size > n)
		_headerMap.insert(std::make_pair("Body", buffer.substr(n, size - n)));
}

std::string RequestHandler::getPath() {
	return (_path);
}

std::string RequestHandler::getProtocol() {
	return (_protocol);
}

std::map<std::string, std::string> RequestHandler::getHeadersMap() {
	return (_headerMap);
}


void	RequestHandler::setResponse(Server* svr, int clientSocket, std::string pwd) {
	ResponseHandler res = ResponseHandler(svr, this, clientSocket, pwd);
}

bool	RequestHandler::autoIndex(int clientSocket) {
	//autoindex working also recursely
	std::string autoInd = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:"; //16\n\n";
	std::string dir_path = getPath();
	if (dir_path.back() != '/')
		dir_path += "/";
	std::string beforeBody = "\n\n<html>\n<body>\n<h1>Autoindex</h1>\n<ul>\n";
	std::string bodyText;
	std::string afterBody;
	std::string lengthContent;
	std::string::size_type i = 0, j = 0;
	int contentLength = beforeBody.length() - 2;
	DIR* dir;
	struct dirent *entry;
	if ((dir = opendir(("." + dir_path).c_str())) == NULL)
		return false;
	int flag = 0;
	while((entry = readdir(dir)) != NULL) {
		if (flag != 0)
			bodyText = bodyText + "<a href=" + dir_path + entry->d_name + ">" + "<br>" + entry->d_name + "</a>";
		else {
			bodyText = bodyText + "<a href=" + dir_path + entry->d_name + ">" + "<br>" + entry->d_name + "</a>";
			flag = 1;
		}
	}
	std::stringstream ss;
	afterBody = "</ul>\n</body>\n</html>\n";
	contentLength += bodyText.length();
	contentLength += afterBody.length() -1;
	ss << contentLength;
	ss >> lengthContent;
	autoInd = autoInd + lengthContent + beforeBody + bodyText + afterBody;
	closedir(dir);
	while(i < autoInd.length()) {
		i += send(clientSocket, autoInd.c_str(), autoInd.length(), 0);
		j = i;
		if (j == i + 1)
			break;
	}
	return true;
}

std::string RequestHandler::extractPath(const std::string& requestLine) {
	std::string path = requestLine;

	size_t pos = requestLine.find('?');
	if (pos != std::string::npos) {
		path = path.substr(0, pos);  // Rimuovi la stringa di query dal percorso
	}

	return path;
}
