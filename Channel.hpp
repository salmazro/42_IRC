/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salmazro <salmazro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/24 19:47:22 by salmazro          #+#    #+#             */
/*   Updated: 2023/06/24 22:43:03 by salmazro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*a channel is a virtual space where multiple users can join and communicate with each other.
 It is identified by a unique name, typically preceded by a '#' symbol.*/

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
//#include "User.hpp" :wait for alhai part

class Channel
{
	private:
	std::string	_channel_name;
	std::string	_pwd; //why password? password of the channel
	//std::vector<User *>	_users_name; :wait for alhai part

	public:

	// Constructor && Destructor
	Channel(std::string name, std::string password);
	~Channel(void);

	// Getter
	std::string getChannelName(void);
	std::string getPwd(void);
	//std::vector<User *> getUserName(void); :lets check later

	//Methods
	//User *getUserByNick(std::string nick); :lets check later
	//void addUser(User *user); :wait for alhai part
	//void removeUser(User *user); :wait for alhai part
	void messageFromChannel(std::string msg);
	void messageToChannel(std::string msg, int senderFd);
};

#endif
