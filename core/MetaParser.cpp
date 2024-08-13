#include "MetaParser.h"
#include "TokenStream.h"
#include "PreambleDefinition.h"

MetaParser::MetaParser(PreambleRepository& pre_repo) : repo(pre_repo)
{
}

std::map<Token, std::optional<Token>> combineMaps(std::map<Token, std::optional<Token>>& file, std::map<Token, std::optional<Token>>& local) {
	std::map<Token, std::optional<Token>> res;
	for (const auto& i : file) {
		res.emplace(i.first,i.second);
	}
	for (const auto& i : local) {
		res.emplace(i.first, i.second);
	}
	return res;
}

std::optional<PreambleNode> MetaParser::parseProgram(MetaLexer& lexer)
{
	uint64_t i = 0;
	std::optional<Token> preambleToken = std::nullopt;
	std::vector<Token> head;
	std::vector<Token> body;
	bool addToHead = false;
	bool addToBody = false;
	int paramCount = 0;
	bool skip = false;
	std::map<Token,std::optional<Token>> file_atributes;
	std::map<Token, std::optional<Token>> local_atributes;
	std::optional<Token> last_atribute;
	while (auto a = lexer.lex()) {
		if (a->kind == Token::Type::comment) continue;
		if (skip == true and a->kind == Token::Type::preamble) skip = false;
		if (skip) {std::cout << "Error Recovery: "; continue;}
		if (a->kind == Token::Type::preamble) {preambleToken = a; addToHead = true; continue;}
		if(a->kind == Token::Type::atribute_name or a->kind == Token::Type::file_scope_atribute_name){
			last_atribute = a;
		}
		else if (last_atribute.has_value() and (a->kind == Token::Type::atribute_value or a->kind == Token::Type::file_scope_atribute_value)) {
			
			if (a->kind == Token::Type::atribute_value and last_atribute->kind == Token::Type::atribute_name) {
				local_atributes.emplace(last_atribute.value(), a);
			}
			else if (a->kind == Token::Type::file_scope_atribute_value and last_atribute->kind == Token::Type::file_scope_atribute_name) {
				file_atributes.emplace(last_atribute.value(), a);
			}
			else {
				TODO("Handle Error : atribute_name and atribute_value dont have matching kinds");
			}
		}
		else if (not last_atribute.has_value() and (a->kind == Token::Type::atribute_value or a->kind == Token::Type::file_scope_atribute_value)) {
			TODO("Handle Error : atribute value without atribute name");
		}
		else if(last_atribute.has_value()){
			if (last_atribute->kind == Token::Type::atribute_name) {
				local_atributes.emplace(last_atribute.value(), std::nullopt);
			}
			else if (last_atribute->kind == Token::Type::atribute_name) {
				file_atributes.emplace(last_atribute.value(), std::nullopt);
			}
			else {
				TODO("Handle unexpected error : Last_atribute unexpected kind");
			}
			last_atribute = std::nullopt; 
		}
			
		if (addToHead) {
			if (a.value().value == "{") {
				addToBody = true;
				addToHead = false;
			}
			else
				head.push_back(a.value());
		}
		if (a.value().value == "}" and paramCount == 0 and not addToBody) skip = true;
		if (addToBody) {
			body.push_back(a.value());

			if (a.value().value == "{") paramCount++;
			if (a.value().value == "}") paramCount--;

			if (paramCount == 0) {
				auto p = repo.get(repo.getPeambuleIndex(preambleToken->value))->parser;
				auto h = TokenStream(repo.getPeambuleIndex(preambleToken->value), head, repo);
				auto b = TokenStream(repo.getPeambuleIndex(preambleToken->value), body, repo);
				//addToBody = false;
				//addToHead = false;
				//head.clear();
				//body.clear();
				if (p) {
					auto ast = p->parse(h, b);
					PreambleNode res{ combineMaps(file_atributes,local_atributes) ,preambleToken.value() ,ast,p};
					return res;
				}
			}
		}
	}
	return std::nullopt;
}
