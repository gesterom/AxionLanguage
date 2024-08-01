#pragma once

#include "CodeLocation.h"
#include "MetaLexer.h"
#include "PreambleDefinition.h"
#include "Token.h"
#include <optional>

namespace Preamble {
	namespace Type {
		
		enum class TypeTokenType : int64_t {
			keyword = Token::count + 1 ,
			colon,
			semicolon,
		};

		class Lexer : public ILexer {
			int32_t preambleIndex = -2;
		public:
			Lexer();
			//virtual void reset() override;
			virtual void setPreambleIndex(int32_t x) override;
			virtual std::optional<Token> lexHead(CodeLocation& loc) override;
			virtual std::optional<Token> lexBody(CodeLocation& loc) override;
			virtual std::string to_string(Token::Type kind) const override;
			virtual ~Lexer() override;
		};
	}
}
