#pragma once

#include <vector>
#include <string>

#define INVALID_VALUE 0xFFFFFFFF

namespace Util
{
	std::vector<std::string> split(const std::string &s, char delim);
};