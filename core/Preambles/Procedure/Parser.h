#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {
		class Parser : public IParser {
			public:
				virtual TODODefineType parse(TokenStream& head, TokenStream& body);
				virtual ~Parser();
		};
	}
}