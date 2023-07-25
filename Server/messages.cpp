#include "Server.hpp"

namespace irc
{
    // Send a message to a client
    void Server::sendMsg(int fd, std::string msg)
    {
        if (send(fd, msg.c_str(), msg.length(), 0) < 0){
            std::cout << "Send error" << std::endl;
        }
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

}


