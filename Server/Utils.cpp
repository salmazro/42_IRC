#include "Utils.hpp"

namespace customUtils {

	// Check for whitespace in the string
#include <string>
#include <cctype>
#include <stdexcept>

std::string checkWhitespace(const std::string& pass) {
	if (pass.empty()) {
		throw std::invalid_argument("Password cannot be empty."); // Throw an exception if the password is empty
	}

	bool hasWhitespace = false;
	for (std::size_t i = 0; i < pass.length(); ++i) {
		if (std::isspace(static_cast<unsigned char>(pass[i]))) {
			hasWhitespace = true;
			break;
		}
	}

	if (hasWhitespace) {
		throw std::invalid_argument("Password cannot contain whitespace."); // Throw an exception if the password contains whitespace
	}

    return pass; // Return the password if it passes the checks
}


	// Convert string to integer
	int convertToInt(const std::string& val) {
		std::istringstream iss(val); // Create a string stream from the input string
		int num;
		if (!(iss >> num) || !iss.eof()) // Try to extract an integer from the string stream
			throw std::invalid_argument("Invalid int"); // Throw an exception if the conversion fails
		return num; // Return the converted integer
	}

	// Split string by delimiter
	std::vector<std::string> splitStringByDelimiter(const std::string& val, const std::string& delim) {
		std::vector<std::string> ret; // Create a vector to store the split words
		size_t startPos = std::string::npos; // Initialize the starting position of the search
		size_t endPos = 0; // Initialize the ending position of the search

		while ((endPos = val.find(delim, startPos + 1)) != std::string::npos) { // Find the next occurrence of the delimiter
			std::string word = val.substr(startPos + 1, endPos - startPos - 1); // Extract the word between the delimiters
			ret.push_back(word); // Add the word to the vector
			startPos = endPos; // Update the starting position for the next search
		}

		// Add the remaining part of the string if any
		std::string lastWord = val.substr(startPos + 1); // Extract the last word after the last delimiter
		if (!lastWord.empty())
			ret.push_back(lastWord); // Add the last word to the vector

		return ret; // Return the vector of split words
	}

	// Split string by space
	std::vector<std::string> splitStringBySpace(const std::string& val) {
		std::vector<std::string> words; // Create a vector to store the split words
		std::istringstream iss(val); // Create a string stream from the input string
		std::string word;

		while (iss >> word) { // Extract each word separated by space
			words.push_back(word); // Add the word to the vector
		}

		return words; // Return the vector of split words
	}

}   // namespace customUtils

