#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {

		enum class NodeKinds : uint64_t {
			namespace_path = 1,
			name,
			function_head,
			function_head_args_definition,
			type,
			square_bracket,
			bracket,
			curly_bracket
		};

		class Parser : public IParser {
		public:
			virtual Ast parse(TokenStream& head, TokenStream& body);
			virtual std::string NodeKind_toString(uint64_t n) const;
			virtual ~Parser();
		};
	}
}
