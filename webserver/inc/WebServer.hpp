/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 12:02:51 by tching            #+#    #+#             */
/*   Updated: 2026/01/18 18:38:30 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "Libraries.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Cookie.hpp"

#define MAXCLIENTS 150

class Server;
class Request;
class Cookie;

class WebServer
{
	private:
		std::vector<Server>	                _servers;
		std::vector<pollfd>			        _poll_fds;
		std::vector<int>			        _listeners;
		size_t							    _listSize;
		std::map<int, std::vector<Server> > _portsMap;
        bool                                _correctConfig;
		bool 								_running;
		std::string 						_response;
		std::map<int, Request>				_clientRequests;
		std::map<std::string, Cookie>		_sessionCookie;
    
    public:
		WebServer();
		WebServer(const std::string &file);
		WebServer(const WebServer &other);
		WebServer& operator=(const WebServer &other);
		~WebServer();

		bool parseConfigFile(const std::string &file);
		void initService();
		void createNewClient(int it_listen);
		const Server &getServerConfig(char *buffer);
		bool correctConfig() const;
		bool continueServer(char *buffer);

		void checkServers();
		void checkClients();
};

#endif
