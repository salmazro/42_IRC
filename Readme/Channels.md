# Channels Function Documentation

## Channel Class

The `Channel` class represents a chat channel in the IRC (Internet Relay Chat) server. It provides various functions to manage channel properties, users, and operations. Let's go through each function and summarize its purpose and significance.

### Channel Constructor

The constructor initializes a new channel object with default values for its properties. It sets the name of the channel, maximum number of users, channel modes, topic, and key (password). These properties define the behavior and characteristics of the channel.

### getModes()

This function returns the current modes of the channel as a map of characters to booleans. The modes represent various settings that can be enabled or disabled for the channel, such as operator privileges (`o`), invite-only mode (`i`), topic protection (`t`), user limit (`l`), and password protection (`k`).

### setmaxUsers()

This function sets the maximum number of users allowed in the channel. It is used to control the channel's capacity and limit the number of participants.

### getmaxUsers()

This function returns the maximum number of users allowed in the channel.

### setTopic()

This function sets the topic (description) of the channel. The topic is a brief message or theme that describes the purpose or focus of the channel.

### getTopic()

This function returns the current topic of the channel.

### addUser()

This function adds a user to the channel. It is called when a user joins the channel and becomes a participant.

### Channel Destructor

The destructor is responsible for cleaning up the channel object when it is no longer needed. It is currently commented out and might be intended for future implementation.

### getKey()

This function returns the current key (password) associated with the channel. It is used to authenticate users who want to join the channel if the channel has password protection enabled.

### setKey()

This function sets the key (password) for the channel. It is used to protect the channel and allow only users who know the correct password to join.

### joinChannel()

This function is called when a user attempts to join a channel. It performs various checks to determine if the user is allowed to join and sends appropriate responses or error messages accordingly. It checks if the channel is already full, if the user is invited (if invite-only mode is enabled), and if the user is the first participant (making them a channel operator).

### partChannel()

This function is called when a user wants to leave the channel. It removes the user from the channel's user list and performs additional checks. If the user was the last participant, the channel is deleted. Otherwise, the user's channel operator status is updated.

### findString()

This utility function searches for a string within a vector of strings and returns true if found, false otherwise.

### getName()

This function returns the name of the channel.

### getUsers()

This function returns a vector of user objects representing the users currently in the channel.

### operator==()

This overloaded operator compares two channel objects for equality based on their names.

### operator<()

This overloaded operator compares two channel objects for ordering based on their names.

### irc::Server::createChannel()

This function is a member of the `irc::Server` class and is used to create a new channel with the given name and password. It performs various checks, such as name validity and duplicate channel names, before creating the channel object and adding it to the server's list of channels.

### irc::Server::getChannel()

This function is a member of the `irc::Server` class and is used to retrieve a channel object based on its name. It searches through the server's list of channels and returns the channel object if found; otherwise, it returns `NULL`.

### irc::Server::deleteChannel()

This function is a member of the irc::Server class and is used to delete a channel from the server. It takes a Channel* parameter and searches for the channel in the server's list of channels. If the channel is found, it is deleted from memory and removed from the list of channels. If the channel is not found, the function returns without performing any actions.

### Channel::sendMessage(std::string message, std::string sender)

This method is a member of the Channel class and is used to send a message to all users in the channel except the sender. It takes two parameters: message, which is the content of the message, and sender, which is the nickname of the user sending the message. The method iterates over the vector of users in the channel and sends the message to each user except the sender.

# Channel::kickUser

This function is used to kick a user from a channel. The user performing the kick must have operator status in the channel. The function takes the following parameters:

- `channel`: Pointer to the channel from which the user will be kicked.
- `user`: Pointer to the user who is performing the kick.
- `messages`: A vector of strings that contains the command and parameters for the kick.

The function checks the validity of the kick command and verifies that the user has operator status in the channel. If the conditions are met, the function proceeds to kick the specified user by sending appropriate messages to the channel and removing the user from the channel.

# Channel::switchMode

This function is used to switch the mode of a channel. The user performing the mode switch must have operator status in the channel. The function takes the following parameters:

- `user`: Pointer to the user who is performing the mode switch.
- `messages`: A vector of strings that contains the command and parameters for the mode switch.

The function checks the validity of the mode switch command and verifies that the user has operator status in the channel. If the conditions are met, the function proceeds to switch the specified mode(s) of the channel based on the provided parameters.

# Channel::execTopic

This function is used to execute the topic command for a channel. The user performing the topic command must have operator status in the channel, unless the channel mode is set to "t" (topic settable by anyone). The function takes the following parameters:

- `user`: Pointer to the user who is performing the topic command.
- `messages`: A vector of strings that contains the command and parameters for the topic command.

The function checks the validity of the topic command and verifies the user's operator status or channel mode. If the conditions are met, the function proceeds to set the topic of the channel based on the provided parameters.

# Channel::inviteUser

This function is used to invite a user to a channel. The user performing the invite must have operator status in the channel, unless the channel mode is set to "i" (invite-only). The function takes the following parameters:

- `user`: Pointer to the user who is performing the invite.
- `messages`: A vector of strings that contains the command and parameters for the invite.

The function checks the validity of the invite command and verifies the user's operator status or channel mode. If the conditions are met, the function proceeds to invite the specified user to the channel by sending an invite message.

# server_messages

This function is used to check if a message corresponds to specific server-related commands. It takes a vector of strings `messages` as a parameter, which contains the command and parameters of the message. The function checks if the message matches the "WHOIS" or "MODE FT_irc_server" commands and returns `true` if it does, indicating that the message is a server-related command.

# handle_nickname

This function is used to handle the change of a user's nickname. It takes the following parameters:

- `user`: Pointer to the user who is changing their nickname.
- `messages`: A vector of strings that contains the command and parameters for the nickname change.

The function checks if the new nickname is already in use by another user. If the nickname is available, it sets the new nickname for the user and sends a message confirming the nickname change.

# join_channel

This function handles the process of a user joining a channel. It creates a new channel if it doesn't exist, checks for channel password correctness, verifies channel limits and access restrictions, and adds the user to the channel. It sends appropriate messages to the user and existing channel members to notify them of the user's join.

#### `isCommand`

This function checks if a given message corresponds to a valid IRC command. It returns `true` if the message is a recognized command and `false` otherwise.

#### `execMessage`

This function executes the appropriate action based on the received message. It handles various IRC commands such as `JOIN`, `PART`, `PRIVMSG`, `NICK`, `MODE`, `TOPIC`, `KICK`, and `INVITE`. It delegates the execution of these commands to their respective functions in the `Channel` class.

### Class: Channel

#### `kickUser`

This function is used to kick a user from a channel. The user performing the kick must have operator status in the channel. It verifies the validity of the kick command, checks the user's operator status, and removes the specified user from the channel.

#### `switchMode`

This function is used to switch the mode of a channel. The user performing the mode switch must have operator status in the channel. It verifies the validity of the mode switch command, checks the user's operator status, and switches the specified mode(s) of the channel.

#### `execTopic`

This function is used to execute the topic command for a channel. The user performing the topic command must have operator status in the channel, unless the channel mode is set to "t" (topic settable by anyone). It verifies the validity of the topic command, checks the user's operator status or channel mode, and sets the topic of the channel.

#### `inviteUser`

This function is used to invite a user to a channel. The user performing the invite must have operator status in the channel, unless the channel mode is set to "i" (invite-only). It verifies the validity of the invite command, checks the user's operator status or channel mode, and invites the specified user to the channel.

#### `partChannel`

This function is used to handle a user leaving a channel. It removes the user from the channel and sends appropriate messages to notify the user and remaining channel members.

#### `sendMessage`

This function is used to send a message to all users in a channel except the sender. It sends the message to all channel members, excluding the user who sent the message.

### Other Functions

#### `server_messages`

This function checks if a message corresponds to specific server-related commands, such as "WHOIS" or "MODE FT_irc_server". It returns `true` if the message matches one of these commands, indicating that it is a server-related command.

#### `joinChannel`

This function adds a user to a channel by updating the necessary data structures and sending appropriate messages to the user and other channel members.

### Conclusion

These functions provide the core functionality of the IRC server implementation. They handle user actions such as changing nicknames, joining and leaving channels, sending messages, and managing channel modes. By understanding these functions, you can gain insight into the inner workings of the IRC server and its command handling logic.