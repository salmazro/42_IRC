#include "user.hpp"

User::User( void ){}

User::~User( void ) {
    _incomingMsgs.clear();
    _outgoingMsgs.clear();
}

std::string User::getRealName( void )
{
    return (this->user_name);
}
std::string User::getNickName( void )
{
    return (this->nick_name);
}
std::string User::getUserName( void )
{
    return (this->user_name);
}
int User::getUserFd( void )
{
    return (this->user_fd);
}

bool User::getIsAuth( void )
{
    return (this->is_auth);
}
void User::setIsAuth( bool auth )
{
    this->is_auth = auth;
}

User::User(int fd)
{  
    this->user_fd = fd;
    this->nick_name = "";
    this->user_name = "";
    this->real_name = "";
    this->is_auth = false;
}

void User::setRealName( std::string name )
{
    this->real_name = name;
}

void User::setNickName( std::string nick )
{
    this->nick_name = nick;
}

void User::setUserName( std::string user )
{
    this->user_name = user;
}

// Channel changes:

void User::addMessage(std::string message)
{
	this->messages.push_back(message);
}

std::deque<std::string> User::getMessagesDeque()
{
	return this->messages;
}

bool User::operator==(User const &rhs) const
{
	if (this->user_fd == rhs.user_fd)
		return true;
	return false;
}

void	User::setChannelOp(Channel channel_name, bool op)
{
	if (this->channel_op.find(channel_name) == this->channel_op.end())
	{
		this->channel_op.insert(std::pair<Channel, bool>(channel_name, op));
		return ;
	}
	this->channel_op[channel_name] = op;
}

bool	User::is_op(Channel channel_name)
{
	return this->channel_op[channel_name];
}

void	User::setInvited(Channel channel_name, bool invited)
{
	if (this->invited_channels.find(channel_name) == this->invited_channels.end())
	{
		this->invited_channels.insert(std::pair<Channel, bool>(channel_name, invited));
		return ;
	}
	this->invited_channels[channel_name] = invited;
}

bool	User::getInvited(Channel channel_name)
{
	return this->invited_channels[channel_name];
}

/**
 * @brief Receive data from the user and process it.
 *
 * This function reads data from the user's socket and processes it. It appends the received data
 * to the internal data buffer, splits the buffer into messages using the delimiter "\r\n",
 * and stores each message in the _incomingMsgs vector after splitting it into words using the
 * delimiter " ".
 *
 * @return The number of bytes received from the user, or a value <= 0 if an error occurred or the connection is closed.
 */
size_t User::receive() {
    char buffer[1024];
    size_t bytesRead = recv(user_fd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        return bytesRead;
    }
    if(bytesRead < sizeof(buffer))
        buffer[bytesRead] = '\0';
    _dataBuffer = buffer;
    std::vector<std::string> temp = customUtils::splitStringByDelimiter(_dataBuffer, "\r\n");
    _incomingMsgs.clear();
    while (!temp.empty()) {
        std::vector<std::string> splitWords = customUtils::splitStringBySpace(temp.front());
        for (std::vector<std::string>::size_type i = 0; i < splitWords.size(); ++i) {
            _incomingMsgs.push_back(splitWords[i]);
        }
        temp.erase(temp.begin(), temp.begin() + 1);
    }
    return bytesRead;
}

std::vector<std::string> & User::getMessages() {
    return _incomingMsgs;
}

std::vector<std::string> & User::getOutgoingMsg() {
    return _outgoingMsgs;
}

void User::setOutgoingMsg( std::string msg ) {
    _outgoingMsgs.push_back( msg );
}

void User::printOutgoingMsgs() {
    for (std::vector<std::string>::const_iterator it = _outgoingMsgs.begin(); it != _outgoingMsgs.end(); ++it) {
        std::cout << *it << std::endl;
    }
}

void User::printIncomingMsgs() {
    for (std::vector<std::string>::const_iterator it = _incomingMsgs.begin(); it != _incomingMsgs.end(); ++it) {
        std::cout << "Incoming Message => " << *it << std::endl;
    }
}
