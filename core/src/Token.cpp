#include "Token.h"

bool operator<(const Token& a, const Token& b) {
	return a.value < b.value;
}