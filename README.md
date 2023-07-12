# ft_irc Project

This repository contains the source code for the ft_irc project. ft_irc is an Internet Relay Chat (IRC) server implemented in C++.

## Prerequisites

Before running the ft_irc project, make sure you have the following dependencies installed:

- C++ compiler (supporting C++98)
- GNU Make
- Docker (optional, for running in a Docker container)
- Irssi (optional, for using the client)

## Compilation

To compile the project, follow these steps:

1. Clone the repository: `git clone <repository_url>`
2. Navigate to the project directory: `cd ft_irc`
3. Run the make command: `make`

## Running the Server

To run the ft_irc server, execute the following command:

```
./ircserv <port> <password>
```

Replace `<port>` with the desired port number and `<password>` with the password to be used for the server.

Example:

```
./ircserv 1000 irc
```

The server will display the following information:

```
Server: 0.0.0.0 port: 1000 password: irc
Server: Listening on host: 0.0.0.0 port: 1000
```

## Using Irssi as the Client

### Running in 42 Lab

To run Irssi as the client in the 42 lab environment, follow these steps:

1. Start Docker.
2. Open another terminal and run the following command: `make irssi`
3. Set a nickname using the command: `/set nick chinedu`
4. Connect to the server using the command: `/conn 10.12.4.10 1000 irc`

In the command `/conn 10.12.4.10 1000 irc`, replace `10.12.4.10` with your seat number (e.g., lab 2, row 4, seat 10), `1000` with the desired port number, and `irc` with the password.

### Running on a Mac Laptop

To run Irssi as the client on a Mac laptop, follow these steps:

1. Install Irssi on your Mac.
2. Open a terminal and run the command: `irssi`
3. Set a nickname using the command: `/set nick chinedu`
4. Connect to the server using the command: `/conn 10.12.4.10 1000 irc`

In the command `/conn 10.12.4.10 1000 irc`, replace `10.12.4.10` with the server IP address, `1000` with the desired port number, and `irc` with the password.

## Connecting Inside Docker Container

To connect inside a Docker container for checking for leaks, follow these steps:

1. Type the command `hostname -i` to get the hostname.
2. Compile the server by running `make` or `make re`.
3. Run the server with `valgrind` to check for leaks: `valgrind ./ircserv 1000 irc`.
4. Connect to the server in the Irssi terminal using the command `/conn <hostname> 1000 irc`, where `<hostname>` is the hostname obtained in step 1.

## Connecting Using Netcat (nc)

To connect using Netcat, follow these steps:

1. Open the Mac terminal.
2. Pass the command `nc localhost 1000` to connect to the server. The server will respond with `CAP * ACK multi-prefix`.
3. Pass the command `USER <username> NICK <nickname> PASS <password>` to authenticate and set the username, nickname, and password.
4.

 To join a channel, use the command `JOIN #<channel-name>`, replacing `<channel-name>` with the desired channel name.
5. To send a message to the channel, use the command `PRIVMSG #<channel-name> :<message>`. To send a message to a specific channel user, use the command `PRIVMSG <user> :<message>`.
6. To leave the channel, send the command `PART #<channel-name>`.
7. There are other commands like `TOPIC`, `MODES`, and `QUIT` that you can explore.

## Using Irssi

To use Irssi as the client, follow these steps:

1. To create or join a channel, type `/join <channel-name>`, replacing `<channel-name>` with the desired channel name.
2. To send a message in the channel, simply type the message and press Enter.
3. To send a private message, type `/msg <user> <message>`, replacing `<user>` with the username of the recipient and `<message>` with the desired message.

## Breakdown of the Functions

- [X] IRC Server - guide [guide 14](https://github.com/ChineduGboof/FT_IRC/blob/main/Readme/Server.md "IRC Server")
- [X] User Class Functions  - guide [guide 15](https://github.com/ChineduGboof/FT_IRC/blob/main/Readme/User.md "User Class Functions")
- [X] Channels Class Functions - guide [guide 16](https://github.com/ChineduGboof/FT_IRC/blob/main/Readme/Channels.md "Channels Function Documentation")


## Further Studies

- [Things to know about IRC](https://ircgod.com/docs/irc/to_know/)
- [FT IRServer Documentation](https://irc.dalexhd.dev/index.html)
- [IRSSI Documentation](https://irssi.org/documentation/manual/)
- [BEEJ Guide To Network Programming](https://beej.us/guide/bgnet/html/#select)

- [Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459)

