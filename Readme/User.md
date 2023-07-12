# User Class Functions

The User class provides various functions for managing user information, handling messages, and tracking user state in a server-client architecture. These functions are essential for the proper functioning of the server and ensuring a seamless user experience. Let's explore the different aspects and checks performed by these functions.

## User Initialization and Cleanup
- The `User::User( void )` constructor initializes a User object with default values, ensuring that all necessary attributes are properly initialized.
- The `User::~User( void )` destructor clears the incoming and outgoing message queues, preventing memory leaks and ensuring proper cleanup.

## User Information
- The `User::getRealName( void )`, `User::getNickName( void )`, and `User::getUserName( void )` functions retrieve the real name, nickname, and username of the user, respectively. These functions allow accessing and utilizing user information as needed.
- The `User::getUserFd( void )` function returns the file descriptor associated with the user's connection, enabling communication with the user.

## User Authentication
- The `User::getIsAuth( void )` function retrieves the authentication status of the user, indicating whether the user is authenticated or not.
- The `User::setIsAuth( bool auth )` function sets the authentication status of the user. This check ensures that only authenticated users can access certain functionalities or perform specific actions.

## Channel Management
- The `User::setChannelOp(Channel channel_name, bool op)` function sets the operator status for a channel, allowing the user to act as an operator in that channel. This check ensures proper channel management and privileges for specific users.
- The `User::is_op(Channel channel_name)` function checks if the user is an operator in a specific channel. This verification is necessary for enforcing operator-specific actions and permissions.
- The `User::setInvited(Channel channel_name, bool invited)` function sets the invitation status for a channel, indicating whether the user is invited to that channel or not. This check ensures controlled access to channels based on invitation status.
- The `User::getInvited(Channel channel_name)` function checks if the user is invited to a specific channel, allowing or restricting access as necessary.

## Message Handling
- The `User::addMessage(std::string message)` function adds a message to the user's message queue. This functionality is crucial for storing and managing messages received by the user.
- The `User::getMessagesDeque()` function returns a deque of messages in the user's message queue. It provides access to the messages for processing or displaying purposes.
- The `User::getMessages()` and `User::getOutgoingMsg()` functions return references to the incoming and outgoing message queues, respectively. These functions facilitate efficient message handling and manipulation.
- The `User::setOutgoingMsg( std::string msg )` function adds a message to the outgoing message queue, allowing the server to send messages to the user.
- The `User::printOutgoingMsgs()` and `User::printIncomingMsgs()` functions print the messages in the outgoing and incoming message queues, respectively. These functions aid in debugging and monitoring message flow.

## Message Reception and Processing
- The `User::receive()` function receives data from the user's socket and processes it. It appends the received data to the internal data buffer, splits the buffer into messages, and stores each message in the incoming message queue. This function ensures effective handling of user input and enables the server to respond appropriately.
 
These checks and functions collectively ensure proper user management, message handling, and communication within the server-client architecture. By providing mechanisms to access and modify user information, handle messages, and validate channel-related permissions, the User class functions play a crucial role in maintaining server integrity and delivering a seamless user experience.

## Short SUmmary of the User Class Functions

### User::User( void ){}

- Constructor for the User class.
- Does not take any arguments.
- Initializes the User object with default values.

### User::~User( void )

- Destructor for the User class.
- Clears the incoming and outgoing message queues.

### std::string User::getRealName( void )

- Returns the real name of the user.

### std::string User::getNickName( void )

- Returns the nickname of the user.

### std::string User::getUserName( void )

- Returns the username of the user.

### int User::getUserFd( void )

- Returns the file descriptor associated with the user's connection.

### bool User::getIsAuth( void )

- Returns the authentication status of the user (whether the user is authenticated or not).

### void User::setIsAuth( bool auth )

- Sets the authentication status of the user.

### User::User(int fd)

- Constructor for the User class.
- Takes a file descriptor as an argument and initializes the User object with the provided file descriptor.

### void User::setRealName( std::string name )

- Sets the real name of the user.

### void User::setNickName( std::string nick )

- Sets the nickname of the user.

### void User::setUserName( std::string user )

- Sets the username of the user.

### void User::addMessage(std::string message)

- Adds a message to the user's message queue.

### std::deque<std::string> User::getMessagesDeque()

- Returns a deque of messages in the user's message queue.

### bool User::operator==(User const &rhs) const

- Overloaded equality operator for the User class.
- Checks if two User objects are equal based on their file descriptors.

### void User::setChannelOp(Channel channel_name, bool op)

- Sets the operator status for a channel for the user.
- Takes a channel name and a boolean value indicating if the user is an operator in that channel.

### bool User::is_op(Channel channel_name)

- Checks if the user is an operator in a specific channel.
- Takes a channel name as an argument.

### void User::setInvited(Channel channel_name, bool invited)

- Sets the invitation status for a channel for the user.
- Takes a channel name and a boolean value indicating if the user is invited to that channel.

### bool User::getInvited(Channel channel_name)

- Checks if the user is invited to a specific channel.
- Takes a channel name as an argument.

### size_t User::receive()

- Receives data from the user's socket and processes it.
- Appends the received data to the internal data buffer, splits the buffer into messages, and stores each message in the incoming message queue.
- Returns the number of bytes received from the user.

### std::vector<std::string> & User::getMessages()

- Returns a reference to the incoming message queue.

### std::vector<std::string> & User::getOutgoingMsg()

- Returns a reference to the outgoing message queue.

### void User::setOutgoingMsg( std::string msg )

- Adds a message to the outgoing message queue.
- Takes a message as an argument.

### void User::printOutgoingMsgs()

- Prints the messages in the outgoing message queue.

### void User::printIncomingMsgs()

- Prints the messages in the incoming message queue.

---

These functions in the User class are essential for managing user information, handling messages, and tracking user state. They provide methods to retrieve and set user information such as nickname, real name, and authentication status. The functions related to channels allow managing operator and invitation statuses for different channels.

The receive function is crucial for

 receiving and processing data from the user's socket. It reads the data, splits it into messages, and stores them in the incoming message queue. This function enables the server to handle and respond to user commands and messages effectively.

The functions for managing the incoming and outgoing message queues allow storing and retrieving messages sent to and received from the user. These queues facilitate message handling and communication between the server and the user.

Overall, these functions play a vital role in managing user data, processing user input, and facilitating communication within the server-client architecture.