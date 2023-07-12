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

// Getter for modes
std::map<char, bool> Channel::getMode()
{
	return this->channel_mode;
}

// Setter for maxUsers
void Channel::setMusers(unsigned int maxUsers)
{
	this->max_users = maxUsers;
}

// Getter for maxUsers
unsigned int Channel::getMusers()
{
	return this->max_users;
}

// Setter for topic
void Channel::setTopic(std::string topic)
{
	this->channel_topic = topic;
}

// Getter for topic
std::string Channel::getTopic()
{
	return this->channel_topic;
}

// Add a user to the channel
void Channel::addUser(User *user)
{
	this->users.push_back(user);
}

// Destructor
Channel::~Channel()
{
	// irc::Server::serverInstance->deleteChannel(this);
}

// Getter for key
std::string Channel::getPwd()
{
	return this->channel_pwd;
}

// Setter for key
void Channel::setPwd(std::string password)
{
	this->channel_pwd = password;
}

// Join a channel
void joinChannel(User *user, Channel *channel)
{
	if (channel->getUsers().size() >= channel->getMusers())
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 471, Channel " + channel->getCname() + " is full.\r\n");
		return;
	}
	if (channel->getMode()['i'] == true && user->getInvited(*channel) == false)
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 473, You are not invited to channel " + channel->getCname() + ".\r\n");
		return;
	}
	if (channel->getUsers().size() == 0)
	{
		user->setChannelOp(*channel, true);
	}
	channel->addUser(user);
}

// Leave a channel
bool Channel::exitChannel(User *user)
{
	std::vector<User *>::iterator it = std::find(this->users.begin(), this->users.end(), user);
	if (it != this->users.end() && this->users.size() == 1)
	{
		user->setChannelOp(*this, false);
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":" + user->getNickName() + " PART " + this->getCname() + " :leaving " + "\r\n");
		this->users.erase(it);
		irc::Server::serverInstance->deleteChannel(this);
		return false;
	}
	else if (it != this->users.end())
	{
		user->setChannelOp(*this, false);
		this->users.erase(it);
		return true;
	}
	else
	{
		irc::Server::serverInstance->sendMsg(user->getUserFd(), ":COY 442, You are not in channel " + this->getCname() + ".\r\n");
		return false;
	}
}

// Utility function to find a string in a vector
bool findString(std::string str, std::vector<std::string> vec)
{
	std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), str);
	if (it != vec.end())
		return true;
	return false;
}

// Getter for name
std::string Channel::getCname()
{
	return this->channel_name;
}

// Getter for users
std::vector<User *> Channel::getUsers()
{
	return this->users;
}

// Overloaded equality operator for Channel objects
bool Channel::operator==(Channel const &rhs) const
{
	if (this->channel_name == rhs.channel_name)
		return true;
	return false;
}

// Overloaded less-than operator for Channel objects
bool Channel::operator<(Channel const &rhs) const
{
	if (this->channel_name < rhs.channel_name)
		return true;
	return false;
}

// Create a channel
Channel *irc::Server::createChannel(std::string name, std::string password)
{
	if ((name[0] != '#' && name[0] != '&') || name.length() > 100)
		return (NULL);
	if (name.find(' ') != std::string::npos || name.find(',') != std::string::npos)
		return (NULL);
	for (std::vector<Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		if ((*it)->getCname() == name)
			return (*it);
	}
	Channel *newChannel = new Channel(name);
	newChannel->setPwd(password);
	this->_channels.push_back(newChannel);
	return newChannel;
}

// Get a channel by name
Channel *irc::Server::getChannel(std::string name)
{
	for (std::vector<Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		if ((*it)->getCname() == name)
			return (*it);
	}
	return NULL;
}

// Delete a channel
void irc::Server::deleteChannel(Channel* channel)
{
    std::vector<Channel*>::iterator it = std::find(this->_channels.begin(), this->_channels.end(), channel);
    if (it != this->_channels.end())
    {
        delete *it;
        this->_channels.erase(it);
    }
    else
        return;
}

// Send a message to all users in the channel
void Channel::sendMessage(std::string message, std::string sender)
{
	for (std::vector<User *>::iterator it = this->users.begin(); it != this->users.end(); ++it)
	{
		if ((*it)->getNickName() != sender)
			irc::Server::serverInstance->sendMsg((*it)->getUserFd(), message + "\r\n");
	}
}
