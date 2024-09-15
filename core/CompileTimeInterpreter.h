#pragma once

#include "AST.h"
#include "SyntaxRepository.h"

class CompileTimeInterpreter
{
	using Runtime_Type = uint64_t; // index in knownTypes array
	using Runtime_Function = uint64_t; // index in knowFunctions array
	struct TypeInfo {
		enum class Type {
			buildIn,
			pointerType,
			referenceType,
			arrayType,
			sliceType,
			structType,
			functionType,
		};
		struct RecordDef {
			std::map<std::string, Runtime_Type>* fields;
		};
		struct FunctTypeDef {
			Runtime_Type retType;
			Runtime_Type args;
		};
		union TypeDef {
			void* buildin_val; // name is all you have :D 
			RecordDef structdef_val;
			FunctTypeDef funcdef_val;
			Runtime_Type innerType; // for slice,reference,pointer,array

			TypeDef();
			~TypeDef();
		};
		std::string name;
		Type type;
		TypeDef def;
		TypeInfo(std::string n);
		TypeInfo(std::string n, RecordDef rec);
		TypeInfo(std::string n, FunctTypeDef func);
		TypeInfo(Type t, std::string n, Runtime_Type innner);
		~TypeInfo();
	};
	struct FunctionInfo {
		std::string name;
		Runtime_Type type;
		Ast code;
	};
	std::vector<TypeInfo> knowTypes;
	std::vector<FunctionInfo> knowFunctions;
	struct RuntimeExpression {
		Runtime_Type type = 0;
		union ValueT {
			int int_val = 0;
			double double_val;
			bool bool_val;
			Runtime_Type type_val;
			Runtime_Function function_val;
		};
		ValueT val;
		RuntimeExpression(int);
		RuntimeExpression(double);
		RuntimeExpression(bool);
		RuntimeExpression(Runtime_Type);
		RuntimeExpression(CompileTimeInterpreter&, Runtime_Function);
	};
	std::map<std::string, RuntimeExpression> knownVariables;
	auto preVisit(PreambleNode& code, SyntaxRepository& repo);
	auto postVisit(PreambleNode& code, SyntaxRepository& repo);
	std::vector<RuntimeExpression> lastExpressionValue;
public:
	CompileTimeInterpreter();
	void execute(PreambleNode& code, SyntaxRepository& repo);
};

