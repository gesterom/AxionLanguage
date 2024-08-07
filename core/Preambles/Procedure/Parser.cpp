#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"
#include "Preambles/Procedure/OperatorDefinition.h"

#include <format>

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
		std::cout << "ERROR : " << err->loc.start() << " : " << err->loc.to_string() << " = " << err.value().oneLinerError << std::endl;
	}
}

Ast::NodeIndex newLeaf(Ast& self, Result<Token, ErrorT> t)
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

Ast::NodeIndex newNode(Ast& self, Result<Ast::Node, ErrorT> t)
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

bool checkIsAtomcExpresion(TokenStream& head) {
	auto t = head.peak(0);
	if (not t.has_value()) return false;
	if (t->kind == Token::Type::atom) return true;
	if (t->kind == Token::Type::string_literal) return true;
	if (t->kind == (Token::Type)ProcedureTokenType::integer_literal) return true;
	if (t->kind == (Token::Type)ProcedureTokenType::double_literal) return true;
	return false;
}

int getPrecedence(std::optional<Token> a) {
	if (not a) return 0;
	if (a->value == "=") return 3;
	if (a->value == "*") return 2;
	if (a->value == "/") return 2;
	if (a->value == "+") return 1;
	if (a->value == "-") return 1;
	TODO("Implement real operator precedence");
}

Result<Ast::NodeIndex, ErrorT> parsePrimary(TokenStream& head, Ast& ast) {
	if (checkIsAtomcExpresion(head)) {
		auto t = head.peak();
		head.consume();
		return newLeaf(ast, t.value());
	}
	return ErrorT{ head.peak(0)->value,std::format("Expected primary Expresion get {}",head.peak()->value.to_string()),"TODO long error" };
}

//Result<Ast::NodeIndex, ErrorT> parseExpresion1(TokenStream& head, Ast& ast, Ast::NodeIndex lhs, int min_precedence) {
//	//auto lookahead = head.peak();
//	//while (lookahead->kind == (Token::Type)ProcedureTokenType::operator_t and getPrecedence(lookahead) >= min_precedence) {
//	//	auto op = lookahead;
//	//	auto rhs = parsePrimary(head, ast);
//	//	lookahead = head.peak();
//	//}
//}
// string?
// string?int|bool = string?(int|bool)
// string?int?bool = (string?int)?bool


struct SyntaxProduction {
	std::string result;
	std::vector<std::string> seq;
};

//exp := 
// [ ] prefixop exp // prefix > infix
// [ ] exp sufixop // sufix > infix 
// [ ] exp infixop exp
// [v] ( exp ) -> bracets
// [v] exp(exp) -> function_call
// [v] exp{exp} -> construct_struct/object
// [v] exp[exp] -> array_acess
// [ ] atom
// [ ] literal
// [ ] operator
// [v] `empty` 
// 
// primary primary => error
//
// prefix exp sufix => prefix ( exp sufix ) if sufix  > prefix
// prefix exp sufix => (prefix exp ) sufix  if prefix > sufix
//

Result<Ast::NodeIndex, ErrorT> parseExpresion(TokenStream& head, Ast& ast) {
	// not Token?
	Ast::Node res;
	res.kind = (uint64_t)NodeKinds::expression;
	bool lastOneIsOperatorOrEmpty = true;
	while (true) {
		if (lastOneIsOperatorOrEmpty and head.optional(Token::Type::parenthesis, "(")) { // brackets
			parseExpresion(head,ast);
			auto t =  head.require(Token::Type::parenthesis,")");
			if (not t) return (ErrorT)t;
		}
		else if (not lastOneIsOperatorOrEmpty and head.optional(Token::Type::parenthesis, "(")) { //function_call or tuple
			do{
				parseExpresion(head, ast);
			}while(head.optional((Token::Type)ProcedureTokenType::comma));
			auto t = head.require(Token::Type::parenthesis, ")");
			if (not t) return (ErrorT)t;
		}
		else if (not lastOneIsOperatorOrEmpty and head.optional(Token::Type::parenthesis, "{")) { //constructor
			do {
				parseExpresion(head, ast);
			} while (head.optional((Token::Type)ProcedureTokenType::comma));
			auto t = head.require(Token::Type::parenthesis, "}");
			if (not t) return (ErrorT)t;
		}
		else if (not lastOneIsOperatorOrEmpty and head.optional(Token::Type::parenthesis, "[")) { //array acess
			do {
				parseExpresion(head, ast);
			} while (head.optional((Token::Type)ProcedureTokenType::comma));
			auto t = head.require(Token::Type::parenthesis, "]");
			if(not t) return (ErrorT)t;
		}
		break;//if token not handled stop parsing expression
	}

	return newNode(ast, res);
	//std::vector<Ast::NodeIndex> output;
	//std::vector<Preamble::Procedure::Operator> stack;
	//while (true) {
	//	if (checkIsAtomcExpresion(head)) {
	//		output.push_back(newLeaf(ast, head.peak(0).value()));
	//		head.consume(1);
	//		continue;
	//	}
	//	if (head.check((Token::Type)ProcedureTokenType::operator_t)) {
	//		continue;
	//	}
	//	if (head.optional(Token::Type::parenthesis, "(")) { // function calls
	//		auto t = parseExpresion(head, ast);
	//		if (not t) printError((ErrorT)t);
	//		else
	//			head.require(Token::Type::parenthesis, ")");
	//	}
	//	if (head.optional(Token::Type::parenthesis, "[")) { // arrays
	//		auto t = parseExpresion(head, ast);
	//		if (not t) printError((ErrorT)t);
	//		else
	//			head.require(Token::Type::parenthesis, "]");
	//	}
	//	if (head.optional(Token::Type::parenthesis, "{")) { // construct value
	//		auto t = parseExpresion(head, ast);
	//		if (not t) printError((ErrorT)t);
	//		else
	//			head.require(Token::Type::parenthesis, "{");
	//	}
	//	break; // if i dont recognize token stop expression
	//}
	//if (head.check(Token::Type::parenthesis, ")")) {}
	//if (head.check(Token::Type::parenthesis, "]")) {}
	//if (head.check(Token::Type::parenthesis, "}")) {}
	//if (head.check(Token::Type::parenthesis, ",")) {}
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
		case Preamble::Procedure::NodeKinds::square_bracket: return "square_bracket";
		case Preamble::Procedure::NodeKinds::bracket: return "bracket";
		case Preamble::Procedure::NodeKinds::curly_bracket: return "curly_bracket";
		case Preamble::Procedure::NodeKinds::prefix_operator: return "prefix_operator";
		case Preamble::Procedure::NodeKinds::infix_operator: return "infix_operator";
		case Preamble::Procedure::NodeKinds::postfix_operator: return "postfix_operator";
		case Preamble::Procedure::NodeKinds::function_call: return "function_call";
		case Preamble::Procedure::NodeKinds::array_acess: return "array_acess";
		case Preamble::Procedure::NodeKinds::Object_construct: return "Object_construct";
		case Preamble::Procedure::NodeKinds::varible_declaration: return "varible_declaration";
		case Preamble::Procedure::NodeKinds::for_loop: return "for_loop";
		case Preamble::Procedure::NodeKinds::if_branch: return "if_branch";
		case Preamble::Procedure::NodeKinds::while_loop: return "while_loop";
		case Preamble::Procedure::NodeKinds::const_mod: return "const_mod";
		case Preamble::Procedure::NodeKinds::mut_mod: return "mut_mod";
		case Preamble::Procedure::NodeKinds::expression: return "expression";
	}
	UNREACHABLE("Switch case not exausted");
}

Preamble::Procedure::Parser::~Parser()
{

}
