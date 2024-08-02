#include "TODO.h"

#include "CallStackTrace.h"

[[noreturn]]
void _TODO(std::string file, int64_t line, std::string function, const std::string& str) {
	std::cout<<"> CallStack !!!"<<std::endl;
	for (auto i : getCallStack()) {
		std::cout<<" > "<< i <<std::endl;
	}
	std::cout<<std::endl;
	std::cout<<std::endl;
	std::cout << " /$$$$$$$$ /$$$$$$  /$$$$$$$   /$$$$$$ " << std::endl;
	std::cout << "|__  $$__//$$__  $$| $$__  $$ /$$__  $$" << std::endl;
	std::cout << "   | $$  | $$  \\ $$| $$  \\ $$| $$  \\ $$" << std::endl;
	std::cout << "   | $$  | $$  | $$| $$  | $$| $$  | $$" << std::endl;
	std::cout << "   | $$  | $$  | $$| $$  | $$| $$  | $$" << std::endl;
	std::cout << "   | $$  | $$  | $$| $$  | $$| $$  | $$" << std::endl;
	std::cout << "   | $$  |  $$$$$$/| $$$$$$$/|  $$$$$$/" << std::endl;
	std::cout << "   |__/   \\______/ |_______/  \\______/ " << std::endl;
	std::cout<<std::endl;
	std::cout << "\tLocation : " << file << ":" << line << std::endl;
	std::cout << "\tFunction : " << function << std::endl;
	std::cout << "\t\tTODO: " << str << std::endl;
	exit(1);
}

void _assert(std::string file, int64_t line, std::string function,std::string cond_str, bool cond, const std::string& str)
{
	if(cond)  return;
	std::cout << "> CallStack !!!" << std::endl;
	for (auto i : getCallStack()) {
		std::cout << " > " << i << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout<<"  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$$ /$$$$$$$  /$$$$$$$$"<<std::endl;
	std::cout<<" /$$__  $$ /$$__  $$ /$$__  $$| $$_____/| $$__  $$|__  $$__/"<<std::endl;
	std::cout<<"| $$  \\ $$| $$  \\__/| $$  \\__/| $$      | $$  \\ $$   | $$   "<<std::endl;
	std::cout<<"| $$$$$$$$|  $$$$$$ |  $$$$$$ | $$$$$   | $$$$$$$/   | $$   "<<std::endl;
	std::cout<<"| $$__  $$ \\____  $$ \\____  $$| $$__/   | $$__  $$   | $$   "<<std::endl;
	std::cout<<"| $$  | $$ /$$  \\ $$ /$$  \\ $$| $$      | $$  \\ $$   | $$   "<<std::endl;
	std::cout<<"| $$  | $$|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$  | $$   | $$   "<<std::endl;
	std::cout<<"|__/  |__/ \\______/  \\______/ |________/|__/  |__/   |__/   "<<std::endl;
	std::cout << std::endl;
	std::cout << "\tLocation : " << file << ":" << line << std::endl;
	std::cout << "\tFunction : " << function << std::endl;
	std::cout << "\tCondition : "<< cond_str<<std::endl;
	std::cout << "\t\tAssert: " << str << std::endl;
	exit(2);
}
