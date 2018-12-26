#ifndef STR_UTILS
#define STR_UTILS

#include <iostream>
#include <vector>

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

void print(glm::vec3 vector, bool endline = true)
{
	std::cout << vector[0] << " " << vector[1] << " " << vector[2];
	if (endline)
		std::cout << std::endl;
}

void print(glm::vec4 vector, bool endline = true)
{
	std::cout << vector[0] << " " << vector[1] << " " << vector[2] << " " << vector[3] << std::endl;
	if (endline)
		std::cout << std::endl;
}

#endif