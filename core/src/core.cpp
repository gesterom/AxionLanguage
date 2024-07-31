// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <cassert>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "CodeLocation.h"
#include "MetaLexer.h"

struct cliArgs {
	struct cliArg {
		uint8_t short_name;
		std::string longName;
		std::optional<std::string> default_value;
		std::optional<std::string> description;
	};
};

// if provided file dont contain build preamble use default build file
// if provided with "copy default build" create default build file insde curent dir 
// if provided with debug_build start interactive debuger and break right after ast is builded
// if provided with interactive compile project and start interactive shell before main
// 
// usage:
//  cerberus build.dog relasese_build # build using build.dog as build file and start function all
//  cerbersu build.dog # build using build.dog and all atom as entry if not founded but multiple build present print error with "i coude not decide with to choose"
//  cerbersu build.dog # build if only one build preamble present use it with warning if not named all
//  cerbersu build.dog # build if there is not any build preamble then use default build script
// 
//  cerberus --copy_default [<file_name>] # copy default build script into file location directory under name <file_name>, error if file exists # -F if to force
// 
//  cerberus --interactive_build <file_name> # start build and stop interactive on first build script
//  cerberus --interactive <file_name> # build produce 
//			# if --interactive defined and executed produce_exe or produce_elf give warning/info that you are sopouse to use 
//			# produce_exe_interactive() or produce_elf_interactive() # or produce_exe(,interactive=true) produce_elf(,interactive=true)
// 
// 

int main(int argc, char** args)
{
	if (argc < 2) return -1;
	for (int i = 0; i < argc; i++) {
		std::cout << i << " : " << args[i] << std::endl;
	}
	PreambleRepository repo;
	//TODO you can only compile one file but file "compiled" is build script writen in cerberus that will call cerberus compiler again and again
	// useage : $ cerberus <build_filename>
	for (int file_count = 1; file_count < argc and std::string(args[file_count]) != "-o"; file_count++) {// FIXME -o makes output 
		MetaLexer lexer(repo, args[file_count]);
		uint64_t i = 0;
		while (auto a = lexer.lex()) {
			if (a->kind == Token::Type::comment and a->preamble_token == -1) continue;
			std::cout << ++i << " : (" << repo.to_string(a->preamble_token) << "/" << repo.to_string(a->preamble_token, a->kind) << ") " << (a->value) << " -> " << a->value.start() << "-" << a->value.end() << std::endl;
		}
	}
}
