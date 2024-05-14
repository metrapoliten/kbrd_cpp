#pragma once

#include <stdexcept>

class ChipException : public std::runtime_error
{
public:
	explicit ChipException(const char* e)
		: runtime_error(e)
	{
	}
};