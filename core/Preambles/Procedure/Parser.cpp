#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"
#include <format>
#include <iostream>

using namespace Preamble::Procedure;

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

Result<Ast::NodeIndex, ErrorT> Parser::requireName(TokenStream& head, Ast& ast) {
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

Result<Ast::NodeIndex, ErrorT> Parser::requireType(TokenStream& head, Ast& ast) {
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

Result<Ast::NodeIndex, ErrorT> Parser::parsePrimary(TokenStream& head, Ast& ast) {
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

//exp := 
// [ ] prefixop exp // prefix > infix
// [ ] exp sufixop // sufix > infix 
// [ ] exp infixop exp
// [v] ( exp ) -> bracets
// [v] exp(exp) -> function_call
// [v] exp{exp} -> construct_struct/object
// [v] exp[exp] -> array_acess
// [v] atom
// [v] literal
// [ ] prefix_operator
// [v] `empty` 
// 
// primary primary => error
//
// prefix exp sufix => prefix ( exp sufix ) if sufix  > prefix
// prefix exp sufix => (prefix exp ) sufix  if prefix > sufix
//

Result<Ast::NodeIndex, ErrorT> Parser::parseExpressionListOperator(TokenStream& head, Ast& ast, NodeKinds kind, std::string endToken, std::vector<Ast::NodeIndex>& output) {
	Ast::Node temp;
	temp.kind = (uint64_t)NodeKinds::expression_list;
	do {
		auto a = parseExpresion(head, ast);
		if (a) temp.children.push_back(a);
		else break;
	} while (head.optional((Token::Type)ProcedureTokenType::comma));
	auto t = head.require(Token::Type::parenthesis, endToken);
	if (not t) return (ErrorT)t;
	auto args = newNode(ast, temp);
	auto func = output[output.size() - 1];
	output.pop_back();;
	Ast::Node funcCall;
	funcCall.kind = (uint64_t)NodeKinds::function_call;
	funcCall.children.push_back(func);
	funcCall.children.push_back(args);
	output.push_back(newNode(ast, funcCall));
	return output[output.size() - 1];
}

std::optional<Token> isPrimaryExpresion(TokenStream& head) {
	if (not head.peak().has_value()) return std::nullopt;
	auto k = head.peak()->kind;
	if (k == Token::Type::atom
		or k == Token::Type::string_literal
		or k == (Token::Type)ProcedureTokenType::bool_literal
		or k == (Token::Type)ProcedureTokenType::double_literal
		or k == (Token::Type)ProcedureTokenType::integer_literal
		) {
		auto t = head.peak();
		head.consume();
		return t;
	}
	return std::nullopt;
}

bool isOperator(Ast& ast, Ast::NodeIndex index) {
	if (index.first == 1) return false; // needs to be leaf
	//if (ast.nodes[index.second].children.size() == 0) return false; // needs to have atleast 1 children 
	auto op = ast.leafs[index.second];
	if (op.kind == (Token::Type)ProcedureTokenType::operator_t) return true;
	return false;
}

bool isValue(Ast& ast, Ast::NodeIndex index) {
	return not isOperator(ast, index);
}

bool isPrefix(OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isPrefix(ast.leafs[index->second].value.to_string());
}

bool isInfix(OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isInfix(ast.leafs[index->second].value.to_string());
}

bool isSuffix(OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isSuffix(ast.leafs[index->second].value.to_string());
}

//
//void reduce_output(std::vector<Ast::NodeIndex> last, int new_precedence) {
//	while (true) {
//		if (last[0] == op and last[1] == val and last[0].sufix and last[0].precedence > new_precedence) {
//			output.push(newExpression(last.pop, last.pop, sufix));
//			continue;
//		}
//		if (last[0] == val and last[1] == op and last[2] == val and last[1].precedence > new_precedence and last[1].infix) {
//			output.push(newExpression(last.pop, last.pop, infix));
//			continue;
//		}
//		if (last[0] == val and last[1] == op and last[1].precedence > new_precedence and last[1].prefix) {
//			output.push(newExpression(last.pop, last.pop, prefix));
//			continue;
//		}
//		break;
//	}
//}

std::optional<Ast::NodeIndex> get(std::vector<Ast::NodeIndex>& vec, int32_t index) {
	if (index >= vec.size()) return std::nullopt;
	if (index < 0) return std::nullopt;
	return vec[index];
}

void reduce_output(OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& last, int32_t new_precedence) {
	while (true)
	{
		if (
			last.size() > 1 and
			isOperator(ast, last[last.size() - 1]) and
			isValue(ast, last[last.size() - 2]) and
			isSuffix(repo, ast, last[last.size() - 1])) { //SUFFUX operator
			auto pre = repo.getPrecedenceSuffix(ast.leafs[last[last.size() - 1].second].value.to_string());
			auto left = repo.isleftAssociativitySuffix(ast.leafs[last[last.size() - 1].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				Ast::Node res;
				res.kind = (uint64_t)NodeKinds::suffix_operator;
				res.children.push_back(last[last.size() - 1]);
				res.children.push_back(last[last.size() - 2]);
				last.pop_back(); last.pop_back();
				last.push_back(newNode(ast, res));
				continue;
			}
		}
		else if (
			last.size() > 2 and
			isValue(ast, last[last.size() - 1]) and
			isOperator(ast, last[last.size() - 2]) and
			isValue(ast, last[last.size() - 3]) and
			isInfix(repo, ast, last[last.size() - 2])) { // INFIX operator
			auto pre = repo.getPrecedenceInfix(ast.leafs[last[last.size() - 2].second].value.to_string());
			auto left = repo.isleftAssociativityInfix(ast.leafs[last[last.size() - 2].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				Ast::Node res;
				res.kind = (uint64_t)NodeKinds::infix_operator;
				res.children.push_back(last[last.size() - 2]);
				res.children.push_back(last[last.size() - 3]);
				res.children.push_back(last[last.size() - 1]);
				last.pop_back(); last.pop_back(); last.pop_back();
				last.push_back(newNode(ast, res));
				continue;
			}
		}
		else if (
			last.size() > 1 and
			isValue(ast, last[last.size() - 1]) and
			isOperator(ast, last[last.size() - 2]) and
			isPrefix(repo, ast, last[last.size() - 2]))
		{ // PREFIX operator
			auto pre = repo.getPrecedencePrefix(ast.leafs[last[last.size() - 2].second].value.to_string());
			auto left = repo.isleftAssociativityPrefix(ast.leafs[last[last.size() - 2].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				Ast::Node res;
				res.kind = (uint64_t)NodeKinds::prefix_operator;
				res.children.push_back(last[last.size() - 2]);
				res.children.push_back(last[last.size() - 1]);
				last.pop_back(); last.pop_back();
				last.push_back(newNode(ast, res));
				continue;
			}
		}
		break;
	}
}

std::optional<CodeLocation> min(Ast& ast, Ast::NodeIndex index) {
	if (index.first == 0) return ast.leafs[index.second].value;
	if (ast.nodes[index.second].children.size() == 0) return std::nullopt;
	auto res = min(ast, ast.nodes[index.second].children[0]);
	for (size_t i = 1; i < ast.nodes[index.second].children.size(); i++) {
		auto temp = min(ast, ast.nodes[index.second].children[i]);
		if (not res.has_value()) res = temp;

		if (temp.has_value() and temp->file_pointer_start() < res->file_pointer_start())
			res = temp;
	}
	return res;
}

int32_t deducePrecedence(OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& output, std::string representation) {
	ASSERT(output.size() > 0, "output size > 0; parseExpression as caller");

	int32_t p1 = -1;
	int32_t p2 = -1;
	int32_t p3 = -1;

	if (isValue(ast, output[output.size() - 1])) {
		p2 = repo.getPrecedenceInfix(representation);
		p3 = repo.getPrecedenceSuffix(representation);
	}
	if (isPrefix(repo, ast, output[output.size() - 1]) and repo.isPrefix(representation)) {
		p1 = repo.getPrecedencePrefix(representation);
	}
	if (isInfix(repo, ast, output[output.size() - 1]) and repo.isPrefix(representation)) {
		p1 = repo.getPrecedencePrefix(representation);
	}
	if (isSuffix(repo, ast, output[output.size() - 1]) and repo.isInfix(representation)) {
		p2 = repo.getPrecedenceInfix(representation);
	}
	if (isSuffix(repo, ast, output[output.size() - 1]) and repo.isSuffix(representation)) {
		p3 = repo.getPrecedenceSuffix(representation);
	}
	int32_t pre = 0;
	if (p1 != -1 and (p1 >= p2 or p2 == -1) and (p1 >= p3 or p3 == -1))
		pre = p1;
	if (p2 != -1 and (p2 >= p1 or p1 == -1) and (p2 >= p3 or p3 == -1))
		pre = p2;
	if (p3 != -1 and (p3 >= p2 or p2 == -1) and (p3 >= p1 or p1 == -1))
		pre = p3;
	ASSERT(pre != -1, "TODO : Syntax error expresion");
	return pre;
}

// let a = vector(int){nullptr,0,0};
Result<Ast::NodeIndex, ErrorT> Parser::parseExpresion(TokenStream& head, Ast& ast) {
	// not Token?
	Ast::Node res;
	res.kind = (uint64_t)NodeKinds::expression;
	std::vector<Ast::NodeIndex> output;
	int32_t precedence = 0;

	bool lastIsValue = false;
	while (true) {
		//value
		if (not lastIsValue and head.optional(Token::Type::parenthesis, "(")) { // brackets
			auto e = parseExpresion(head, ast);
			if (e) output.push_back(e);
			auto t = head.require(Token::Type::parenthesis, ")");
			if (not t) return (ErrorT)t;
			lastIsValue = true;
			continue;
		}
		else if (not lastIsValue and head.optional(Token::Type::parenthesis, "[")) { //array literal
			Ast::Node temp;
			temp.kind = (uint64_t)NodeKinds::array_literal;
			do {
				auto a = parseExpresion(head, ast);
				if (a) temp.children.push_back(a);
				else break;
			} while (head.optional((Token::Type)ProcedureTokenType::comma));
			auto t = head.require(Token::Type::parenthesis, "]");
			output.push_back(newNode(ast, temp));
			lastIsValue = true;
			continue;
		}
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "(")) { //function_call or tuple
			parseExpressionListOperator(head, ast, NodeKinds::function_call, ")", output);
			lastIsValue = true;
			continue;
		} //  
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "{")) { //constructor
			parseExpressionListOperator(head, ast, NodeKinds::Object_construct, "}", output);
			lastIsValue = true;
			continue;
		}
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "[")) { //array acess
			parseExpressionListOperator(head, ast, NodeKinds::array_acess, "]", output);
			lastIsValue = true;
			continue;
		}
		else if (auto t = isPrimaryExpresion(head)) {
			output.emplace_back(newLeaf(ast, t.value()));
			lastIsValue = true;
			continue;
		}
		//operators
		// int ? not ++ int 
		// adsas + das int ? -> 3/6
		//      val  op		-> val
		// val  op   val	-> val
		//      op   val	-> val
		// int ? (not int) -> 6
		// val op op val op op val op val op val 
		// +unary != +add
		// z = not x ++ + y * 2 ** 3 ;
		// 1 * 2 + 3
		// 1 2 3 * +
		// output : 1 2
		// ops : +
		// 1 2 * 3 + 
		//
		else if (auto t = head.optional((Token::Type)::ProcedureTokenType::operator_t)) {
			if (output.size() > 0) {
				auto pre = deducePrecedence(repo, ast, output, t->value.to_string());
				reduce_output(repo, ast, output, pre);
			}
			output.emplace_back(newLeaf(ast, t.value()));
			lastIsValue = false;
			continue;
		}
		break;//if token not handled stop parsing expression
	}
	reduce_output(repo, ast, output, INT32_MAX);
	if (output.size() == 1) {
		return (Ast::NodeIndex)output[0];
	}
	else if (output.size() == 0) {
		Ast::Node res;
		res.kind = (uint64_t)NodeKinds::expression;
		return newNode(ast, res);
	}
	else {
		auto err = min(ast, output[1]);
		ASSERT(err != std::nullopt, "something fishy!!");
		if (err) {
			return ErrorT{ err.value(),std::format("Unexpected token {}, invalid expresion !!",err->to_string()),"TODO long error" };
		}
		else
			UNREACHABLE("To remove warning");
	}
}

// atom : type/*expresion that evaluated in compile time will result in Type*/
Result<Ast::NodeIndex, ErrorT> Parser::requireFunctionHeadArgs(TokenStream& head, Ast& ast) {
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
std::optional<Ast::NodeIndex> Parser::parseHead(TokenStream& head, Ast& ast) {
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

std::optional<Ast::NodeIndex> Parser::parseBody(TokenStream& body, Ast& ast) {
	body.require(Token::Type::parenthesis, "{");
	auto t = parseExpresion(body, ast);
	body.require(Token::Type::parenthesis, "}");
	if (t) return t;
	else return std::nullopt;
}

Preamble::Procedure::Parser::Parser(OperatorRepository& repo) : repo(repo)
{
}

Ast Preamble::Procedure::Parser::parse(TokenStream& head, TokenStream& body)
{
	Ast res;
	res.headNode = parseHead(head, res);
	if (res.headNode == std::nullopt) return res;
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
	case Preamble::Procedure::NodeKinds::suffix_operator: return "postfix_operator";
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
	case Preamble::Procedure::NodeKinds::expression_list: return "expression_list";
	case Preamble::Procedure::NodeKinds::operator_in_construction: return "operator_in_construction";
	}
	UNREACHABLE(std::format("Switch case not exausted {}", n));
}

Preamble::Procedure::Parser::~Parser()
{

}
