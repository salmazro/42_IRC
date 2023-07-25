#include "Channel.hpp"

// Constructor
Channel::Channel(std::string _name)
{
	this->channel_name = _name;
	this->max_users = 1000;
	this->channel_mode['o'] = false;
	this->channel_mode['k'] = false;
	this->channel_mode['t'] = false;
	this->channel_mode['i'] = false;
	this->channel_mode['l'] = false;
	this->channel_topic = "";
	this->channel_pwd = "";
}

// Destructor
Channel::~Channel()
{
	// irc::Server::serverInstance->deleteChannel(this);
}

// Overloaded equality operator for Channel objects
bool Channel::operator==(Channel const &rhs) const
{
	// Compare the channel names of the current object and the provided object
	// If they are equal, return true; otherwise, return false
	if (this->channel_name == rhs.channel_name)
		return true;
	return false;
}

// Overloaded less-than operator for Channel objects
bool Channel::operator<(Channel const &rhs) const
{
	// Compare the channel names of the current object and the provided object
	// If the channel name of the current object is less than the channel name of the provided object, return true; otherwise, return false
	if (this->channel_name < rhs.channel_name)
		return true;
	return false;
}

/*                              SETTERS && GETTERS                              */

// Set the maximum number of users
void Channel::setMusers(unsigned int maxUsers)
{
	this->max_users = maxUsers;
}

// Set the topic of the channel
void Channel::setTopic(std::string topic)
{
	this->channel_topic = topic;
}

// Set the key (password) of the channel
void Channel::setPwd(std::string password)
{
	this->channel_pwd = password;
}

// Get the name of the channel
std::string Channel::getCname()
{
	return this->channel_name;
}

// Get the list of users in the channel
std::vector<User *> Channel::getUsers()
{
	return this->users;
}

// Get the maximum number of users in the channel
unsigned int Channel::getMusers()
{
	return this->max_users;
}

// Get the modes of the channel
std::map<char, bool> Channel::getMode()
{
	return this->channel_mode;
}

// Get the topic of the channel
std::string Channel::getTopic()
{
	return this->channel_topic;
}

// Get the key (password) of the channel
std::string Channel::getPwd()
{
	return this->channel_pwd;
}

/*                              SERVER FUNCTIONS                              */

// Create a channel
Channel *irc::Server::createChannel(std::string name, std::string password)
{
	// Check if the channel name is valid
	if ((name[0] != '#' && name[0] != '&') || name.length() > 100)
		return (NULL);

	// Check if the channel name contains invalid characters
	if (name.find(' ') != std::string::npos || name.find(',') != std::string::npos)
		return (NULL);

	// Check if a channel with the same name already exists
	for (std::vector<Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		if ((*it)->getCname() == name)
			return (*it);
	}

	// Create a new channel and set its password
	Channel *newChannel = new Channel(name);
	newChannel->setPwd(password);

	// Add the new channel to the list of channels
	this->_channels.push_back(newChannel);

	return newChannel;
}

// Get a channel by name
Channel *irc::Server::getChannel(std::string name)
{
	// Iterate over the list of channels
	for (std::vector<Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		// Check if the channel name matches the provided name
		if ((*it)->getCname() == name)
			return (*it);
	}

	// Return null if the channel with the provided name is not found
	return NULL;
}

// Delete a channel
void irc::Server::deleteChannel(Channel* channel)
{
	// Find the iterator pointing to the channel in the vector
	std::vector<Channel*>::iterator it = std::find(this->_channels.begin(), this->_channels.end(), channel);

	// Check if the channel is found in the vector
	if (it != this->_channels.end())
	{
		// Delete the channel object and erase it from the vector
		delete *it;
		this->_channels.erase(it);
	}

	// Return if the channel is not found
	else
		return;
}

/*                              Channel FUNCTIONS                              */

// Allow a user exit the channel
bool Channel::exitChannel(User *user)
{
	// Find the iterator pointing to the user in the vector
	std::vector<User *>::iterator it = std::find(this->users.begin(), this->users.end(), user);

	// Check if the user is found and the channel has only one user
	if (it != this->users.end() && this->users.size() == 1)
	{
		// Set the user's channel operator status to false
		user->setChannelOp(*this, false);

		// Send a PART message to the user to indicate they are leaving the channel
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":" + user->getNickName() + " PART " + this->getCname() + " :leaving " + "\r\n");

		// Remove the user from the channel and delete the channel
		this->users.erase(it);
		irc::Server::serverInstance->deleteChannel(this);

		return false;
	}

	// Check if the user is found in the channel
	else if (it != this->users.end())
	{
		// Set the user's channel operator status to false
		user->setChannelOp(*this, false);

		// Remove the user from the channel
		this->users.erase(it);

		return true;
	}

	else
	{
		// Send an error message to the user indicating they are not in the channel
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 442, You are not in channel " + this->getCname() + ".\r\n");

		return false;
	}
}

// Send a message to all users in the channel
void Channel::sendMessage(std::string message, std::string sender)
{
	// Iterate over the list of users in the channel
	for (std::vector<User *>::iterator it = this->users.begin(); it != this->users.end(); ++it)
	{
		// Check if the user is not the sender, if true send the message to the user's socket
		if ((*it)->getNickName() != sender)
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(), message + "\r\n");
	}
}

// Add a user to the channel
void Channel::addUser(User *user)
{
	// Add the user to the list of users in the channel
	this->users.push_back(user);
}

/*                              General FUNCTIONS                              */

// Allow a user to join a channel
void joinChannel(User *user, Channel *channel)
{
	// Check if the channel is full
	if (channel->getUsers().size() >= channel->getMusers())
	{
		// Send an error message to the user indicating that the channel is full
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 471, Channel " + channel->getCname() + " is full.\r\n");
		return;
	}

	// Check if the channel is invite-only and the user is not invited
	if (channel->getMode()['i'] == true && user->getInvited(*channel) == false)
	{
		// Send an error message to the user indicating that they are not invited to the channel
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":ASY 473, You are not invited to channel " + channel->getCname() + ".\r\n");
		return;
	}

	// Check if the channel has no users
	if (channel->getUsers().size() == 0)
	{
		// Set the user as a channel operator
		user->setChannelOp(*channel, true);
	}

	// Add the user to the channel
	channel->addUser(user);
}

// Utility function to find a string in a vector
bool findString(std::string str, std::vector<std::string> vec)
{
	// Find the iterator pointing to the string in the vector
	std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), str);

	// Check if the string is found in the vector
	if (it != vec.end())
		return true;

	// Return false if the string is not found
	return false;
}


