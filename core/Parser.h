#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {
		class Parser : public IParser {
			virtual void parse(const std::vector<Token>& head, const std::vector<Token>& body);
			virtual ~Parser();
		};
	}
}