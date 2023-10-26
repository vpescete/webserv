#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

void   	RequestHandler::parsereq(std::string buffer) {
	int	i = 0, j = 0;
	char*	temp;
	temp = (char*)malloc(10000);

	//first parser of the method, the path and the protocol to private attributes of the class
	while (buffer[i] != '\n') {
		while (buffer[i] != ' '){
			temp[j] = buffer[i];
			j++;
			i++;
		}
		temp[j] = '\0';
		_method = temp;
		temp = (char*)memset(temp, 0, j);
		j = 0;
		i++;
		while (buffer[i] != ' ') {
			temp[j] = buffer[i];
			j++;
			i++;
		}
		temp[j] = '\0';
		_path = temp;
		temp = (char*)memset(temp, 0, j);
		j = 0;
		i++;
		while (buffer[i] != '\n') {
			temp[j] = buffer[i];
			j++;
			i++;
		}
		temp[j] = '\0';
		_protocol = temp;
		temp = (char*)memset(temp, 0, j);
		j = 0;
	}
	// second parser of the request to put all handler in a map
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string key;
	std::string value;
	std::string temp2;
	temp2 = buffer.substr(i + 1, buffer.length());
	if (_method == "GET") {
		while (end < temp2.size())
		{
			start = temp2.find(':', 0);
			end = temp2.find('\n', 0);
			if(end != std::string::npos) {
				key = temp2.substr(0, start);
				value = temp2.substr(start + 1, end - start - 2);
				removeWhitespace(key);
				removeWhitespace(value);
				_mapHeader.insert(std::make_pair<std::string, std::string>(key, value));
				// std::cout << RED << key << YELLOW << " : " << GREEN << value << RESET << std::endl;
			}
			temp2 = temp2.substr(end + 1);
		}
	}
	else if (_method == "POST")
	{
		const char* headerEnd = strstr(buffer.c_str(), "\r\n\r\n");
		temp2 = buffer.substr(i + 1, buffer.length() - strlen(headerEnd));
		std::cout << BLUE << buffer << RESET << std::endl;
		int k = 0;
		while ((headerEnd[k] == '\n' || headerEnd[k] == '\r') && headerEnd[k] != '\0')
			headerEnd++;
		if (headerEnd != NULL) {
			// Parse and print the headers
			std::cout << headerEnd << std::endl;
			while (end < temp2.size())
			{
				start = temp2.find(':', 0);
				end = temp2.find('\n', 0);
				if(end != std::string::npos) {
					key = temp2.substr(0, start);
					value = temp2.substr(start + 1, end - start - 1);
					removeWhitespace(key);
					removeWhitespace(value);
					_mapHeader.insert(std::make_pair<std::string, std::string>(key, value));
					// std::cout << RED << key << YELLOW << " : " << GREEN << value << RESET << std::endl;
				}
				temp2 = temp2.substr(end + 1);
			}
			if (headerEnd[0] == '-') {
			// std::cout << YELLOW << "Headers:\n" << buffer << RESET << std::endl;
			// Extract the body (if the call is upload without images)
			std::string bodyStart = headerEnd + 4;
			if (bodyStart.length() > 0) {
					// std::cout << RED << "Body:\n" << bodyStart << RESET << std::endl;
				while(bodyStart[j] != '\n' && bodyStart[j] != '\0') 
					j++;
				_bodyStart = bodyStart.substr(0, j);
				end = 0;
				bodyStart = bodyStart.substr(j + 1, bodyStart.length() - j);
				while (end < bodyStart.size())
				{
					start = bodyStart.find(':', 0);
					end = bodyStart.find('\n', 0);
					if (bodyStart[0] == '\r')
						break;
					if(end != std::string::npos) {
						key = bodyStart.substr(0, start);
						value = bodyStart.substr(start + 2, end - start - 2);
						removeWhitespace(key);
						removeWhitespace(value);
						_mapBody.insert(std::make_pair<std::string, std::string>(key, value));
						// std::cout << RED << key << YELLOW << " : " << GREEN << value << RESET << std::endl;
					}
					bodyStart = bodyStart.substr(end + 1, bodyStart.length() - end);
				}
				j = 0;
				while (bodyStart[j] == '\n' || bodyStart[j] == '\r')
					j++;
				bodyStart = bodyStart.substr(j, bodyStart.length() - j - 2);
				_bodyEnd = bodyStart;
				}
			}
			// Extract the body (if the call is anything else)
			else
				_bodyEnd = headerEnd;
		}
	}
	temp = NULL;
	free(temp);
}

std::string RequestHandler::getMethod() {
	return (_method);
}

std::string RequestHandler::getPath() {
	return (_path);
}

std::string RequestHandler::getProtocol() {
	return (_protocol);
}