/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 12:02:51 by tching            #+#    #+#             */
/*   Updated: 2026/01/18 19:18:44 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"		
		
WebServer::WebServer() {}		
		
WebServer::WebServer(const std::string &file) : _correctConfig(false) {		
	_running = true;	
	_correctConfig = parseConfigFile(file);	
}		
		
WebServer::WebServer(const WebServer &other) {		
	_servers = other._servers;	
	_poll_fds = other._poll_fds;	
	_listeners = other._listeners;	
	_portsMap = other._portsMap;	
	_correctConfig = other._correctConfig;	
}		
		
WebServer &WebServer::operator=(const WebServer &other) {		
	if (this != &other) {	
		_servers = other._servers;
		_poll_fds = other._poll_fds;
		_listeners = other._listeners;
		_portsMap = other._portsMap;
		_correctConfig = other._correctConfig;
	}	
	return *this;	
}		
		
WebServer::~WebServer() {}

// Utils Functions
void my_usleep(unsigned long usec) {
    clock_t goal = usec + clock();
    while (goal > clock());
}

bool web_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++) {
		if (!std::isspace(line[i]))
			return line[i] == '#';
	}
	return true;
}

void insertNewPollfd(std::vector<pollfd> &_poll_fds, int socket) {
	pollfd listen_pollfd;
	listen_pollfd.fd = socket;
	listen_pollfd.events = POLLIN;
	_poll_fds.push_back(listen_pollfd);
}

int createNewListener(int port , std::vector<Server> servers) {
	int listening = socket(PF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		std::cerr << "ERROR (socket): Can't create a socket"<< std::endl;
		_exit(1);
	}

	int yes = 1;
	if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR,  &yes, sizeof(int)) == -1) {
		std::cerr << "ERROR (setsockopt): Failure"<< std::endl;
		_exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(server_addr.sin_zero), '\0', 8);

	if(bind(listening, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
		std::cerr << "ERROR (bind): Can't bind to IP/port"<< std::endl;
		_exit(1);
	}

	if (listen(listening, SOMAXCONN) == -1) {
		std::cerr << "ERROR (listen): Can't listen"<< std::endl;
		_exit (1);
	}

	std::vector<Server> ::const_iterator it_serv;
	std::cout << "Listening on port: " << port <<" [Servers: ";
	for (it_serv = servers.begin(); it_serv != servers.end(); it_serv++) {
		std::cout << (*it_serv).getServerName();
		if (it_serv + 1 != servers.end())
			std::cout << ", ";
	}
	std::cout << "]" << std::endl;

	return listening;
}

void initListeners(std::map<int, std::vector<Server> >& _portsMap, std::vector<pollfd>& _poll_fds, std::vector<int>& _listeners) {
	std::map<int, std::vector<Server> >::const_iterator it;
	std::map<int, std::vector<Server> >::const_iterator end = _portsMap.end();
	for (it = _portsMap.begin(); it != end; it++) {
		int listening = createNewListener(it->first, it->second);
		_listeners.push_back(listening);
		insertNewPollfd(_poll_fds, listening);
	}
}

std::string extractServerName(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;
	std::string srv;
	while (std::getline(bufferStream, line)) {
		if (line.find("Host:") != std::string::npos) {
			std::istringstream iss(line);
			std::string host;
			iss >> host >> srv;
			size_t pos = srv.find(".");
			if (pos != std::string::npos) {
				srv = srv.substr(0, pos);
				return srv;
			}
		}
	}
	return "";
}

std::string extractServerPort(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;
	std::string port;
	while (std::getline(bufferStream, line)) {
		if (line.find("Host:") != std::string::npos) {
			std::istringstream iss(line);
			std::string host;
			iss >> host >> port;
			size_t pos = port.find(":");
			if (pos != std::string::npos) {
                port = port.substr(pos + 1);
				return port;
			}
		}
	}
	return "";
}

const Server &WebServer::getServerConfig(char *buffer) {
    std::string srv = ::extractServerName(buffer);
    std::string port = ::extractServerPort(buffer);
	std::map<int, std::vector<Server> >::const_iterator itMap;
	std::map<int, std::vector<Server> >::const_iterator endMap = _portsMap.end();
	for (itMap = _portsMap.begin(); itMap != endMap; itMap++) {
		if (itMap->first == std::atoi(port.c_str())) {
			std::vector<Server>::const_iterator itServ;
			std::vector<Server>::const_iterator end = itMap->second.end();
			for (itServ = itMap->second.begin(); itServ != end; itServ++) {
				if (itServ->getServerName() == srv)
					return *itServ;
			}
			return *itMap->second.begin();
		}
	}
	return *(_portsMap.begin()->second.begin());
}

bool WebServer::correctConfig() const {
	return _correctConfig;
}

bool WebServer::continueServer(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;

	std::getline(bufferStream, line);
	if (line.find("shutdown") != std::string::npos) {
		_running = false;
		std::cout << "Server is shutting down" << std::endl;
	}
	return _running;
}

//Class Functions
bool WebServer::parseConfigFile(const std::string &file) {
	std::ifstream fileStream(file.c_str());
	std::string line;
	std::string buffer;
	int servers = 0;
	int checkEnd = 0;

	if (!fileStream.is_open()) {
		std::cerr << "ERROR (is_open): Can't open file"<< std::endl;
		return false;
	}
	while (std::getline(fileStream, line)) {
		if (web_isComment(line))
			continue;
			
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos) {
			buffer = line;
			buffer += "\n";
			checkEnd = 1;
			while (std::getline(fileStream, line) && checkEnd > 0) {
				buffer += line;
				buffer += "\n";
				if (line.find("{") != std::string::npos)
					checkEnd++;
				if (line.find("}") != std::string::npos)
					checkEnd--;
			}
			Server servAux(buffer);
			_servers.push_back(servAux);

			std::set<int>ports = servAux.getPort();
			std::set<int>::iterator it;
			for (it = ports.begin(); it != ports.end(); it++)
				_portsMap[*it].push_back(servAux);
            if (!servAux.checkConfig()) {
				std::cerr << "ERROR (checkConfig): Can't config server"<< std::endl;
                fileStream.close();
                return false;
            }
			servers++;
		}
	}
	fileStream.close();
	(void)servers;
    return true;
}

void WebServer::createNewClient(int it_listen) {
	sockaddr_in client;
	socklen_t addr_size = sizeof(sockaddr_in);

	int client_sock = accept(it_listen, reinterpret_cast<sockaddr*>(&client), &addr_size);

	if (client_sock == -1) {
		std::cerr << "ERROR (accept): Can't accept client"<< std::endl;
		return ;
	}
	if ((_poll_fds.size() - _listSize) < MAXCLIENTS) {
		insertNewPollfd(_poll_fds, client_sock);
		std::cout << "New client connected: " << client_sock << std::endl;
	}
	else {
		std::string page = Request::defaultErrorPage("503", "Service Unavailable");

		std::stringstream ss;
		int size = page.size();
		ss << size;

		_response.clear();
		_response += "HTTP/1.1 503 Service Unavailable\r\n";
		_response += "Content-Type: text/html\r\n";
		_response += "Content-Length:" + ss.str() + "\r\n";
		_response += "Connection: close\r\n\r\n";
		_response += page;
		std::cout << "Server is full" << std::endl;
		std::cout << "RESPONSE: " << _response << std::endl;
		send(client_sock, _response.c_str(), _response.size(), 0);
		close(client_sock);
	}
}

void WebServer::checkServers() {
	size_t it = 0;
	while (it < _listSize) {
		if(_poll_fds[it].revents & POLLIN)
			createNewClient(_poll_fds[it].fd);
		it++;
	}
}

std::string	extractSessionId(const char *requestBuffer, int bytesReceived) {
	std::string receivedData(requestBuffer, bytesReceived);
	size_t idPos = receivedData.find("session_id=");
	std::string	id;
	if (idPos != std::string::npos)
		id = receivedData.substr(idPos + 11, 6);
	else
		id = "42";
	return (id);
}

void WebServer::checkClients() {
	char buffer[1024];
	memset(buffer, 0, 1024);

	std::vector<pollfd>::iterator it = _poll_fds.begin() + _listSize;
	while (it != _poll_fds.end()) {
		if (it->revents & POLLIN) {
			int bytes = recv(it->fd, buffer, 1024, 0);
			if (bytes <= 0) {
				if (bytes == -1)
					std::cerr << "ERROR (recv): Client disconnected: " << it->fd << std::endl;
				else if (bytes == 0)
					std::cout << "OK: Client disconnected: " << it->fd << std::endl;
				close(it->fd);
				it = _poll_fds.erase(it);
				continue;
			}
			else if (bytes > 0) {
				std::map<int, Request>::iterator it_req = _clientRequests.find(it->fd);
				if (it_req != _clientRequests.end())
					it_req->second.parseBody(buffer, bytes);
				else {
					std::istringstream request(buffer);
					std::string request_line;
					getline(request, request_line);
					if (request_line.find("?") != std::string::npos)
						request_line = request_line.substr(0, request_line.find("?"));

					Server server = getServerConfig(buffer);

					std::string sessionId = extractSessionId(buffer, bytes);

					std::map<std::string, Cookie>::iterator it_cookie = _sessionCookie.find(sessionId);
					if (sessionId == "42" || it_cookie == _sessionCookie.end()) {
						Cookie cookie;
						_sessionCookie.insert(std::make_pair(cookie.getCookieId(), cookie));
						Request newRequest(buffer, server, cookie);
						_clientRequests.insert(std::make_pair(it->fd, newRequest));
					}
					else {
						it_cookie->second.checkCookieExpiry(buffer);
						Request newRequest(buffer, server, it_cookie->second);
						_clientRequests.insert(std::make_pair(it->fd, newRequest));
					}
					memset(buffer, 0, 1024);
				}
				for (std::map<int, Request>::iterator it_req = _clientRequests.begin(); it_req != _clientRequests.end(); ++it_req) {
					if (it->fd == it_req->first && it_req->second.isResponseReady()) {
						_response = it_req->second.getResponse();
						_clientRequests.erase(it_req);
						it->events = POLLOUT;
						break;
					}
				}
			}
		}
		if (it->revents & POLLOUT) {
			int bytes = send(it->fd, _response.c_str(), _response.size(), 0);
			if (bytes == -1) {
				close(it->fd);
				it = _poll_fds.erase(it);
				continue;
			}
			else {
				it->events = POLLIN;
				_response.clear();
			}
		}
		if (it->revents & ( POLLERR | POLLHUP)) {
			if (it->revents & POLLERR)
				std::cerr << "ERROR (POLLERR): Client disconnected: " << it->fd << std::endl;
			else
				std::cout << "OK (POLLHUP): Client disconnected: " << it->fd << std::endl;
			close(it->fd);
			it = _poll_fds.erase(it);
			continue;
		}
		it++;
	}
}

void WebServer::initService() {
	::initListeners(_portsMap, _poll_fds, _listeners);
	_listSize = _listeners.size();

	while (_running) {
		int ret = poll(reinterpret_cast<pollfd *>(&_poll_fds[0]), static_cast<unsigned int>(_poll_fds.size()), -1);
		if (ret == -1) {
			std::cerr << "ERROR (poll): Server is shutting down"<< std::endl;
			_running = false;
		}

		checkServers();
		checkClients();
		my_usleep(10000);
	}
}