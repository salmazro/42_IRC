#include "Server.hpp"

namespace irc
{
    Server *Server::serverInstance = nullptr;

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
    void Server::config()
    {
        struct sockaddr_in address;
        socklen_t addressLen = sizeof(address);
        std::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(static_cast<uint16_t>(_port));

        // Create a socket
        if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw std::runtime_error("Server: socket error");
        }

        int optval = 1;
        // Set socket options
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&optval), sizeof(optval)) < 0) {
            throw std::runtime_error("Server: socket options error");
        }

        // Set socket to non-blocking mode
        if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0) {
            throw std::runtime_error("Server: file control error");
        }

        // Bind the socket to the specified address and port
        if (bind(_sockfd, reinterpret_cast<struct sockaddr *>(&address), addressLen) < 0) {
            throw std::runtime_error("Server: bind error");
        }

        // Start listening for incoming connections
        if (listen(_sockfd, std::numeric_limits<int>::max()) < 0) {
            throw std::runtime_error("Server: listen error");
        }

        _status = ONLINE;
        std::cout << GREEN "Server: Listening on host:  " + _host + " port: " << _port << DEFAULT << std::flush << std::endl;
    }

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

    // Initialize poll file descriptor
    void Server::initPollFD(int fd)
    {
        struct pollfd pFD = {
            .fd = fd,
            .events = POLLIN | POLLOUT,
            .revents = 0
        };
        _pollFD.push_back(pFD);

        if (_pollFD.back().fd == -1) {
            throw std::runtime_error("Failed to initialize pollfd struct");
        }
    }

    // Perform polling on file descriptors
    void Server::polling()
    {
        int pollResult = poll(_pollFD.data(), _pollFD.size(), -1);
        if (pollResult < 0)
        {
            if (errno == EINTR)
            {
                handleSignal(SIGINT);
                std::cerr << RED << "POLL: poll error" << DEFAULT << std::endl;
                throw std::runtime_error(" ");
            }
        }
    }

    // Search for incoming connections
    void Server::searchingForConnections()
    {
        for (size_t i = 0; i < _pollFD.size(); i++)
        {
            if (_pollFD[i].revents & POLLOUT && _pollFD[i].fd != _sockfd)
            {
                User* user = nullptr;
                for (std::vector<User*>::iterator it = getUser().begin(); it != getUser().end(); ++it) {
                    if ((*it)->getUserFd() == _pollFD[i].fd) {
                        user = *it;
                        break;
                    }
                }
                if (user != nullptr) {
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

    //Handle a new client connection
    void Server::handleNewConnection()
    {
        int fd = -1;
        socklen_t addressLen = sizeof(struct sockaddr_storage);
        struct sockaddr_storage remoteAddress;
        std::memset(&remoteAddress, 0, sizeof(remoteAddress));

        // Accept the incoming connection
        if ((fd = accept(_sockfd, (struct sockaddr *)&remoteAddress, &addressLen)) < 0) {
            throw std::runtime_error("Server: client connection error");
        }

        // Create a new user for the connection
        createNewUser(fd);

        // Initialize poll file descriptor for the new user
        initPollFD(fd);

        // Print information about the new connection
        printNewConnectionInfo(remoteAddress, fd);
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

    // Send a message to a client
    void Server::sendMsg(int fd, std::string msg)
    {
        if (send(fd, msg.c_str(), msg.length(), 0) < 0){
            std::cout << "Send error" << std::endl;
        }
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

    // Scan incoming messages for a specific command
    std::string Server::scanMsg(User *users, std::string str)
    {
        for (std::vector<std::string>::const_iterator it = users->_incomingMsgs.begin(); it != users->_incomingMsgs.end(); ++it)
        {
            if(*it == str)
                if(++it != users->_incomingMsgs.end())
                    return(*it);
        }
        return "";
    }

    // Authenticate a user based on password and nickname
    bool Server::authenticate_user(int index)
    {
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

    // Extract a substring from a message based on a specific pattern
    std::string Server::ExtractFromMessage(const std::string& message, const std::string &to_find) {
        size_t lineStart = message.find(to_find);

        if (lineStart != std::string::npos) {
            size_t toFindStart = lineStart + std::string(to_find).length();

            size_t passwordEnd = message.find_first_of("\r\n", toFindStart);

            std::string password = message.substr(toFindStart, passwordEnd - toFindStart);

            return password;
        }
        return "";
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
					std::string msg = ":irc 001 " + _users[index - 1]->getNickName() + " :Welcome to the perfect Chat system " + _users[index - 1]->getNickName() + "\n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
					msg = ":irc 002 " + _users[index - 1]->getNickName() + " :Host are Omar, Chinedu and Yonatan\n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
					msg = ":irc 003 " + _users[index - 1]->getNickName() + " :Created on july->2023\n";
					this->sendMsg(_users[index - 1]->getUserFd(), msg);
					msg = ":irc 004 " + _users[index - 1]->getNickName()  + " :Enjoy your stay!\n";
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
		std::cout << "-----------------------------------------------------------------------------\n";
		std::cout << "\t\t\tUsers List" << std::endl;
		while (x < _users.size())
		{
			std::cout << "User_Nick:\t" << _users.at(x)->getNickName() << std::endl;
			x++;
		}
		std::cout << "-----------------------------------------------------------------------------\n";
	}
	void Server::displayChannels()
	{
		size_t x = 0;
		std::cout << "-----------------------------------------------------------------------------\n";
		std::cout << "\t\t\t Channels List" << std::endl;
		while (x < this->_channels.size())
		{
			std::cout << "channel_name:\t" << _channels.at(x)->getCname() << std::endl;
			x++;
		}
		std::cout << "-----------------------------------------------------------------------------\n";
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

	void Server::closeSocketAndRemoveUser(size_t index)
	{
		if(index  == 0)
			std::cerr << "recv error" << std::endl;
		close(_pollFD[index].fd);
		_pollFD.erase(_pollFD.begin() + index);
		removeUser_fromChannels(_users[index - 1]);
		removeUser(_users[index - 1]->getUserFd());
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

	void Server::bye()
	{
		_running = false;

		// Close socket connections
		for (size_t i = 0; i < _pollFD.size(); i++)
		{
			if (_pollFD[i].fd != -1)
			{
				close(_pollFD[i].fd);
				_pollFD[i].fd = -1;
			}
		}

		std::vector<User *>::iterator it = _users.begin();
		for (;it != _users.end(); it++) {
			delete *it;
		}
		std::vector<Channel *>::iterator it_channel = _channels.begin();
		for (;it_channel != _channels.end(); it_channel++) {
			delete *it_channel;
		}
		_users.clear();
		_channels.clear();
		_pollFD.clear();

		// Free socket file descriptor
		if (_sockfd != -1) {
			close(_sockfd);
			_sockfd = -1;
		}
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

