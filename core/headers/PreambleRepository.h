#pragma once

#include <string>	
#include <vector>
#include "Token.h"

class PreambleDefinition;

class PreambleRepository {
	std::vector<PreambleDefinition*> vec;
public:
	PreambleRepository();
	std::vector<PreambleDefinition*> get() const;
	PreambleDefinition* get(int32_t index) const;
	int32_t getPeambuleIndex(CodeLocation representation) const;
	std::string to_string(Token::Type t) const;
	std::string prambleName(Token::Type pre) const;
	~PreambleRepository();
};
