#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {

		struct Operator {
			Token token;
			uint32_t presedence;
			bool leftAssociativity;
			enum class Type {
				prefix,
				infix,
				postfix
			};
			Type type;
		};

		enum class NodeKinds : uint64_t {
			namespace_path = 1,
			name,
			function_head,
			function_head_args_definition,
			type,
			square_bracket,
			bracket,
			curly_bracket,
			prefix_operator,
			infix_operator,
			suffix_operator,
			function_call,
			array_acess,
			Object_construct,
			varible_declaration,
			for_loop,
			if_branch,
			while_loop,
			const_mod,
			mut_mod,
			expression,
			expression_list,
			array_literal,
			operator_in_construction = 0xff,
		};

		class Parser : public IParser {
		private:
			OperatorRepository& repo;
			Result<Ast::NodeIndex, ErrorT> requireName(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> requireType(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parsePrimary(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpresion(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpressionListOperator(TokenStream& head, Ast& ast, NodeKinds kind, std::string endToken, std::vector<Ast::NodeIndex>& output);
			Result<Ast::NodeIndex, ErrorT> requireFunctionHeadArgs(TokenStream& head, Ast& ast);
			std::optional<Ast::NodeIndex> parseHead(TokenStream& head, Ast& ast);
			std::optional<Ast::NodeIndex> parseBody(TokenStream& body, Ast& ast);
		public:
			Parser(OperatorRepository& repo);
			virtual Ast parse(TokenStream& head, TokenStream& body);
			virtual std::string NodeKind_toString(uint64_t n) const;
			virtual ~Parser();
		};
	}
}
