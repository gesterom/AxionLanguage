#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"
#include "Preambles/Procedure/OperatorDefinition.h"

// operatotrs on steatements for examle if expresion statement else statement 
// 
// statement := `if` bool_expresion statement [`else` statement]
// statement := `while` bool_expresion statement
// statement := `for` iterator_expresion statement
// statement := `do` statement `while` bool_expresion `;`
// statement := expresion `;`
// statement := `{` [statement..] `}`
// statement := `twice` statement
// 
// {} = blok kodu = statement with list of statements
// expresion = literal,operation, atom.atom is a operation on atoms
// expresion := expresion infix_op expresion
// expresion := prefix_op expresion
// expresion := expresion sufix_op
// expresion := `ifx` bool_expresion `then` expresion `else` expresion
// expresion := `send` message_expresion `to` agent_expresion
// expresion := `goto` label_atom
//
// require = if present consume otherwise panic
// optional = bool result and consume
// check = bool result and do not consume 
//


// foo(x : Int)->int

using namespace Preamble::Procedure;

void printError(const std::optional<ErrorT>& err) {
	if (err.has_value()) {
		std::cout << "ERROR : " << err->loc.start() << " : " << err->loc.val() << " = " << err.value().oneLinerError << std::endl;
	}
}

Ast::NodeIndex newLeaf(Ast& self, Result<Token,ErrorT> t)
{
	if (t) {
		auto res = (uint32_t)self.leafs.size();
		self.leafs.push_back(t);
		return { 0,res };
	}
	else {
		printError((ErrorT)t);
		UNREACHABLE("ERROR");
	}
}

Ast::NodeIndex newNode(Ast& self, Result<Ast::Node,ErrorT> t)
{
	if (t) {
		auto res = (uint32_t)self.nodes.size();
		self.nodes.push_back(t);
		return { 1,res };
	}
	else {
		printError((ErrorT)t);
		UNREACHABLE("ERROR");
	}
}

void add(std::vector<Token>& vec, const Result<Token, ErrorT>& t) {
	if (t) {
		vec.emplace_back(t);
	}
	else {
		printError(t);
	}
}

#define TRY_ASIGNE(x,y) do{auto __temp__ = y;if(__temp__){x.emplace_back(__temp__);} else return (ErrorT)__temp__;}while(false)
#define TRY_ADD(x,y) do{auto __temp__ = y;if(__temp__){x.emplace_back(__temp__);} else return (ErrorT)__temp__;}while(false)

Result<Ast::NodeIndex, ErrorT> requireName(TokenStream& head, Ast& ast) {
	std::vector<Token> atoms;
	TRY_ADD(atoms, head.require(Token::Type::atom));
	while (head.optional((Token::Type)ProcedureTokenType::operator_t, ".")) {
		TRY_ADD(atoms, head.require(Token::Type::atom));
	}
	if (atoms.size() == 0) return ErrorT{ head.peak(-1).value().value,"expected id","" };
	auto last = atoms.at(atoms.size() - 1);
	Ast::Node path;
	path.kind = (uint64_t)NodeKinds::namespace_path;
	for (int i = 0; i < atoms.size() - 1; i++) {
		path.children.push_back(newLeaf(ast, atoms[i]));
	}
	auto path_id = newNode(ast, path);
	Ast::Node name;
	name.kind = (uint64_t)NodeKinds::name;
	name.children.push_back(path_id);
	name.children.push_back(newLeaf(ast, last));
	return newNode(ast, name);
}

Result<Ast::NodeIndex, ErrorT> requireType(TokenStream& head, Ast& ast) {
	Ast::Node res;
	res.kind = (uint64_t)NodeKinds::type;
	auto a = requireName(head, ast);
	if (not a) return (ErrorT)a;
	res.children.emplace_back(a);
	if (head.optional(Token::Type::parenthesis, "(")) {
		while (not head.check(Token::Type::parenthesis, ")") and head.peak().has_value()) {
			res.children.push_back(newLeaf(ast, head.peak().value()));
			head.consume();
		}
		return newNode(ast, res);
	}
	else {
		return newNode(ast, res);
	}
}

Result<Ast::NodeIndex, ErrorT> parseExpresion(TokenStream& head, Ast& ast) {

	if (auto a = head.optional(Token::Type::atom)) {

	}
	if (head.optional(Token::Type::parenthesis, "(")) {

		head.require(Token::Type::parenthesis, ")");
	}
	if (head.optional(Token::Type::parenthesis, "[")) {
		head.require(Token::Type::parenthesis, "]");
	}
	if (head.check(Token::Type::parenthesis, ")")) {}
	if (head.check(Token::Type::parenthesis, "]")) {}
	if (head.check(Token::Type::parenthesis, "}")) {}
	if (head.check(Token::Type::parenthesis, ",")) {}
	return ErrorT{ head.peak()->value,"TODO","TODO" };
}

// atom : type/*expresion that evaluated in compile time will result in Type*/
Result<Ast::NodeIndex, ErrorT> requireFunctionHeadArgs(TokenStream& head, Ast& ast) {
	Ast::Node res;
	res.kind = (uint64_t)NodeKinds::function_head_args_definition;
	if (not head.check(Token::Type::parenthesis, ")")) {
		do {
			res.children.push_back(newLeaf(ast, head.require(Token::Type::atom)));
			head.require((Token::Type)ProcedureTokenType::colon);
			res.children.push_back(parseExpresion(head, ast));
		} while (head.optional((Token::Type)ProcedureTokenType::comma));
	}
	return newNode(ast, res);
}

// try stmt -> expresion = to stmt or return from function with error

// name args returnType
std::optional<Ast::NodeIndex> parseHead(TokenStream& head, Ast& ast) {
	std::vector<ErrorT> errors;
	Ast::Node res;
	res.kind = (uint64_t)NodeKinds::function_head;
	res.children.resize(3);
	if (auto name = requireName(head, ast)) {
		res.children[0] = name; // name
	}
	else { return std::nullopt; }
	if (not head.require(Token::Type::parenthesis, "(")) return std::nullopt;
	auto args = requireFunctionHeadArgs(head, ast);
	if (not args) return std::nullopt;
	res.children[1] = args;

	if (not head.require(Token::Type::parenthesis, ")")) return std::nullopt;

	if (head.optional((Token::Type)ProcedureTokenType::operator_t, "->")) {
		auto retT = requireType(head, ast);
		if (not retT)
			res.children[2] = retT;
		else return std::nullopt;
	}
	else {
		Ast::Node retT;
		retT.kind = (uint64_t)NodeKinds::type;
		res.children[2] = newNode(ast, retT);
	}
	if (head.requireEmpty()) return std::nullopt; //TODO Better error : "unexpected tockens"
	return newNode(ast, res);
}

std::optional<Ast::NodeIndex> parseBody(TokenStream& body, Ast& ast) {
	return std::nullopt;
}

Ast Preamble::Procedure::Parser::parse(TokenStream& head, TokenStream& body)
{
	Ast res;
	res.headNode = parseHead(head, res);
	res.bodyNode = parseBody(body, res);
	return res;
}

std::string Preamble::Procedure::Parser::NodeKind_toString(uint64_t n) const
{
	switch ((Preamble::Procedure::NodeKinds)n)
	{
	case Preamble::Procedure::NodeKinds::namespace_path: return "namespace_path";
	case Preamble::Procedure::NodeKinds::name: return "name";
	case Preamble::Procedure::NodeKinds::function_head: return "function_head";
	case Preamble::Procedure::NodeKinds::function_head_args_definition: return "function_head_args_definition";
	case Preamble::Procedure::NodeKinds::type: return "type";
	}
	UNREACHABLE("Switch case not exausted");
}

Preamble::Procedure::Parser::~Parser()
{

}
