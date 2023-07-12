#include "Server/Server.hpp"
#include "Server/Utils.hpp"

int main(int argc, char* argv[])
{
	irc::Server* server = nullptr; // Pointer to an instance of the IRC server

	try
	{
		if (argc != 3)
		{
			throw std::runtime_error("Usage: ./ircserv <port> <password>"); // Throw an exception for incorrect command-line arguments
		}

		int port = customUtils::convertToInt(argv[1]); // Get the port number from the command-line argument
		std::string password = customUtils::checkWhitespace(argv[2]); // Get the server password from the command-line argument

		if (port < 0 || port > 65535) // Check if the port number is within the valid range
		{
			throw std::out_of_range("Port Range is 0-65535"); // Throw an exception for an invalid port number
		}

		// Create an instance of the IRC server with the provided IP address, port, and password
		server = new irc::Server("0.0.0.0", port, password);
		irc::Server::serverInstance = server; // Set the server instance as the global instance

		// Register signal handlers for graceful shutdown on SIGINT and SIGTERM
		std::signal(SIGINT, &irc::Server::signalHandler);
		std::signal(SIGTERM, &irc::Server::signalHandler);

		server->config(); // Perform server configuration
		server->run(); // Start the IRC server
	}
	catch (const std::exception& e)
	{
		std::cout << RED << e.what() << DEFAULT << std::endl; // Print the error message in red color
		delete server; // Clean up the server instance
		return EXIT_FAILURE; // Return failure status
	}
	catch (...)
	{
		std::cout << "Fatal Error: An unhandled exception occurred." << std::endl; // Print a generic error message for unhandled exceptions
		delete server; // Clean up the server instance
		return EXIT_FAILURE; // Return failure status
	}

	delete server; // Clean up the server instance
	return EXIT_SUCCESS; // Return success status
}

