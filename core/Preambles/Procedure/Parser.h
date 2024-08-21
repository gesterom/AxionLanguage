#pragma once

#include "NodeBuilder.h"
#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {

		class Parser : public IParser {
		private:
			SyntaxRepository& repo;
			NodeBuilder builder;
			
			enum NodeKinds : uint64_t {
				leaf = 0,
				scope = 1,
				empty_scope_path,
				scope_path,
				name,
				type,
				function_head_args_definition,
				function_head,
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
				empty_expression,
				expression_list,
				array_literal,
				varible,
				statement, empty_stmt,
				expression_stmt,
				stmt_block
			};

			void reduce_output(const OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& last, int32_t new_precedence);
			OperatorPrecedence deducePrecedence(const OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& output, std::string representation);

			Result<Ast::NodeIndex, ErrorT> requireName(TokenStream& head, Ast& ast);
			//expression
			Result<Ast::NodeIndex, ErrorT> parsePrimary(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpresion(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseType(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpressionListOperator(TokenStream& head, Ast& ast, NodeKinds kind, std::string endToken, std::vector<Ast::NodeIndex>& output);
			Result<Ast::NodeIndex, ErrorT> requireFunctionHeadArgs(TokenStream& head, Ast& ast);
			//statement
			Result<Ast::NodeIndex, ErrorT> parseIf(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseLet(TokenStream& body, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseStatement(TokenStream& body, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseBlockStatement(TokenStream& body, Ast& ast);

			std::optional<Ast::NodeIndex> parseHead(TokenStream& head, Ast& ast);
			std::optional<Ast::NodeIndex> parseBody(TokenStream& body, Ast& ast);
		public:
			Parser(SyntaxRepository& repo);
			virtual Ast parse(TokenStream& head, TokenStream& body);
			//virtual std::string NodeKind_toString(uint64_t n) const;
			virtual ~Parser();
		};
	}
}
