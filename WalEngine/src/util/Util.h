#pragma once

#define INVALID_VALUE 0xFFFFFFFF

#include <vector>

namespace Util
{
	std::vector<std::string> split(const std::string &s, char delim);
};