/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 12:02:51 by tching            #+#    #+#             */
/*   Updated: 2026/01/18 18:30:00 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Libraries.hpp"

class Location
{
	private:
		std::string							_location; // path of the location block
		std::set<std::string>				_acceptedMethods; // allowed methods (GET, POST, DELETE)
		std::string							_root; // root directory for this location
		std::set<std::string>				_index; // list of index files
		bool								_directoryListing; // autoindex on/off
		std::map<std::string, std::string>	_cgiExtension; // map of file extension to CGI executable
		std::map<int, std::string>			_return; // redirection code and URL
		bool								_rootLocation; // flag to indicate if this is the root location

    public:
		Location();
		Location(const Location &other);
		Location(const std::string &config);
		Location& operator=(const Location &other);
		~Location();

		void parseConfig(const std::string &config);
		void printData() const;

		bool isAcceptedMethod(const std::string &method) const;
		std::string getLocation() const;
		std::string getRoot() const;
		std::set<std::string> getIndex() const;
		bool getDirectoryListing() const;
		std::map<std::string, std::string> getCgiExtension() const;
		std::map<int, std::string> getReturn() const;
		bool isIndexFile(std::string& fileName) const;
		bool	getRootLocation() const;
		std::string getCgiPath(const std::string &extension) const;
        
};

#endif