#pragma once

#include <string>

class TokenRepository
{
	uint64_t addTokenType(std::string tokenName);
	std::string to_string(uint64_t kind) const;
};

