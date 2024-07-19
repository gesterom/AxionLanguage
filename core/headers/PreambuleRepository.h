#pragma once

#include <vector>
#include <string>	

#include "Token.h"

class PreambuleDefinition;

class PreambuleRepository {
	std::vector<PreambuleDefinition*> vec;
public:
	PreambuleRepository();
	std::vector<PreambuleDefinition*> get() const;
	PreambuleDefinition* get(int64_t index) const;
	int64_t getPeambuleIndex(std::string representation) const;
	std::string to_string(Token::PreambuleType t) const;
	std::string to_string(Token::PreambuleType pre, Token::Type t) const;
	~PreambuleRepository();
};
