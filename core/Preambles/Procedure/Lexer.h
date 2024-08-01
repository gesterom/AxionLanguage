#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {

		enum class ProcedureTokenType : int64_t {
			keyword = Token::count+1,
			comma,
			colon,
			semicolon,
			integer_literal,
			double_literal,
			operator_t,
		};
		
		class Lexer : public ILexer {
			int32_t preambleIndex = -2;
			int64_t paramCount = 0;
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