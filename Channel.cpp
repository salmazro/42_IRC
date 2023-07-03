/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salmazro <salmazro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 20:55:50 by salmazro          #+#    #+#             */
/*   Updated: 2023/07/03 21:43:36 by salmazro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//This is the constructor of the Channel class : done
Channel::Channel(std::string name, std::string pwd) {
	_channel_name = name;
	_pwd = pwd;
}

//This is the destructor of the Channel class
Channel::~Channel(void)
{
	return ;
}

//This function returns a std::string representing the name of the channel
std::string Channel::getChannelName(void)
{
	return (this->_channel_name);
}


//This function returns a std::string representing the password of the channel
std::string	Channel::getPwd(void)
{
	return (this->_pwd);
}

/*

!!! open this comment first !!!

//This function returns a std::vector<User *>, representing the list of users currently in the channel
std::vector<User *>	Channel::getUsersname( void )
{
	return (this->_users_name);
}

//The purpose of this function is to find and return a pointer to the User object that corresponds to the specified nickname in the channel : done
// ***** Lets try this way *****
User* Channel::getUserNickname(std::string nick)
{
	// Iterate over the list of users
	for (std::vector<User*>::iterator it = _users_name.begin(); it != _users_name.end(); it++)
	{
		if (nick[0] == ':')
			nick.erase(0, 1);
		// Compare the nicknames and return the User pointer if found
		if ((*it)->getNick() == nick)
			return *it;
	}
	// Return NULL if the user with the specified nickname is not found
	return (NULL);
}

//This function is used to add a new user to the channel : done
void Channel::addUser(User* user)
{
	if (getUserByNick(user->getNick()) == NULL)
	{
		_users_name.push_back(user);
	}
}

//This function is used to remove a specific user from the channel : done
void Channel::removeUser(User* user)
{
	for (std::vector<User*>::iterator it = _users_name.begin(); it != _users_name.end(); ++it)
	{
		if (*it == user)
		{
			_users_name.erase(it);
			return;
		}
	}
}
*/

/*

!!! open this comment last !!!

//This function is used to send a message to all users in the channel : done
void Channel::messageFromChannel(std::string msg)
{
	// Iterate over the list of users
	for (std::vector<User*>::iterator it = _users_name.begin(); it != _users_name.end(); ++it)
	{
		// Ensure that the message ends with a proper CRLF sequence.
		if (msg.find("\r\n") == std::string::npos)
			msg += "\r\n";

		// Send the message to the user's socket
		if (send((*it)->getFd(), msg.c_str(), msg.length(), 0) < 0)
			ft::errorMessage("messageFromChannel: send:", strerror(errno));
	}
}

//This function is used to send a message to all users in the channel except the user who sent the message : done 
void Channel::messageToChannel(std::string msg, int senderFd)
{
	// Ensure that the message ends with a proper CRLF sequence.
	if (msg.find("\r\n") == std::string::npos)
		msg += "\r\n";

	// Iterate over the list of users
	for (std::vector<User*>::iterator it = _users_name.begin(); it != _users_name.end(); ++it)
	{
		// Skip sending the message to the user who sent it
		if ((*it)->getFd() != senderFd)
		{
			// Send the message to the user's socket
			if (send((*it)->getFd(), msg.c_str(), msg.length(), 0) < 0)
				ft::errorMessage("messageToChannel: send:", strerror(errno));
        }
    }
}

*/
