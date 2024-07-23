#pragma once

#include <vector>
#include <string>	

#include "Token.h"

class PreambleDefinition;

class PreambleRepository {
	std::vector<PreambleDefinition*> vec;
public:
	PreambleRepository();
	std::vector<PreambleDefinition*> get() const;
	PreambleDefinition* get(int64_t index) const;
	int64_t getPeambuleIndex(CodeLocation representation) const;
	std::string to_string(Token::PreambleType t) const;
	std::string to_string(Token::PreambleType pre, Token::Type t) const;
	~PreambleRepository();
};
