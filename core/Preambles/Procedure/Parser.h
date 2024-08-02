#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {

		struct SyntaxDefinitionSteamtemnt {
			std::string constractionName;
			struct Type {
				enum {
					keyword,
					expresion,
					statement
				} kind;
				uint64_t requiredConstruction; // -1 means any
			};
			std::vector<Type> vec;
		};

		// `if` exp `:` stmt `else` stmt

		struct SyntaxExpresion {
			struct Type {
				enum {
					keyword,
					expresion,
				} kind;
				std::string keyword_val_or_expresion_type;
			};
			std::vector<Type> vec;
			std::string constractionName;
		};

		using IndexInNodeArray = uint64_t;

		struct SteatementNode {
			std::string nodeName; //nodeKind
			std::vector<IndexInNodeArray> children;
		};

		struct ExpresionNode {
			std::string nodeName; //nodeKind
			std::vector<IndexInNodeArray> children;
		};

		class Parser : public IParser {
			public:
				virtual Ast parse(TokenStream& head, TokenStream& body);
				virtual ~Parser();
		};
	}
}
