#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <sstream>
# include <limits>
# include <exception>
# include <cstdlib>
# include <queue>
# include <ctime>
# include <algorithm>
# include <iostream>
# include <cstdio>
# include <deque>
# include <list>
# include <sys/socket.h>
# include <netinet/in.h>
#include "Utils.hpp"
#include "../Channels/Channel.hpp"
# include <deque>

class Channel;
class User
{
private:
    int user_fd;
    std::string real_name;
    std::string nick_name;
    std::string user_name;
    bool        is_auth;

	std::map<Channel, bool> invited_channels;
	std::map<Channel, bool> channel_op;
public:
    std::string _dataBuffer;
    std::vector<std::string> _incomingMsgs;
    std::vector<std::string> _outgoingMsgs;
    std::vector<std::string> _channelToJoin;
    std::vector<std::string> _channelKeys;

	std::deque<std::string> messages;

public:
    User( void );
    User(int fd);
    std::string getRealName( void );
    std::string getNickName( void );
    std::string getUserName( void );
    int  getUserFd( void );
    bool getIsAuth(void);
    void setIsAuth( bool auth );

	void setInvited(Channel channel_name, bool invited);
	bool getInvited(Channel channel_name);
	bool operator==(User const &rhs) const;
	std::deque<std::string> getMessagesDeque();
    void setNickName( std::string nick );
    void setUserName( std::string user );
    void setRealName( std::string real );

    size_t receive();
    std::vector<std::string> & getMessages();
    std::vector<std::string> & getOutgoingMsg();
    void setOutgoingMsg( std::string msg );
    void printOutgoingMsgs();
    void printIncomingMsgs();
	void addMessage(std::string message);
	void setChannelOp(Channel channel_name, bool op);
	bool is_op(Channel channel_name);
    ~User( void );
};



#endif