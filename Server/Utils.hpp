#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <limits>
#include <exception>
#include <cstdlib>
#include <queue>
#include <ctime>
#include <algorithm>

#include <iostream>
#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace customUtils {

	// Converts a string to an integer
	int convertToInt(const std::string& pass);

	// Checks if the string contains whitespace
	std::string checkWhitespace(const std::string& val);

	// Splits a string by a specified delimiter and returns the parts as a vector of strings
	std::vector<std::string> splitStringByDelimiter(const std::string& val, const std::string& delim);

	// Splits a string by whitespace and returns the parts as a vector of strings
	std::vector<std::string> splitStringBySpace(const std::string& val);

	// Converts a value to a string
	template <typename T>
	std::string convertToString(const T arg) {
		std::stringstream str;
		str << arg;
		return str.str();
	};

}

#endif
