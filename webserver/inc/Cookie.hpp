/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 12:02:51 by tching            #+#    #+#             */
/*   Updated: 2026/01/18 18:36:09 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
# define COOKIE_HPP

#include "Libraries.hpp"

#define MAXAGE 10

class Cookie {
	public:
		Cookie();
		~Cookie();
		void	checkCookieExpiry(const char *requestBuffer);
		void	generateTimeStamp();
		static std::string	generateRandomString(int length);
		std::string	formatTime(std::time_t timeInSeconds);
		std::string	getCookieBody() const;
		std::string	getCookieHeader() const;
		std::string	getCookieId() const;
		std::string	setCookieHeader();
		bool	isExpired() const;
		int		getFirstAccessTime() const;
		int		getCookieMaxAge() const;
	private:
		int			_cookieMaxAge;
		std::string	_cookieSessionId;
		int			_firstAccessTime;
		int			_lastEntry;
		std::vector<std::string> _loginHistory;
		std::string	_cookieHeader;
};

#endif