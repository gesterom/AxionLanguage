#include "TokenStream.h"

#include "TODO.h"
#include <format>

TokenStream::TokenStream(std::vector<Token> vec, PreambleRepository& repo) : repo(repo), internal(vec)
{
	ASSERT(vec.size() != 0,"Fuck anyone");
}

bool TokenStream::is_good() const noexcept
{
	if (pointer >= internal.size()) return false;
	if (pointer < 0) return false;
	return true;
}

std::optional<Token> TokenStream::peak(int64_t n) const noexcept
{
	if (n + pointer >= internal.size()) return std::nullopt;
	if (n + pointer < 0) return std::nullopt;
	return internal.at(pointer + n);
}

bool TokenStream::consume(int64_t n)
{
	pointer = std::min(pointer + n, internal.size());
	return pointer>= internal.size();
}

std::optional<ErrorT> TokenStream::require(Token::Type kind)
{
	auto token = this->peak();
	if (not token.has_value()) return ErrorT{ internal.end()->value,"Internal Stream ended","Internal Stream to early" };
	if (token.value().kind == kind) { pointer++; return std::nullopt; }
	return ErrorT{
		token.value().value,
		std::format("Expected : {} get : {}",repo.to_string(kind),repo.to_string(token.value().kind)),
		"TODO Long errors"
	};
}

std::optional<ErrorT> TokenStream::require(Token::Type kind, std::string val)
{
	auto token = this->peak();
	if (not token.has_value()) return ErrorT{ internal.back().value, "Internal Stream ended","Internal Stream to early"};
	if (token.value().kind == kind and token.value().value == val) { pointer++; return std::nullopt; }
	return ErrorT{ token.value().value,
		std::format("Expected : {} get : {}", repo.to_string(kind), repo.to_string(token.value().kind)),
		"TODO Long errors"
	};
}

std::optional<ErrorT> TokenStream::requireEmpty()
{
	auto token = this->peak();
	if(not token.has_value()) return std::nullopt;
	return ErrorT{ token.value().value,
		std::format("Expected end of Stream get : {} {}",repo.to_string(token->kind),token->value.val() ),
		"TODO Long errors"
	};
}

bool TokenStream::optional(Token::Type kind)
{
	auto token = this->peak();
	if (not token.has_value()) return false;
	if (token.value().kind == kind) { pointer++; return true;}
	return false;
}

bool TokenStream::optional(Token::Type kind, std::string val)
{
	auto token = this->peak();
	if (not token.has_value()) return false;
	if (token.value().kind == kind and token.value().value == val) { pointer++; return true; }
	return false;
}

bool TokenStream::check(Token::Type kind) const noexcept
{
	auto token = this->peak();
	if (not token.has_value()) return false;
	if (token.value().kind == kind) { return true; }
	return false;
}

bool TokenStream::check(Token::Type kind, std::string val) const noexcept
{
	auto token = this->peak();
	if (not token.has_value()) return false;
	if (token.value().kind == kind and token.value().value == val) { return true; }
	return false;
}