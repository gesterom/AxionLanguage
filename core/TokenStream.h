#pragma once

#include "PreambleRepository.h"
#include "Token.h"

#include "TODO.h"

#pragma warning(push)
#pragma warning(disable : 26495)
template<typename R, typename E>
class Result {
	bool isGood;
	union Value {
		R res;
		E err;
		Value() {}
		~Value() {} // Destructor to call destructors of R and E if needed
	};
	Value val;
public:
	Result(R r) : isGood(true) {
		new(&val.res) R(r);
	}
	Result(E e) : isGood(false) {
		new(&val.err) E(e);
	}
	operator bool() const {
		return isGood;
	}

	// Implicit cast to R
	operator R() {
		ASSERT(isGood, "implicit cast to R when not good");
		return val.res;
	}

	// Implicit cast to E
	operator E() {
		ASSERT(not isGood, "implicit cast to E when good");
		return val.err;
	}
	operator const R() const {
		ASSERT(isGood, "implicit cast to R when not good");
		return val.res;
	}

	// Implicit cast to E
	operator const E() const {
		ASSERT(not isGood, "implicit cast to E when good");
		return val.err;
	}
	~Result() {
		if (isGood) {
			val.res.~R(); // Manually call the destructor of R if isGood
		}
		else {
			val.err.~E(); // Manually call the destructor of E if not isGood
		}
	}
};
#pragma warning(pop)

class TokenStream {
	std::vector<Token> internal;
	uint64_t pointer = 0;
	PreambleRepository& repo;
	uint32_t preambleIndex = 0;
public:
	TokenStream(PreamleIndex index, std::vector<Token> vec, PreambleRepository& repo);
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