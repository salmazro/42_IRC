# IRC Server

This README.md document provides an overview of the functions in the code and explains their purpose and the need for each of these functions to get the server working. Diagrams are used where necessary to illustrate the flow of the program.

## Table of Contents

- [Main Function](#main-function)
- [irc::Server Class](#ircserver-class)
  - [Constructor](#constructor)
  - [Destructor](#destructor)
  - [Config](#config)
  - [Run](#run)
  - [Initialize PollFD](#initialize-pollfd)
  - [Polling](#polling)
  - [Searching for Connections](#searching-for-connections)
  - [Handle New Connection](#handle-new-connection)
  - [Create New User](#create-new-user)
  - [Print New Connection Info](#print-new-connection-info)
  - [Send Message](#send-message)
  - [Remove User](#remove-user)
  - [Check Duplicate](#check-duplicate)
  - [Scan Message](#scan-message)
  - [Authenticate User](#authenticate-user)
  - [Extract From Message](#extract-from-message)
  - [Get Fd By Nick](#get-fd-by-nick)
  - [Find Cap](#find-cap)
  - [Split Channel Input](#split-channel-input)
  - [Handle Client Data](#handle-client-data)

## Main Function

The `main` function is the entry point of the program. It initializes an instance of the `irc::Server` class, parses command-line arguments to obtain the port and password values, and then proceeds to configure and run the server.

The main function performs the following steps:
1. Checks the number of command-line arguments and throws an error if the required arguments are not provided.
2. Parses the port and password from the command-line arguments.
3. Creates an instance of the `irc::Server` class and sets it as the server instance.
4. Sets up signal handlers for interrupt and termination signals.
5. Calls the `config` function to configure the server.
6. Calls the `run` function to start the server.

## IRC Server Port Number
The maximum port number that can be used for an IRC server is `65,535`. Port numbers range from `0 to 65,535`, and ports below `1024` are reserved for well-known services and protocols.

In general, IRC servers commonly use port number `6667` for unencrypted connections and port number `6697` for encrypted connections using SSL/TLS. These ports are widely supported by IRC clients and servers.

## Understanding IP Addresses for IRC Servers: 0.0.0.0 and 127.0.0.1

The IP address 0.0.0.0 is used to indicate that a server should bind to all available network interfaces, allowing it to listen for incoming connections from both local and external networks. On the other hand, the IP address 127.0.0.1 (localhost) refers to the local machine itself and is used for communication between network programs within the same machine. If an IRC server is set to bind to 0.0.0.0, it can accept connections from both the local machine and external networks. However, if it is set to bind to 127.0.0.1, it will only accept connections from the local machine and will not be accessible from external network connections.

## irc::Server Class

The `irc::Server` class represents the IRC server and encapsulates the functionality required to handle connections, process client data, and manage users.

### Constructor

The constructor initializes the `irc::Server` object with the specified host, port, and password. It also sets the server instance, prints the server information, and initializes the server status.

### Destructor

The destructor performs cleanup tasks before destroying the `irc::Server` object. It calls the `bye` function to perform server shutdown tasks and prints a farewell message.

### Config

The `config` function is responsible for configuring the server. It performs the following steps:
1. Creates a socket using the `socket` system call.
2. Sets socket options using the `setsockopt` system call to enable reuse of the address.
3. Sets the socket to non-blocking mode using the `fcntl` system call.
4. Binds the socket to the specified address and port using the `bind` system call.
5. Starts listening for incoming connections using the `listen` system call.

The function prints the server information and sets the server status to online.

### Run

The `run` function is the main loop of the server. It performs the following steps:
1. Checks if the server is online. If not, it throws an error.
2. Sets the running flag to true and initializes the poll file descriptors using the `initPollFD` function.
3. Enters a loop that continuously polls for events using the polling function and handles connections, client data, and disconnections.
4. If the running flag is false, the loop is exited.
5. If a new connection is found, the handleNewConnection function is called to handle it.
6. If a client has data available to read, the handleClientData function is called to process the data.
7. If a client has disconnected, the removeUser function is called to remove the user from the server.
8. After handling all events, the destructor is called to perform cleanup tasks.

#### Initialize PollFD

The `initializePollFD` function is responsible for initializing the `pollfd` array with the server socket and setting the necessary flags for polling. It sets up the polling file descriptor (`pollfd`) for the server socket, so that the server can monitor events on the socket.

The function performs the following steps:
1. Clears the `pollfd` array.
2. Sets the `fd` field of the first element in the array to the server socket file descriptor.
3. Sets the `events` field of the first element to `POLLIN` to monitor read events.
4. Sets the `revents` field of the first element to 0.

#### Polling

The `polling` function is responsible for polling for events on the server socket and client sockets. It uses the `poll` system call to monitor file descriptors for events.

The function performs the following steps:
1. Calls the `poll` system call, passing the `pollfd` array and the number of elements in the array.
2. Checks if the `poll` system call failed and throws an error if it did.
3. Checks if the server socket has an event (`POLLIN` flag set) using the `revents` field of the first element in the `pollfd` array.
4. If the server socket has an event, it means there is a new connection waiting to be accepted, so it calls the `handleNewConnection` function.
5. Iterates over the remaining elements in the `pollfd` array.
6. For each element, checks if there is an event (`POLLIN` flag set) using the `revents` field.
7. If there is an event, it means there is data available to read from the client socket, so it calls the `handleClientData` function.
8. If there are no more events to process, the function returns.

#### Searching for Connections

The `searchForConnections` function is responsible for searching for new connections by checking the `revents` field of the first element in the `pollfd` array.

The function performs the following steps:
1. Checks if the server socket has an event (`POLLIN` flag set) using the `revents` field of the first element in the `pollfd` array.
2. If there is an event, it means there is a new connection waiting to be accepted, so it returns `true`.
3. If there is no event, it means there are no new connections, so it returns `false`.

#### Handle New Connection

The `handleNewConnection` function is responsible for accepting a new connection and handling it. It creates a new client socket, accepts the connection, and adds the client socket to the `pollfd` array.

The function performs the following steps:
1. Accepts a new connection using the `accept` system call, which returns a new socket file descriptor.
2. Sets the new socket file descriptor to non-blocking mode using the `fcntl` system call.
3. Adds the new socket file descriptor to the `pollfd` array by finding an empty slot in the array.
4. Sets the `fd` field of the new element to the new socket file descriptor.
5. Sets the `events` field of the new element to `POLLIN` to monitor read events.
6. Sets the `revents` field of the new element to 0.
7. Calls the `createNewUser` function to create a new user associated with the client socket.
8. Calls the `printNewConnectionInfo` function to print information about the new connection.

#### Create New User

The `createNewUser` function is responsible for creating a new user object and adding it to

 the server's user list. It initializes the user object with the client socket and assigns a unique nickname to the user.

The function performs the following steps:
1. Creates a new user object.
2. Sets the user's client socket to the provided client socket.
3. Generates a unique nickname for the user using the `getUniqueNickname` function.
4. Assigns the generated nickname to the user.
5. Adds the user to the server's user list.

#### Print New Connection Info

The `printNewConnectionInfo` function is responsible for printing information about a new connection. It displays the client's IP address and the assigned nickname.

The function performs the following steps:
1. Retrieves the client's IP address using the `getpeername` system call and the client socket.
2. Retrieves the nickname of the user associated with the client socket.
3. Prints the IP address and nickname.

#### Send Message

The `sendMessage` function is responsible for sending a message to a specific user or channel. It takes a destination, which can be either a user nickname or a channel name, and the message to be sent.

The function performs the following steps:
1. Checks if the destination is a channel or a user by calling the `isChannel` function.
2. If the destination is a channel, it retrieves the users in the channel using the `getUsersInChannel` function.
3. Iterates over the users in the channel and sends the message to each user individually.
4. If the destination is a user, it retrieves the user's client socket using the `getFdByNick` function.
5. Sends the message to the user's client socket.

#### Remove User

The `removeUser` function is responsible for removing a user from the server. It closes the user's client socket, removes the user from the server's user list, and sends a disconnection message to other users.

The function performs the following steps:
1. Retrieves the user's client socket using the `getFdByNick` function.
2. Closes the client socket using the `close` system call.
3. Retrieves the nickname of the user.
4. Removes the user from the server's user list.
5. Sends a disconnection message to other users using the `sendMessage` function.

#### Check Duplicate

The `checkDuplicate` function is responsible for checking if a nickname is already taken by another user. It iterates over the server's user list and compares each user's nickname with the provided nickname.

The function performs the following steps:
1. Iterates over the server's user list.
2. Compares the nickname of each user with the provided nickname.
3. If a user with the same nickname is found, it returns `true`.
4. If no user with the same nickname is found, it returns `false`.

#### Scan Message

The `scanMessage` function is responsible for scanning a message for commands and extracting the command and its arguments.

The function performs the following steps:
1. Creates a regular expression pattern to match the command and its arguments.
2. Uses the `std::regex_search` function to search for the pattern in the message.
3. Retrieves the command and its arguments from the captured groups in the regular expression match.
4. Returns the command and its arguments as a `std::pair` of strings.

#### Authenticate User

The `authenticateUser` function is responsible for authenticating a user by checking if the provided password matches the server's password.

The function performs the following steps:
1. Compares the provided password with the server's password.
2. If the passwords match, it returns `true`.
3. If the passwords do not match, it returns `false`.

#### Extract From Message

The `extractFromMessage` function is responsible

 for extracting a substring from a message, given a starting delimiter and an ending delimiter.

The function performs the following steps:
1. Finds the position of the starting delimiter in the message using the `std::string::find` function.
2. Finds the position of the ending delimiter in the message, starting from the position of the starting delimiter.
3. Retrieves the substring between the starting delimiter and the ending delimiter using the `std::string::substr` function.
4. Returns the extracted substring.

#### Get Fd By Nick

The `getFdByNick` function is responsible for retrieving the client socket file descriptor associated with a user's nickname.

The function performs the following steps:
1. Iterates over the server's user list.
2. Compares the nickname of each user with the provided nickname.
3. If a user with the matching nickname is found, it returns the client socket file descriptor associated with that user.
4. If no user with the matching nickname is found, it returns -1.

#### Find Cap

The `findCap` function is responsible for finding the index of a specific character in a string.

The function performs the following steps:
1. Uses the `std::string::find` function to find the index of the character in the string.
2. Returns the index of the character, or -1 if the character is not found.

#### Split Channel Input

The `splitChannelInput` function is responsible for splitting a channel input into the channel name and the message.

The function performs the following steps:
1. Uses the `findCap` function to find the index of the first space character in the channel input.
2. Retrieves the channel name by extracting the substring from the start of the channel input to the position of the space character.
3. Retrieves the message by extracting the substring from the position after the space character to the end of the channel input.
4. Returns the channel name and the message as a `std::pair` of strings.

#### Handle Client Data

The `handleClientData` function is responsible for handling client data received from a client socket. It reads the data from the client socket, scans it for commands, and executes the appropriate actions.

The function performs the following steps:
1. Retrieves the user's client socket from the `pollfd` array using the index of the element.
2. Reads the data from the client socket using the `recv` system call.
3. Checks if the `recv` system call failed or if the number of received bytes is 0. If either condition is true, it means the client has disconnected, so it calls the `removeUser` function to handle the disconnection.
4. If data was received, it converts the received data to a `std::string`.
5. Calls the `scanMessage` function to scan the received message for commands.
6. Checks the command extracted from the message and executes the appropriate action. The supported commands include authentication, joining a channel, sending messages, and quitting.
7. If the command is not recognized, it sends an error message to the user.

After handling the client data, the `handleClientData` function returns.