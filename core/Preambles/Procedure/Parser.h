#pragma once

#include "NodeBuilder.h"
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
			namespace_,
			namespace_path,
			name,
			type,
			function_head_args_definition,
			function_head,
			//square_bracket,
			//bracket,
			//curly_bracket,
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
		};

		class Parser : public IParser {
		private:
			OperatorRepository& repo;
			NodeBuilder& builder;
			std::map<NodeKinds, uint32_t> translator;

			void reduce_output(OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& last, int32_t new_precedence);
			int32_t deducePrecedence(OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& output, std::string representation);

			Result<Ast::NodeIndex, ErrorT> requireName(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> requireType(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parsePrimary(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpresion(TokenStream& head, Ast& ast);
			Result<Ast::NodeIndex, ErrorT> parseExpressionListOperator(TokenStream& head, Ast& ast, NodeKinds kind, std::string endToken, std::vector<Ast::NodeIndex>& output);
			Result<Ast::NodeIndex, ErrorT> requireFunctionHeadArgs(TokenStream& head, Ast& ast);
			std::optional<Ast::NodeIndex> parseHead(TokenStream& head, Ast& ast);
			std::optional<Ast::NodeIndex> parseBody(TokenStream& body, Ast& ast);
		public:
			Parser(OperatorRepository& repo, NodeBuilder& builder);
			virtual Ast parse(TokenStream& head, TokenStream& body);
			virtual std::string NodeKind_toString(uint64_t n) const;
			virtual ~Parser();
		};
	}
}
