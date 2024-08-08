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
			postfix_operator,
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
		public:
			virtual Ast parse(TokenStream& head, TokenStream& body);
			virtual std::string NodeKind_toString(uint64_t n) const;
			virtual ~Parser();
		};
	}
}
