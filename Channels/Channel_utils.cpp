#include "Channel.hpp"

// Function to kick a user from a channel
void Channel::kickUser(Channel *channel, User *user, std::vector<std::string> messages)
{
	// Check if the command is valid and the user has sufficient privileges
	if (messages.size() < 3 || messages[0] != "KICK" || messages[1] != channel->getCname() || user->is_op(*this) == false)
		return;

	std::string nick = messages[2];
	std::vector<User *> users = channel->getUsers();

	// Iterate over the users in the channel to find the one to be kicked
	for(std::vector<User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if ((*it)->getNickName() == nick)
		{
			// Send kick message and remove the user from the channel
			channel->sendMessage(user->getNickName() + " has kicked " + nick + " from " + channel->getCname() + "!", "");
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(),":" + (*it)->getNickName() + " PART :" + channel->getCname() + "\n");
			channel->exitChannel(*it);
			break;
		}
	}
}

// Function to switch the mode of a channel
void Channel::execMode(User *user, std::vector<std::string> messages)
{
	bool mode_bool = false;

	// Check if the command is valid and the user has sufficient privileges
	if (messages.size() < 2 || messages[1] != this->getCname() || user->is_op(*this) == false)
		return;

	if (messages.size() == 2)
	{
		// Construct the mode string based on the channel's current modes
		std::string mode_str = "";
		if (this->channel_mode['o'] == true)
			mode_str += "o";
		if (this->channel_mode['i'] == true)
			mode_str += "i";
		if (this->channel_mode['t'] == true)
			mode_str += "t";
		if (this->channel_mode['k'] == true)
			mode_str += "k";
		if (this->channel_mode['l'] == true)
		{
			mode_str += "l ";
			mode_str += customUtils::convertToString(this->max_users);
		}

		// Send the mode information to the user
		irc::Server::serverInstance->sendMsg(user->getUserFd(), "324 " + user->getNickName() + " " + this->getCname() + " +" + mode_str + "\r\n");
	}

	std::string mode = messages[2];
	std::cout << "mode: " << mode << std::endl;

	// Check if the mode command is properly formatted
	if (mode.length() < 2 || (mode[0] != '+' && mode[0] != '-'))
		return;

	// Iterate over the mode characters and update the channel's modes accordingly
	for (unsigned int i = 0; i < mode.length(); i++)
	{
		if (mode[i] == '+')
			mode_bool = true;
		else if (mode[i] == '-')
			mode_bool = false;
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

	std::string mode_str = "";
	for (size_t i = 0; i < mode.length(); i++)
	{
		char c = mode[i];
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
	std::cout << "mode_str: " << mode_str << std::endl;
	this->sendMessage(":" + user->getNickName() + " MODE " + this->getCname() + " " + mode_str + "\r\n", "");
}

// Function to execute the TOPIC command for a channel
void Channel::execTopic(User *user, std::vector<std::string> messages)
{
	if (messages.size() < 3 || this->getCname() != messages[1])
		return;

	if (messages.size() == 2)
	{
		// Send the current topic of the channel to the user
		this->sendMessage(":irc 331 " + user->getNickName() + " " + this->getCname() + " " + this->getTopic(), "");
		return;
	}

	if (channel_mode['t'] == true && user->is_op(*this) == false)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 482 " + user->getNickName() + " " + this->getCname() + " :You're not channel operator\r\n");
		return;
	}

unsigned int startIndex = (messages[2] == ":") ? 3 : 2;
	std::string topic = "";

	// Construct the topic string
	if (messages.size() > 3 || messages[2] != ":")
	{
		topic = messages[startIndex];
		for (unsigned int i = startIndex + 1; i < messages.size(); i++)
			topic += " " + messages[i];
	}

	// Set the topic of the channel
	this->setTopic(topic);
	this->sendMessage(":irc 332 " + user->getNickName() + " " + this->getCname() + " " + this->getTopic(), "");
}

// Function to invite a user to a channel
void Channel::inviteUser(User *user, std::vector<std::string> messages)
{
	if (this->channel_mode['i'] == true && user->is_op(*this) == false)
		return;

	if (messages.size() < 3 || messages[2] != this->getCname())
		return;

	std::string nick = messages[1];

	// Iterate over the users in the server to find the one to be invited
	for(std::vector<User *>::iterator it = irc::Server::serverInstance->getUser().begin() ; it != irc::Server::serverInstance->getUser().end() ; ++it)
	{
		if ((*it)->getNickName() == nick)
		{
			(*it)->setInvited(*this, true);
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(), ":" + user->getNickName() + " INVITE " + nick + " " + this->getCname() + "\r\n");
			break;
		}
	}
}

// Function to handle server messages
bool server_messages(std::vector<std::string> messages)
{
	std::string message = messages[0];

	// Check if the message is a WHOIS or MODE command targeting the FT_irc_server
	if (message == "WHOIS" || (message == "MODE" && messages[1] == "FT_irc_server"))
	{
		return true;
	}

	return false;
}

// Function to handle the NICK command for a user
void handle_nickname(User *user, std::vector<std::string> messages)
{
	std::string nick = messages[1];
	std::cout << "nick: " << nick << std::endl;

	// Check if the nickname is already in use
	if (irc::Server::serverInstance->check_duplicate(nick) == true)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 433, Nickname is already in use\r\n");
		return;
	}

	irc::Server::serverInstance->sendMsg(user->getUserFd(), "You have changed your nickname to " + nick + "\r\n");
	user->setNickName(nick);
}

// Function to handle joining a channel
void join_channel(std::string chnl_name , User *user, Channel *channel, std::string password)
{
	// Check if the channel exists, otherwise create it
	if (channel == NULL)
	{
		channel = irc::Server::serverInstance->createChannel(chnl_name, password);
	}

	// Check if the password matches the channel's key
	if (password != channel->getPwd())
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":irc 475 " + user->getNickName() + " " + chnl_name + " :Incorrect Channel Key\n");
		return;
	}

	// Check if the channel has reached its maximum user limit
	if (channel->users.size() >= channel->getMusers())
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":irc 471 " + user->getNickName() + " " + chnl_name + " :Cannot join channel (+l)\n");
		return;
	}

	// Check if the channel is invite-only and the user has not been invited
	if (channel->getMode()['i'] == true && user->getInvited(*channel) == false)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":irc 473 " + user->getNickName() + " " + chnl_name + " :Cannot join channel (+i)\n");
		return;
	}

	// Join the channel
	joinChannel(user, channel);
	irc::Server::serverInstance->sendMsg(user->getUserFd(), ":irc 332 " + user->getNickName() + " " + channel->getCname() + " " + channel->getTopic() + "\r\n");

	// Notify other users in the channel about the user's join
	for (size_t i = 0; i < channel->users.size(); i++)
	{
		std::string msg2 = ":" + user->getNickName() + " JOIN " + chnl_name + " \r\n";
		irc::Server::serverInstance->sendMsg(channel->users.at(i)->getUserFd(), msg2);
	}
}

// Function to check if a message is a valid command
bool isCommand(std::string message)
{
	if (message == "NICK" || message == "USER" || message == "JOIN" || message == "PART" || message == "PRIVMSG" || message == "QUIT" || message == "MODE" || message == "INVITE" || message == "TOPIC")
		return true;

	return false;
}

void executeCommand(std::vector<std::string> messages, User *user)
{
	// Get the command from the messages vector
	std::string message = messages[0];

	// Check if the message is a valid command
	if (isCommand(message) == false)
	{
		// Send an error message if the command is unknown
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 421, Unknown command\r\n");
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
		if (messages.size() < 2)
			return;

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
		if (messages.size() < 3)
			return;

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
				irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 401, No such nick/channel\r\n");
			}
		}
		// Check if the recipient is a channel
		else if (messages[1][0] == '#')
		{
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
				irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 401, No such nick/channel\r\n");
				return;
			}

			channel = *it;
			msg = messages[2];

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
				irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 401, User not in channel\r\n");
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
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 442, You're not on that channel\r\n");
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
