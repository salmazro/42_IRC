#include "Server.hpp"

namespace irc
{
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


    void Server::closeSocketAndRemoveUser(size_t index)
        {
            if(index  == 0)
                std::cerr << "recv error" << std::endl;
            close(_pollFD[index].fd);
            _pollFD.erase(_pollFD.begin() + index);
            removeUser_fromChannels(_users[index - 1]);
            removeUser(_users[index - 1]->getUserFd());
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
}