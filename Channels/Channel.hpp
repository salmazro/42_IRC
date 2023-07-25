#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Server/Server.hpp"
#include "../Server/user.hpp"
#include <algorithm>
#include <map>

class User;
class Channel
{
	private:
		std::string				channel_name;		// Name of the channel
		std::map<char, bool>	channel_mode;		// Modes of the channel
		unsigned int			max_users;			// Maximum number of users allowed in the channel
		std::string				channel_topic;		// Topic of the channel
		std::string				channel_pwd;		// Password or key for accessing the channel

	public:
		std::vector<User *>		users;				// List of users in the channel
		void					numericResponse(std::string msg, std::string code, int fd, std::string opt, User *user);

		//  Constructor && Destructor
		Channel(std::string _name);
		~Channel();
		bool					operator==(Channel const &rhs) const;
		bool					operator<(Channel const &rhs) const;

		//  Channel Functions
		bool					exitChannel(User *user);								// Method to allow a user exit the channel
		void					sendMessage(std::string message, std::string sender);	// Method to send a message to all users in the channel
		void					setInvited(User *user, Channel &channel_name);			// Method to set a user as invited to the channel
		void					addUser(User *user);									// Method to add a user to the channel

		//  Setters
		void					setMusers(unsigned int maxUsers);	// Method to set the maximum number of users in the channel
		void					setTopic(std::string topic);		// Method to set the topic of the channel
		void					setPwd(std::string password);		// Method to set the key (password) of the channel

		//getters
		std::string				getCname();		// Method to get the name of the channel
		std::vector<User *>		getUsers();		// Method to get the list of users in the channel
		unsigned int			getMusers();	// Method to get the maximum number of users in the channel
		std::map<char, bool>	getMode();		// Method to get the modes of the channel
		std::string				getTopic();		// Method to get the topic of the channel
		std::string				getPwd();		// Method to get the key (password) of the channel

		// Channel Command Functions
		void					kickUser(Channel *channel, User *user, std::vector<std::string> messages);	// Method to kick a user from the channel
		void					inviteUser(User *user, std::vector<std::string> messages);					// Method to invite a user to the channel
		void					execTopic(User *user, std::vector<std::string> messages);					// Method to execute a topic change
		void					execMode(User *user, std::vector<std::string> messages);					// Method to switch modes of the channel
};

void	joinChannel(User *user, Channel *channel);						// Function to allow a user to join a channel
void	executeCommand(std::vector<std::string> messages, User *user);	// Function to execute a message
bool	findString(std::string str, std::vector<std::string> vec);		// Utility function to find a string in a vector

#endif

