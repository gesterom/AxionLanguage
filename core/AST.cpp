#include "AST.h"

#include <format>

std::ostream& operator<<(std::ostream& out, const std::optional<Ast::NodeIndex>& a)
{
	if (a.has_value()) {
		out << std::format("({}:{})", a->first, a->second);
	}
	else {
		out << std::format("(NULL)");
	}
	return out;
}
