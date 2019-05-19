#pragma once

#define INVALID_VALUE 0xFFFFFFFF
#define MATH_PI 3.1415926535897932384626433832795
#define TO_RADIANS(x) (float)(((x) * MATH_PI / 180.0f))
#define TO_DEGREES(x) (float)(((x) * 180.0f / MATH_PI))

#include <vector>

namespace Util
{
	std::vector<std::string> split(const std::string &s, char delim);
};