#pragma once

#include "Token.h"
#include "PreambleRepository.h"

class TokenStream {
	std::vector<Token> internal;
	uint64_t pointer = 0;
	PreambleRepository& repo;
public:
	TokenStream(std::vector<Token> vec, PreambleRepository& repo);
	bool is_good() const noexcept;
	std::optional<Token> peak(int64_t n = 0) const noexcept;
	bool consume(int64_t n = 1);
	std::optional<ErrorT> require(Token::Type kind);
	std::optional<ErrorT> require(Token::Type kind, std::string val);
	std::optional<ErrorT> requireEmpty();
	bool optional(Token::Type kind);
	bool optional(Token::Type kind, std::string val);
	bool check(Token::Type kind) const noexcept;
	bool check(Token::Type kind, std::string val) const noexcept;
};