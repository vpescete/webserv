#include "../include/RequestHandler.hpp"

std::string trim(const std::string& str)
{
	std::string::const_iterator it = str.begin();
	while (it != str.end() && std::isspace(*it))
	{
		it++;
	}

	std::string::const_reverse_iterator rit = str.rbegin();
	while (rit.base() != it && std::isspace(*rit))
	{
		rit++;
	}

	return std::string(it, rit.base());
}

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

std::string RequestHandler::getTrueBody() {
	return _body;
}

bool RequestHandler::getBool() {
	return _flag;
}

std::string RequestHandler::getMethod() {
	return (_method);
}

std::string RequestHandler::get_deleteMethod(std::string::size_type start, std::string::size_type end, std::string temp2, std::string key, std::string value) {
	while (end < temp2.size())
		{
			start = temp2.find(':', 0);
			end = temp2.find('\n', 0);
			if(end != std::string::npos) {
				key = temp2.substr(0, start);
				value = temp2.substr(start + 1, end - start - 2);
				// removeWhitespace(key);
				// removeWhitespace(value);
				_mapHeader.insert(std::pair<std::string, std::string>(key, value));
				if (key == "Content-Type")
					_flag = true;
			}
			temp2 = temp2.substr(end + 1);
		}
		return temp2;
}


void RequestHandler::postMethod(std::string::size_type start, std::string::size_type end, std::string temp2, std::string key, std::string value, const char* headerEnd){
	int k = 0;
	while ((headerEnd[k] == '\n' || headerEnd[k] == '\r') && headerEnd[k] != '\0')
		headerEnd++;
	if (headerEnd != NULL) {
		// Parse and print the headers
		// std::cout << headerEnd << std::endl;
		while (end < temp2.size())
		{
			start = temp2.find(':', 0);
			end = temp2.find('\n', 0);
			if(end != std::string::npos) {
				key = temp2.substr(0, start);
				value = temp2.substr(start + 1, end - start - 1);
				// removeWhitespace(key);
				// removeWhitespace(value);
				_mapHeader.insert(std::pair<std::string, std::string>(key, value));
				if (key == "Content-Type")
					_flag = true;
				// std::cout << RED << key << YELLOW << " : " << GREEN << value << RESET << std::endl;
			}
			temp2 = temp2.substr(end + 1);
		}
		if (headerEnd[0] == '-') {
			uploadNoImage(start, end, key, value, headerEnd);
		}
		else {
			std::cout << CYAN << headerEnd << RESET << std::endl;
			_body = headerEnd;
			_mapHeader.insert(std::pair<std::string, std::string>("Body", _body));
		}
		// std::cout << GREEN << _bodyEnd << RESET << std::endl;
	}
}

void	RequestHandler::uploadNoImage(std::string::size_type start, std::string::size_type end, std::string key, std::string value, const char* headerEnd) {
	// int j = 0;
	// int k = 0;
	std::cout << "PROVA" << std::endl;
	std::string bodyStart = headerEnd + 2;
	// bool flag = false;
	(void)start;
	(void)end;
	(void)key;
	(void)value;
	if (bodyStart.length() > 0) {
		_mapHeader.insert(std::pair<std::string, std::string>("Body", bodyStart));
		std::cout << bodyStart << std::endl;
		_body = bodyStart;
	}
	//std::cout << CYAN << _mapHeader.at("Body") << RESET << std::endl;
}
	// 	while(bodyStart[j] != '\n' && bodyStart[j] != '\0')
	// 		j++;
	// 	_bodyStart = bodyStart.substr(0, j);
	// 	end = 0;
	// 	bodyStart = bodyStart.substr(j + 1, bodyStart.length() - j);
	// 	while (end < bodyStart.size())
	// 	{
	// 		std::cout << "HEY" << std::endl;
	// 		start = bodyStart.find(':', 0);
	// 		end = bodyStart.find('\n', 0);
	// 		if (bodyStart[0] == '\r' && bodyStart[1] == '\n' && bodyStart[2] == '\r')
	// 			break;
	// 		std::cout << BLUE << start << "----" << end << RESET << std::endl;
	// 		if(end != std::string::npos && start < end) {
	// 			std::cout << "AHAHAH" << std::endl;
	// 			key = bodyStart.substr(0, start);
	// 			value = bodyStart.substr(start + 2, end - start - 2);
	// 			// removeWhitespace(key);
	// 			// removeWhitespace(value);
	// 			_mapBody.insert(std::pair<std::string, std::string>(key, value));
	// 			std::cout << RED << key << YELLOW << " : " << GREEN << value << RESET << std::endl;
	// 		}
	// 		else {
	// 			std::cout << "ciao" << std::endl;
	// 			_body = bodyStart.substr(2, bodyStart.length());
	// 			bodyStart = bodyStart.substr(0, _body.length());
	// 			flag = true;
	// 			break ;
	// 		}
	// 		bodyStart = bodyStart.substr(end + 1, bodyStart.length() - end);
	// 	}
	// }
	// j = 0;
	// if (flag == false) {
	// 	while (bodyStart[j] == '\n' || bodyStart[j] == '\r')
	// 		j++;
	// 	bodyStart = bodyStart.substr(j, bodyStart.length() - j - 2);
	// 	_bodyEnd = bodyStart;
	// }
	// else {
	// 	_bodyEnd = _body.substr(_body.find("------WebKitFormBoundary"), _body.length() - _body.find("------WebKitFormBoundary"));
	// 	std::cout << YELLOW << _body << RESET << std::endl;
	// }
	// std::cout << BLUE << _bodyEnd << RESET << std::endl;
// }

void	RequestHandler::parsereq(std::string buffer) {
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
	// second parser of the request to put all handler in a map
	std::string::size_type start = 0;
	std::string::size_type end = 0;
	std::string key;
	std::string value;
	std::string temp2;
	// std::cout << GREEN << "[DEBUG METHOD]" << buffer.substr(0, i) << RESET << std::endl;
	// std::cout << RED << "[DEBUG METHOD]" << buffer.substr(0, i) << RESET << std::endl;
	if (i + 1 < buffer.length())
		temp2 = buffer.substr(i + 1, buffer.length());
	else {
		temp2 = "";
		return ;
	}
	// std::cout << YELLOW << buffer << RESET << std::endl;
	if (_method == "GET" || _method == "DELETE") {
		get_deleteMethod(start,end, temp2, key, value);
	}
	else if (_method == "POST")
	{
		const char* headerEnd = strstr(buffer.c_str(), "\r\n\r\n");
		temp2 = buffer.substr(i + 1, buffer.length() - strlen(headerEnd));
		postMethod(start, end, temp2, key, value, headerEnd);
	}
	// temp = NULL;
	// free(temp);
}

std::string RequestHandler::getPath() {
	return (_path);
}

std::string RequestHandler::getProtocol() {
	return (_protocol);
}

std::map<std::string, std::string> RequestHandler::getHeaders() {
	return (_mapHeader);
}


void	RequestHandler::setResponse(Server* svr, int clientSocket, std::string pwd) {
	ResponseHandler res = ResponseHandler(svr, this, clientSocket, pwd);
	// if (_path == "/") {
	// 	std::ifstream file(svr->getIndex());
	// 	if (file.is_open()) {
	// 		std::stringstream buffer;
	// 		buffer << file.rdbuf();
	// 		std::string content = buffer.str();
	// 		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
	// 		send(clientSocket, response.c_str(), response.length(), 0);

	// 	}
	// 	else {
	// 		std::ifstream file("./errors/404.html");
	// 		std::stringstream buffer;
	// 		buffer << file.rdbuf();
	// 		std::string content = buffer.str();
	// 		// std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
	// 		// send(clientSocket, response.c_str(), response.length(), 0);
	// 		std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length:  " + std::to_string(content.length()) + "\r\n\r\n" + content;
	// 		send(clientSocket, response.c_str(), response.length(), 0);
	// 	}
	// 	// Read and send the file requested from the path in the request
	// }
	// else {
	// 	std::ifstream file("." + _path);
	// 	if (file.is_open()) {
	// 		std::stringstream buffer;
	// 		buffer << file.rdbuf();
	// 		std::string content = buffer.str();
	// 		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
	// 		send(clientSocket, response.c_str(), response.length(), 0);
	// 	} else {
	// 		std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
	// 		send(clientSocket, response.c_str(), response.length(), 0);
	// 	}
	// }
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
	// std::cout << BLUE << autoInd << RED << lengthContent << GREEN << beforeBody << CYAN << bodyText << YELLOW << afterBody << RESET << std::endl;
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

	// Rimuovi la stringa di query dal percorso, se presente
	size_t pos = requestLine.find('?');
	if (pos != std::string::npos) {
		path = path.substr(0, pos);  // Rimuovi la stringa di query dal percorso
	}

	return path;
}
