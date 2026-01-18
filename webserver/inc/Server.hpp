/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 12:02:51 by tching            #+#    #+#             */
/*   Updated: 2026/01/18 18:26:22 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Libraries.hpp"
#include "Location.hpp"

class Server
{
    private:
        std::set<int>                       _port; // to store the ports server listens to
        std::string                         _serverName; //NGINX uses server_name directive
        std::map<int, std::string>          _errorPages; // map of error code to error page path
        int                                 _maxSize; // NGINX uses client_max_body_size directive
        std::vector<Location>               _locations; // list of location block in the server


    public:
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
        Server(const std::string &serverConfig);

        ~Server();

        bool checkConfig();
        
        void printData();
        void setLocation(Location location);
        void parseServer(const std::string &serverConfig);
        
        std::set<int>           getPort() const;
        std::string             getServerName() const;
        std::string             getErrorPage(const int &errorCode) const;
        int                     getMaxSize() const;
        
        const Location          &getLocation(const std::string &location) const;
        
};

#endif