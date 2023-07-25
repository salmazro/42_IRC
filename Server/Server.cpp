#include "Server.hpp"

namespace irc
{
    Server *Server::serverInstance = NULL;

    // Default constructor
    Server::Server() {}

    // Parameterized constructor
    Server::Server(const std::string &host, const int &port, const std::string &password)
        : _host(host), _password(password), _port(port), _running(false)
    {
        serverInstance = this;
        std::cout << GREEN "Server: " + _host + " port: " << _port << " password: " + _password + DEFAULT << std::flush << std::endl;
    }

    // Destructor
    Server::~Server()
    {
        bye();
        std::cout << YELLOW "Hope You Enjoyed Our IRCServer 🖥️" << DEFAULT << std::endl;
    }

    // Perform server configuration


    // Start the IRC server
    void Server::run()
    {
        if (!_status)
            throw std::runtime_error("Server: Offline, must init first");

        std::signal(SIGINT, signalHandler);
        _running = true;
        initPollFD(_sockfd);

        while (_running)
        {
            polling();
            searchingForConnections();

            if (!_running)
                break;
        }
    }


    // Search for incoming connections
    void Server::searchingForConnections()
    {
        for (size_t i = 0; i < _pollFD.size(); i++)
        {
            if (_pollFD[i].revents & POLLOUT && _pollFD[i].fd != _sockfd)
            {
                User* user = NULL;
                for (std::vector<User*>::iterator it = getUser().begin(); it != getUser().end(); ++it) {
                    if ((*it)->getUserFd() == _pollFD[i].fd) {
                        user = *it;
                        break;
                    }
                }
                if (user != NULL) {
                    if (!user->getOutgoingMsg().empty()) {
                        std::string message = user->getOutgoingMsg()[0];
                        sendMsg(_pollFD[i].fd, message);
                    }
                }
            }

            if (_pollFD[i].revents & POLLIN)
            {
                if (_pollFD[i].fd == _sockfd) {
                    handleNewConnection();
                    break;
                }
                else {
                    handleClientData(i);
                }
            }
        }
    }



    // Create a new user for a client connection
    void Server::createNewUser(int fd)
    {
        _users.push_back(new User(fd));

        // Example code for sending initial messages to the new user
        std::string message = "CAP * ACK multi-prefix\r\n";
        send(fd, message.c_str(), message.length(), 0);
    }

    // Print information about a new client connection
    void Server::printNewConnectionInfo(const struct sockaddr_storage &remoteAddress, int fd)
    {
        char remoteIP[INET6_ADDRSTRLEN];

        if (remoteAddress.ss_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)&remoteAddress;
            inet_ntop(AF_INET, &(ipv4->sin_addr), remoteIP, INET_ADDRSTRLEN);
        }
        else if (remoteAddress.ss_family == AF_INET6)
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&remoteAddress;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), remoteIP, INET6_ADDRSTRLEN);
        }
        else
        {
            std::cout << "Unknown address family" << std::endl;
            return;
        }
        std::cout << YELLOW << "pollserver: new connection from " << remoteIP << " on socket " << fd << DEFAULT << std::endl;
    }


    // Remove a user from the server
    void Server::removeUser(int fd)
    {
        for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
        {
            if ((*it)->getUserFd() == fd)
            {
                delete *it;
                _users.erase(it);
                break;
            }
        }
    }

    // Check if a nickname is duplicate
    bool Server::check_duplicate(std::string nick)
    {
        std::vector<User *>::iterator it = _users.begin();
        while (it != _users.end())
        {
            if((*it)->getNickName() == nick)
                return (true);
            it++;
        }
        return (false);
    }


    // Authenticate a user based on password and nickname
    bool Server::authenticate_user(int index)
    {
        
        if(_users[index - 1]->_incomingMsgs.size() < 6)
                throw std::runtime_error("INCORRECT INFO\n");
        std::string pass = scanMsg(_users[index - 1], "PASS");
        std::string nick_name = scanMsg(_users[index - 1], "NICK");
        if(pass == _password && check_duplicate(nick_name) == false)
        {
            std::cout << "WELCOME : " << nick_name << " To Our IRC server , Enjoy!" << std::endl;
            _users[index - 1]->setNickName(nick_name);
            _users[index - 1]->setIsAuth(true);
            return true;
        }
        else
        {
            if(pass == "" || pass != _password) {
                throw std::runtime_error("INCORRECT PASSWORD\n");
            }
            if(check_duplicate(nick_name) == true) {
                throw std::runtime_error("duplicate user\n");
            }
        }
        return false;
    }


    // Get the file descriptor of a user by nickname
    int Server::getFdByNick(std::string nick)
    {
        std::vector<User *>::iterator it = _users.begin();
        while (it != _users.end())
        {
            if((*it)->getNickName() == nick)
                return ((*it)->getUserFd());
            it++;
        }
        return -1;
    }

    // Check if the CAP command is present in the incoming messages of a user
    bool Server::findCap(int index)
    {
        for (std::vector<std::string>::const_iterator it = _users[index - 1]->_incomingMsgs.begin(); it != _users[index - 1]->_incomingMsgs.end(); ++it) {
            if(*it == "CAP")
                return true;
        }
        return false;
    }

	void Server::splitChannelInp(int index)
	{
		size_t i = 0;
		_users[index -1 ]->_channelToJoin.clear();
		_users[index -1 ]->_channelKeys.clear();
		while (i < _users[index - 1]->_incomingMsgs.size())
		{
			if(i == 1)
			{
				std::vector<std::string> splitWords = customUtils::splitStringByDelimiter(_users[index - 1]->_incomingMsgs.at(i) , ",");
				for (std::vector<std::string>::size_type i = 0; i < splitWords.size(); ++i)
				{
						_users[index -1 ]->_channelToJoin.push_back(splitWords[i]);
				}
			}
			if(i == 2)
			{
				std::vector<std::string> splitWords = customUtils::splitStringByDelimiter(_users[index - 1]->_incomingMsgs.at(i) , ",");
				for (std::vector<std::string>::size_type i = 0; i < splitWords.size(); ++i)
				{
						_users[index -1 ]->_channelKeys.push_back(splitWords[i]);
				}
			}
			i++;
		}
		i = 0;
		while (i < _users[index -1 ]->_channelToJoin.size())
		{
			std::cout << "channels: " << _users[index -1 ]->_channelToJoin.at(i) << std::endl;
			i++;
		}
		i = 0;
		while (i < _users[index -1 ]->_channelKeys.size())
		{
			std::cout << "key: " << _users[index -1 ]->_channelKeys.at(i) << std::endl;
			i++;
		}
		return ;
	}

    void Server::handleClientData(size_t index)
    {
        if (_pollFD[index].fd != _sockfd)
        {
            int bytesRead = _users[index - 1]->receive();
            if (bytesRead <= 0)
            {
				std::cerr << "recv error" << std::endl;
                closeSocketAndRemoveUser(index);
				return ;
            }
			if(_users[index - 1]->_dataBuffer == "\r\n" || _users[index - 1]->_dataBuffer == "" || _users[index - 1]->_dataBuffer == "\n")
				return ;
            if(findCap(index) == true && scanMsg(_users[index - 1], "PASS") == "" && scanMsg(_users[index - 1], "NICK") == "" )
                return ;
            if (_users[index - 1]->getIsAuth() == false)
            {
				if(_users[index - 1]->_incomingMsgs.at(0) == "JOIN" && _users[index - 1]->_incomingMsgs.size() == 2)
					return ;
             try{
                  if(authenticate_user(index))
                {
					std::string msg = ":IRC 001 " + _users[index - 1]->getNickName() + " :Welcome to the IRC Server " + _users[index - 1]->getNickName() + "\n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
                    msg = ":IRC 002 " + _users[index - 1]->getNickName() + " :This chat relay hosted by: Aalhamel, Salmazro, and Ysadeh \n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
					msg = ":IRC 003 " + _users[index - 1]->getNickName() + " :Published on 16 July \n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
					msg = ":IRC 004 " + _users[index - 1]->getNickName() + " :Enjoy Chatting!! \n";

                }
                else
                {
					std::cout << "Not authenticated\n";
					closeSocketAndRemoveUser(index);
                }
             }
             catch(std::exception & e)
             {
				std::cout << _users[index - 1]->getNickName() + " Not Authenticated\n";
                this->sendMsg(_users[index - 1]->getUserFd(), "Error : " + std::string(e.what()));
				std::cout << RED << "Error : " + std::string(e.what()) << std::endl << DEFAULT;
				closeSocketAndRemoveUser(index);
             }
            }
            else if(_users[index - 1]->getIsAuth() == true)
            {
				if(_users[index -1 ]->_incomingMsgs.at(0) == "JOIN")
				{
					splitChannelInp(index);
				}
				if(_users[index - 1]->_incomingMsgs.at(0) == "QUIT")
				{
					closeSocketAndRemoveUser(index);
					return ;
				}
				executeCommand(_users[index - 1]->getMessages(), _users[index-1]);

				displayUsers();
				displayChannels();
            }
        }
    }


void Server::displayUsers()
{
    size_t x = 0;
    std::cout << "\n";
    std::cout << "-------------**********----------**********----------**********-------------\n";
    std::cout << std::setw(25) << std::left << "User Nicknames:" << std::endl;
    while (x < _users.size())
    {
        std::cout << std::setw(25) << std::left << _users.at(x)->getNickName() << std::endl;
        x++;
    }
    std::cout << "-------------**********----------**********----------**********-------------\n";
    std::cout << "\n";
}

void Server::displayChannels()
{
    size_t x = 0;
    std::cout << "-------------**********----------**********----------**********-------------\n";
    std::cout << std::setw(25) << std::left << "Channel Names:" << std::endl;
    while (x < this->_channels.size())
    {
        std::cout << std::setw(25) << std::left << _channels.at(x)->getCname() << std::endl;
        x++;
    }
    std::cout << "-------------**********----------**********----------**********-------------\n";
    std::cout << "\n";
}

	void Server::removeUser_fromChannels(User *user)
	{
		size_t x = 0;
		size_t y = 0;
		while (y < _channels.size())
		{
			x = 0;
			while (x < _channels.at(y)->users.size())
			{
				if (_channels.at(y)->users.at(x) == user)
				{
					_channels.at(y)->users.erase(_channels.at(y)->users.begin() + x);
					break ;
				}
				x++;
			}
			y++;
		}
	}

	std::vector<User *> &Server::getUser(void)
	{
		return _users;
	}

	void Server::closeClientSocket(size_t index)
	{
		close(_pollFD[index].fd);
		_pollFD.erase(_pollFD.begin() + index);
	}


	void Server::handleSignal(int signal)
	{
		std::cout << "Received signal: " << signal << std::endl;
		_running = false;
	}

	void Server::signalHandler(int signal)
	{
		if (serverInstance != NULL) {
			serverInstance->handleSignal(signal);
		}
	}

	bool Server::verifyPassword(std::string userPassword)
	{
		if (userPassword == _password)
			return true;
		return false;
	}

	std::vector<Channel *> irc::Server::getChannels() {
		return this->_channels;
	}

} // namespace irc

