#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"

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

Result<Ast::NodeIndex, ErrorT> requireType(TokenStream& head, Ast& ast) {
	auto a = head.require(Token::Type::atom);
	Ast::Node res;
	if (head.optional(Token::Type::parenthesis, "(")) {
		while (not head.check(Token::Type::parenthesis, ")") and head.peak().has_value()) {
			res.children.push_back(ast.newLeaf(head.peak().value()));
			head.consume();
		}
		return ast.newNode(res);
	}
	else {
		if (a) {
			return ast.newLeaf(a);
		}
		else {
			printError(a);
			return (ErrorT)a;
		}
	}
}

Result<Ast::NodeIndex, ErrorT> requireId(TokenStream& head, Ast& ast) {
	auto a = head.require(Token::Type::atom);
	if (a) {
		return ast.newLeaf(a);
	}
	else {
		printError(a);
		return (ErrorT)a;
	}
}

Ast::NodeIndex parseHead(TokenStream& head, Ast& ast) {
	std::vector<ErrorT> errors;
	Ast::Node res;
	res.kind = 1;
	res.children.push_back(requireId(head, ast));
	if (auto t = head.require(Token::Type::parenthesis, "(")){
		res.children.push_back(ast.newLeaf(t));
	}
	else {
		return Ast::ErrorNode;
	}
	if(head.check(Token::Type::atom)){
		do {
			res.children.push_back(ast.newLeaf(head.require(Token::Type::atom)));
			head.require((Token::Type)ProcedureTokenType::colon);
			res.children.push_back(requireType(head,ast));
		}while(head.optional((Token::Type)ProcedureTokenType::comma));
	}
	res.children.push_back(ast.newLeaf(head.require(Token::Type::parenthesis, ")")));
	if (head.optional((Token::Type)ProcedureTokenType::operator_t,"->")) {
		res.children.push_back(requireType(head,ast));
	}
	head.requireEmpty();
	return ast.newNode(res);
}

Ast::NodeIndex parseBody(TokenStream& body,Ast& ast){
	return Ast::ErrorNode;
}

Ast Preamble::Procedure::Parser::parse(TokenStream& head, TokenStream& body)
{
	Ast res;
	res.headNode = parseHead(head,res).second;
	res.bodyNode = parseBody(body,res).second;
	return res;
}

Preamble::Procedure::Parser::~Parser()
{

}
