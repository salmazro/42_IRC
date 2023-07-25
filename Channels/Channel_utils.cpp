#include "Channel.hpp"

/*                              Channel Command                              */

// This function kicks a user from a specific IRC channel.
void Channel::kickUser(Channel *channel, User *user, std::vector<std::string> messages)
{

	// First, we verify the command validity and check if the user has operator status.
	// The kick command requires at least 3 arguments and the executing user must be an operator of the channel.
	if (messages.size() < 3 || messages[0] != "KICK" || messages[1] != channel->getCname() || user->is_op(*this) == false)
		return;

	// The nickname of the user to be kicked is the third argument of the command.
	std::string nick = messages[2];

	// We retrieve the list of users currently in the channel.
	std::vector<User *> users = channel->getUsers();

	// Iterate over the users in the channel to find the one to be kicked
	for(std::vector<User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if ((*it)->getNickName() == nick) // If the user is found,
		{
			// Send a message to the channel notifying that the user has been kicked.
			channel->sendMessage(user->getNickName() + " has kicked " + nick + " from " + channel->getCname() + "!", "");

			// Send a message to the kicked user notifying that they have been removed from the channel.
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(),":" + (*it)->getNickName() + " PART :" + channel->getCname() + "\n");

			channel->exitChannel(*it); // Remove the user from the channel.
			break; // Once the user is kicked, we break out of the loop.
		}
	}
}

// This function invites a user to join a specific IRC channel.
void Channel::inviteUser(User *user, std::vector<std::string> messages)
{
	// If the channel is in invite-only mode and the user is not an operator of the channel,
	// the function returns without doing anything.
	if (this->channel_mode['i'] == true && user->is_op(*this) == false)
		return;

	// If the invite command does not have enough arguments or the third argument is not this channel,
	// the function returns without doing anything.
	if (messages.size() < 3 || messages[2] != this->getCname())
		return;

	std::string nick = messages[1]; // The nickname of the user to be invited is the second argument of the command.

	// We iterate over all the users in the server to find the one that matches the nickname provided in the invite command.
	for(std::vector<User *>::iterator it = irc::Server::serverInstance->getUser().begin() ; it != irc::Server::serverInstance->getUser().end() ; ++it)
	{
		// If the user is found,
		if ((*it)->getNickName() == nick)
		{
			(*it)->setInvited(*this, true);  // Set the 'invited' status of the user to true for this channel.

			// Send a message to the invited user notifying that they have been invited to this channel.
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(), ":" + user->getNickName() + " INVITE " + nick + " " + this->getCname() + "\r\n");

			break; // Once the user is invited, we break out of the loop.
		}
	}
}

// This function implements the TOPIC command in an IRC (Internet Relay Chat) system.
// The TOPIC command is used to change or retrieve the topic of a chat channel.
void Channel::execTopic(User *user, std::vector<std::string> messages)
{
	// If the number of command arguments is less than 3 or the target channel is not this channel,
	// the function returns without doing anything.
	if (messages.size() < 3 || this->getCname() != messages[1])
		return;

	// If the TOPIC command has only one argument (the channel name),
	// then the current topic of the channel is returned to the user.
	if (messages.size() == 2)
	{
		// Send the current topic of the channel to the user
		this->sendMessage("IRC: 331 " + user->getNickName() + " " + this->getCname() + " " + this->getTopic(), "");
		return;
	}

	// If the channel is in 't' mode (only operators can change the topic) and the user is not an operator,
	// the user is notified that they do not have the necessary privileges.
	if (channel_mode['t'] == true && user->is_op(*this) == false)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 482 " + user->getNickName() + " " + this->getCname() + " :You're not channel operator\r\n");
		return;
	}

	// Determine the starting index of the topic in the command arguments
	unsigned int startIndex = (messages[2] == ":") ? 3 : 2;
	std::string topic = "";

	// If there are more than 3 command arguments or the third argument is not a colon,
	// the function constructs the new topic string from the command arguments.
	if (messages.size() > 3 || messages[2] != ":")
	{
		topic = messages[startIndex];
		for (unsigned int i = startIndex + 1; i < messages.size(); i++)
			topic += " " + messages[i];
	}

	// Set the new topic of the channel and notify the user that the topic has been set.
	this->setTopic(topic);
	this->sendMessage("IRC: 332 " + user->getNickName() + " " + this->getCname() + " " + this->getTopic(), "");
}

// This function implements the MODE command in an IRC (Internet Relay Chat) system.
// The MODE command is used to change or retrieve the mode (settings) of a chat channel.
void Channel::execMode(User *user, std::vector<std::string> messages)
{
	// Initialize mode_bool to false. This will be used to track whether we're adding (+) or removing (-) a mode.
	bool mode_bool = false;

	// If the command is not valid (has less than 2 arguments) or the target channel is not this one,
	// or the user executing the command is not an operator, return without making changes.
	if (messages.size() < 2 || messages[1] != this->getCname() || user->is_op(*this) == false)
		return;

	// If the MODE command has only one argument (the channel name),
	// the current modes of the channel are returned to the user.
	if (messages.size() == 2)
	{
		// Construct a string of the current modes based on the channel's mode settings.
		std::string mode_str = "";
		if (this->channel_mode['o'] == true) // 'o' represents channel operators
			mode_str += "o";
		if (this->channel_mode['i'] == true) // 'i' represents invite-only channels
			mode_str += "i";
		if (this->channel_mode['t'] == true) // 't' represents channels where only operators can change the topic
			mode_str += "t";
		if (this->channel_mode['k'] == true) // 'k' represents channels that require a password to join
			mode_str += "k";
		if (this->channel_mode['l'] == true) // 'l' represents channels with a user limit
		{
			mode_str += "l ";
			mode_str += customUtils::convertToString(this->max_users); // Append the user limit if applicable
		}

		// Send the mode information to the user
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "324 " + user->getNickName() + " " + this->getCname() + " +" + mode_str + "\r\n");
	}

	// The command argument that specifies the modes to change is stored in the variable mode.
	std::string mode = messages[2];
	std::cout << "mode: " << mode << std::endl;

	// If the mode argument is incorrectly formatted (less than 2 characters or does not start with '+' or '-'), return without making changes.
	if (mode.length() < 2 || (mode[0] != '+' && mode[0] != '-'))
		return;

	// Iterate over the characters in the mode string.
	// Each character represents a mode that should be added or removed, based on whether the current character is '+' or '-'.
	// After processing each mode character, the function updates the channel's mode settings accordingly.
	for (unsigned int i = 0; i < mode.length(); i++)
	{
		// If the mode is '+', we are adding a mode.
		if (mode[i] == '+')
			mode_bool = true;

		// If the mode is '-', we are removing a mode.
		else if (mode[i] == '-')
			mode_bool = false;

		// Handling of 'o', 'i', 't', 'l', 'k' modes...
		else if (mode[i] == 'o' && messages.size() > 3)
		{
			if (mode[0] == '+' && messages[3].length() > 0)
			{
				std::string nick = messages[3];
				for(std::vector<User *>::iterator it = irc::Server::serverInstance->_users.begin(); it != irc::Server::serverInstance->_users.end(); ++it)
				{
					if ((*it)->getNickName() == nick)
					{
						(*it)->setChannelOp(*this, mode_bool);
						if ((*it)->is_op(*this) == true)
							break;
					}
				}
				this->channel_mode['o'] = mode_bool;
			}
			else
				this->channel_mode['o'] = mode_bool;
		}
		else if (mode[i] == 'i')
		{
			this->channel_mode['i'] = mode_bool;
		}
		else if (mode[i] == 't')
		{
			this->channel_mode['t'] = mode_bool;
		}
		else if (mode[i] == 'l' && messages.size() > 3)
		{
			if (mode[0] == '+' && std::atoi(messages[3].c_str()) > 0 && std::atoi(messages[3].c_str()) < 1000)
			{
				this->max_users = std::atoi(messages[3].c_str());
				this->channel_mode['l'] = true;
			}
			else
			{
				this->channel_mode['l'] = false;
				this->max_users = 1000;
			}
		}
		else if (mode[i] == 'k' && messages.size() > 3)
		{
			if (mode[0] == '+' && messages[3].length() > 0)
			{
				this->channel_mode['k'] = true;
				this->channel_pwd = messages[3];
			}
			else
			{
				this->channel_mode['k'] = false;
				this->channel_pwd = "";
			}
		}
		else
			return;
	}

	// Construct a mode string to display back to the user, similar to how the current modes were returned earlier.
	std::string mode_str = "";
	for (size_t i = 0; i < mode.length(); i++)
	{
		char c = mode[i];

		// Switch statement to handle each possible mode character...
		switch (c)
		{
			case ('+'):
				mode_str += "+";
				break;
			case ('-'):
				mode_str += "-";
				break;
			case ('o'):
				mode_str += "o";
				break;
			case ('i'):
				mode_str += "i";
				break;
			case ('t'):
				mode_str += "t";
				break;
			case ('l'):
				mode_str += "l";
				break;
			case ('k'):
				mode_str += "k";
				break;
		}
	}

	// Print the mode string and send a MODE message to the user indicating the new channel modes.
	std::cout << "mode_str: " << mode_str << std::endl;
	this->sendMessage(":" + user->getNickName() + " MODE " + this->getCname() + " " + mode_str + "\r\n", "");
}

/*                              Error Handeling                              */

// This function handles server messages for an IRC-like system.
// It checks if the received command is a WHOIS or a MODE command targeting the FT_irc_server.
// If the command is one of these, the function returns true; otherwise, it returns false.
bool server_messages(std::vector<std::string> messages)
{
	// Get the first part of the command from the input message list. This should contain the type of command.
	std::string message = messages[0];

	// Check if the command is a WHOIS or a MODE command targeting the FT_irc_server.
	// The WHOIS command is used to retrieve information about a user, while the MODE command is used to change or view channel modes.
	// In this case, if the command is a WHOIS, or if it's a MODE command specifically for the FT_irc_server, the function will return true.
	if (message == "WHOIS" || (message == "MODE" && messages[1] == "FT_irc_server"))
	{
		return true;
	}

	// If the command is not WHOIS or a MODE command for FT_irc_server, return false.
	return false;
}

// This function generates and throws a runtime error message.
void	errorMessage(std::string function, std::string error)
{
	throw std::runtime_error(function + " " + error);
}

// This function sends a numeric response back to the client.
// A numeric response is a server response that consists of a three-digit number and is used in the IRC protocol.
// The message, the response code, the file descriptor of the client, an optional string, and a pointer to the user object are passed as arguments.
void	numericResponse(std::string msg, std::string code, int fd, std::string opt, User *user)
{
	std::string	response;

	// Get the nickname of the user. If the nickname is not set (is empty), use "Unknown" as the nickname.
	std::string	nick = user->getNickName();
	if (nick.empty())
		nick = "Unknown";

	// Construct the response string starting with the server IP, then the response code, and the nickname.
	response = ":127.0.0.1 " + code + " " + nick + " ";

	// If the optional string is not empty, append it to the response.
	if (opt != "")
		response += opt + " ";

	// Append the message and the carriage return and line feed characters to the response.
	response += msg + "\r\n";

	// If the file descriptor is 0, set it to the user's file descriptor.
	if (fd == 0)
		fd = user->getUserFd();

	// Send the response string to the client using the send function.
	// If sending fails, call the errorMessage function with the function name and the error message.
	if (send(fd, response.c_str(), strlen(response.c_str()), 0) < 0)
		errorMessage("numericResponse: send:", strerror(errno));
}

// This function is used to handle the nickname change command (NICK) for a user in an IRC (Internet Relay Chat) server.
// The function takes a pointer to a User object and a vector of messages as its parameters.
void handle_nickname(User *user, std::vector<std::string> messages)
{
	// Get the new nickname from the messages vector. It is expected to be the second element in the vector.
	std::string nick = messages[1];

	// Output the new nickname to the console for debugging.
	std::cout << "nick: " << nick << std::endl;

	// Use the server's check_duplicate method to check if the new nickname is already in use by another user on the server.
	if (irc::Server::serverInstance->check_duplicate(nick) == true)
	{
		// If the nickname is already in use, send a message back to the user informing them that the nickname is taken.
		// 433 is the numeric reply for "Nickname is already in use".
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 433, Nickname is already in use\r\n");
		return;
	}

	// If the nickname is not in use, send a message to the user confirming that their nickname has been changed.
	irc::Server::serverInstance->sendMsg(user->getUserFd(), "You have changed your nickname to " + nick + "\r\n");

	// Update the nickname in the user's User object.
	user->setNickName(nick);
}

// This function handles a user's request to join a channel in an IRC (Internet Relay Chat) server.
// The function takes a channel name, a pointer to a User object, a pointer to a Channel object, and a password string as its parameters.
void join_channel(std::string chnl_name , User *user, Channel *channel, std::string password)
{
	// If the pointer to the Channel object is NULL, the channel does not exist yet.
	// So, we create a new channel using the server's createChannel method.
	if (channel == NULL)
	{
		channel = irc::Server::serverInstance->createChannel(chnl_name, password);
	}

	// Check if the password provided by the user matches the channel's password.
	// If it does not match, send an error message to the user.
	if (password != channel->getPwd())
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "IRC: 475 " + user->getNickName() + " " + chnl_name + " :Incorrect Channel Key\n");
		return;
	}

	// Check if the channel's user limit has been reached.
	// If it has, send an error message to the user.
	if (channel->users.size() >= channel->getMusers())
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "IRC: 471 " + user->getNickName() + " " + chnl_name + " :Cannot join channel (+l)\n");
		return;
	}

	// Check if the channel is set to invite-only mode, and the user has not been invited.
	// If the channel is invite-only and the user has not been invited, send an error message to the user.
	if (channel->getMode()['i'] == true && user->getInvited(*channel) == false)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "IRC: 473 " + user->getNickName() + " " + chnl_name + " :Cannot join channel (+i)\n");
		return;
	}

	// If the user meets all the requirements to join the channel, call the joinChannel method to add the user to the channel.
	joinChannel(user, channel);

	// Send a message to the user, informing them that they have successfully joined the channel.
	irc::Server::serverInstance->sendMsg(user->getUserFd(), "IRC: 332 " + user->getNickName() + " " + channel->getCname() + " " + channel->getTopic() + "\r\n");

	// Iterate over all the users in the channel and send each of them a message informing them that the user has joined the channel.
	for (size_t i = 0; i < channel->users.size(); i++)
	{
		std::string msg2 = ":" + user->getNickName() + " JOIN " + chnl_name + " \r\n";
		irc::Server::serverInstance->sendMsg(channel->users.at(i)->getUserFd(), msg2);
	}
}

// This function checks if a given string represents a valid IRC (Internet Relay Chat) command.
// IRC commands are predefined strings that trigger specific actions on the IRC server.
bool isCommand(std::string message)
{
	if (message == "NICK" || message == "USER" || message == "JOIN" || message == "PART" || message == "PRIVMSG" || message == "QUIT" || message == "MODE" || message == "INVITE" || message == "TOPIC" || message == "KICK"|| message == "PING")
		return true; // If the message matches any of the valid commands, return true.

	return false; // If the message does not match any of the valid commands, return false.
}

/*                              General Command                              */

// This function executes an IRC command given a vector of messages and a pointer to the user object.
void executeCommand(std::vector<std::string> messages, User *user)
{
	// Get the command from the messages vector
	std::string message = messages[0];

	// Check if the message is a valid command
	if (isCommand(message) == false)
	{
		// Send an error message if the command is unknown
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 421, Unknown command\r\n");
		return;
	}

	// Get the channel object based on the message recipient
	Channel *channel = irc::Server::serverInstance->getChannel(messages[1]);

	// If the command is INVITE, retrieve the channel object from messages[2]
	if (message == "INVITE")
		channel = irc::Server::serverInstance->getChannel(messages[2]);

	// Check if the message is a WHOIS or MODE command targeting the FT_irc_server
	if (server_messages(messages) == true)
		return;

	// Process the JOIN command
	if (message == "JOIN")
	{
		// Check for invalid message structure
		if (messages.size() < 2 || messages.size() > 3 || (messages.size() == 3 && messages[2].empty()))
			return numericResponse("usage: /JOIN <channels> [<keys>]", "461", user->getUserFd(), "", user);

		// Iterate over channels the user wants to join
		size_t x = 0;
		while (x < user->_channelToJoin.size())
		{
			// Ensure the channel name starts with #
			if (user->_channelToJoin.at(x).at(0) != '#')
				user->_channelToJoin.at(x).insert(0, "#");

			// Retrieve the channel object
			channel = irc::Server::serverInstance->getChannel(user->_channelToJoin.at(x));
			// Join the channel with or without a password
			if (user->_channelKeys.size())
				join_channel(user->_channelToJoin.at(x), user, channel, user->_channelKeys.at(x));
			else
				join_channel(user->_channelToJoin.at(x), user, channel, "");

			x++;
		}
		return;
	}
	// Process the PING command
	else if (message == "PING")
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "PONG\r\n");
		return;
	}
	// Process the PRIVMSG command
	else if (message == "PRIVMSG")
	{
		// check if the command structure is valid
		if (message.size() == 1)
			return numericResponse("A nick must be provided!", "411", user->getUserFd(), "", user);

		if (messages.size() < 3)
			return numericResponse("usage: PRIVMSG <NICK> [Message]", "412", user->getUserFd(), "", user);

		std::string msg = messages[2];

		// Check if the recipient is a user
		if (messages[1][0] != '#')
		{
			// Check if the recipient user exists
			if (irc::Server::serverInstance->getFdByNick(messages[1]) != -1)
			{
				msg = messages[2];
				for (unsigned int i = 3; i < messages.size(); i++)
					msg += " " + messages[i];

				// Prepare and send the PRIVMSG message to the recipient user
				std::string msg2 = ":" + user->getNickName() + " PRIVMSG " + messages[1] + " " + msg + "\r\n";
				std::cout << "sending: " << msg2 << std::endl;
				irc::Server::serverInstance->sendMsg(irc::Server::serverInstance->getFdByNick(messages[1]), msg2);
			}

			else
			{
				// Send an error message if the recipient user does not exist
				return numericResponse("No such nick/channel", "401", user->getUserFd(), "", user);
			}
		}
		// Check if the recipient is a channel
		else if (messages[1][0] == '#')
		{
		// Get the list of channels
		std::vector<Channel *> channels = irc::Server::serverInstance->getChannels();
		std::vector<Channel *>::iterator it;
		std::cout << "channel name: " << messages[1] << std::endl;

		// Find the channel with the given name
		for (it = channels.begin(); it != channels.end(); ++it)
		{
			if ((*it)->getCname() == messages[1])
			{
				std::cout << "channel name: " << (*it)->getCname() << std::endl;
				break;
			}
		}

		// If the channel is not found, send an error message
		if (it == channels.end())
		{
			return numericResponse("No such nick/channel", "401", user->getUserFd(), "", user);
		}

		// Assign the found channel to the 'channel' variable
		channel = *it;
		msg = messages[2];

		// Find the user in the channel's user list
		std::vector<User *>::iterator it2;
		for (it2 = channel->users.begin(); it2 != channel->users.end(); ++it2)
		{
			if ((*it2)->getNickName() == user->getNickName())
			{
				std::cout << "found user" << std::endl;
				break;
			}
		}

		// Check if the user is a member of the channel
		if (it2 == channel->users.end())
		{
			// Send an error message if the user is not in the channel
			irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 401, User not in channel\r\n");
			return;
		}

		// Concatenate the message parts
		for (unsigned int i = 3; i < messages.size(); i++)
			msg += " " + messages[i];

		// Prepare and send the PRIVMSG message to the channel
		std::string msg2 = ":" + user->getNickName() + " PRIVMSG " + messages[1] + " " + msg + "\r\n";
		channel->sendMessage(msg2, user->getNickName());
		}

		return;
	}
	// Process the NICK command
	else if (message == "NICK")
	{
		handle_nickname(user, messages);
	}

	// Check if the user is not in the specified channel
	if (channel == NULL)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 442, You're not on that channel\r\n");
		return;
	}

	// Process the PART command
	if (message == "PART")
	{
		std::string msg = ":" + user->getNickName() + " PART :" + channel->getCname() + "\r\n";
		irc::Server::serverInstance->sendMsg(user->getUserFd(), msg);
		channel->sendMessage(":" + user->getNickName() + " PART " + channel->getCname() + " :leaving", user->getNickName());

		if (channel->exitChannel(user) == true)
		{
			return;
		}
	}
	// Process the MODE command
	else if (message == "MODE")
	{
		channel->execMode(user, messages);
	}
	// Process the TOPIC command
	else if (message == "TOPIC")
	{
		channel->execTopic(user, messages);
	}
	// Process the KICK command if the user is a channel operator
	else if (message == "KICK" && user->is_op(*channel) == true)
	{
		channel->kickUser(channel, user, messages);
	}
	// Process the INVITE command
	else if (message == "INVITE")
	{
		channel->inviteUser(user, messages);
	}
}
