#pragma once

#include "SyntaxRepository.h"
#include "Result.h"	
#include "TODO.h"
#include "Token.h"


class TokenStream {
	std::vector<Token> internal;
	uint64_t pointer = 0;
	SyntaxRepository& repo;
	uint32_t preambleIndex = 0;
public:
	TokenStream(PreamleIndex index, std::vector<Token> vec, SyntaxRepository& repo);
	bool is_good() const noexcept;
	std::optional<Token> peak(int64_t n = 0) const noexcept;
	bool consume(int64_t n = 1);
	Result<Token, ErrorT> require(Token::Type kind);
	Result<Token, ErrorT> require(Token::Type kind, std::string val);
	std::optional<ErrorT> requireEmpty();
	std::optional<Token> optional(Token::Type kind);
	std::optional<Token> optional(Token::Type kind, std::string val);
	bool check(Token::Type kind) const noexcept;
	bool check(Token::Type kind, std::string val) const noexcept;
};