
#ifndef SERVER_HPP
#define SERVER_HPP

#define DEFAULT		"\033[0m"
#define RED			"\033[0;31m"
#define GREEN		"\033[0;32m"
#define YELLOW		"\033[0;33m"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>
#include <cctype>
#include <vector>
#include <deque>
#include <map>
#include <sstream>
#include <limits>
#include <string>
#include <cerrno>
#include "../Channels/Channel.hpp"
#include <algorithm>
#include "user.hpp"
#include "Utils.hpp"
#include "Responses.hpp"
#include <sys/socket.h>


class Channel;
class User;
namespace irc {

    class Server {
        private:

            std::string                 _host;
            std::string                 _password;
            std::string                 _remoteIP;
            std::vector<pollfd>         _pollFD;
            int                         _port;
            int                         _status;
            int                         _sockfd;
            bool                        _running;
            
            void    initPollFD(int fd);

            Server();
			
        public:
            Server  ( const std::string& host, const int& port, const std::string& password );
            ~Server ();

            static  Server*      serverInstance;
            void    config();
            void    run();
            void    handleNewConnection();
            void    handleClientData(size_t index);
            void    closeClientSocket(size_t index);
            void    polling();
            void    printNewConnectionInfo(const struct sockaddr_storage& remoteAddress, int fd);
            void    searchingForConnections();
            void    closeSocketAndRemoveUser(size_t index);
            void    bye();
            std::string getRemoteIP() const;

            void    handleSignal(int signal);
            static void signalHandler(int signal);
            
            //user
            std::vector<User *> _users;
            std::vector<User *>& getUser( void );
            void    createNewUser(int fd);
            void    removeUser(int fd);
            void    displayUsers();
            bool    authenticate_user(int index);

            void sendMsg(int fd, std::string msg);
            std::string ExtractFromMessage(const std::string& message, const std::string &to_find);
            bool check_duplicate(std::string nick);
            int getFdByNick(std::string nick);
            bool findCap(int index);
            void displayChannels();
            std::string scanMsg(User *users, std::string str);
            // std::string find(std::string, )

            bool verifyPassword(std::string userPassword);
			
			// Channels
			Channel *createChannel(std::string name, std::string password);
			Channel *getChannel(std::string name);
            void    deleteChannel(Channel* channel);
            void    splitChannelInp(int index);
            void	removeUser_fromChannels(User *user);
			std::vector<Channel *> _channels;
			std::vector<Channel *> getChannels();
    };
}

#endif