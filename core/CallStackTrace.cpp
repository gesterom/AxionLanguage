#include "CallStackTrace.h"


#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

std::vector<std::string> getCallStack() {
	std::vector<std::string> stack;
	void* stackArray[100];
	unsigned short frames;
	SYMBOL_INFO* symbol;
	HANDLE process;

	process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);

	frames = CaptureStackBackTrace(0, 100, stackArray, NULL);
	symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	if (symbol == nullptr) return stack;
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (unsigned int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64)(stackArray[i]), 0, symbol);
		stack.push_back("0x" + std::to_string(symbol->Address) + " - " + std::string(symbol->Name));
	}

	free(symbol);
	return stack;
}

#elif defined(__unix__) || defined(__APPLE__)
#include <execinfo.h>
#include <cxxabi.h>

std::vector<std::string> getCallStack() {
	std::vector<std::string> stack;
	void* array[100];
	size_t size;
	char** symbols;

	size = backtrace(array, 100);
	symbols = backtrace_symbols(array, size);

	for (size_t i = 0; i < size; i++) {
		// Demangling the function names
		char* demangled = abi::__cxa_demangle(symbols[i], NULL, NULL, NULL);
		if (demangled) {
			stack.push_back(std::string(demangled));
			free(demangled);
		}
		else {
			stack.push_back(std::string(symbols[i]));
		}
	}

	free(symbols);
	return stack;
}

#endif
