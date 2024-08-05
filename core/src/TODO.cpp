#include "TODO.h"

#include "CallStackTrace.h"

/* https://www.patorjk.com/software/taag/#p=display&f=Big%20Money-ne */

std::string TODO_logo =
R"(
 /$$$$$$$$ /$$$$$$  /$$$$$$$   /$$$$$$ 
|__  $$__//$$__  $$| $$__  $$ /$$__  $$
   | $$  | $$  \ $$| $$  \ $$| $$  \ $$
   | $$  | $$  | $$| $$  | $$| $$  | $$
   | $$  | $$  | $$| $$  | $$| $$  | $$
   | $$  | $$  | $$| $$  | $$| $$  | $$
   | $$  |  $$$$$$/| $$$$$$$/|  $$$$$$/
   |__/   \______/ |_______/  \______/ 
)";

std::string ASSERT_logo =
R"(
  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$$ /$$$$$$$  /$$$$$$$$
 /$$__  $$ /$$__  $$ /$$__  $$| $$_____/| $$__  $$|__  $$__/
| $$  \ $$| $$  \__/| $$  \__/| $$      | $$  \ $$   | $$   
| $$$$$$$$|  $$$$$$ |  $$$$$$ | $$$$$   | $$$$$$$/   | $$   
| $$__  $$ \____  $$ \____  $$| $$__/   | $$__  $$   | $$   
| $$  | $$ /$$  \ $$ /$$  \ $$| $$      | $$  \ $$   | $$   
| $$  | $$|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$  | $$   | $$   
|__/  |__/ \______/  \______/ |________/|__/  |__/   |__/   
)";

std::string UNREACHABLE_logo =
R"(
 /$$   /$$ /$$   /$$ /$$$$$$$  /$$$$$$$$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$$$$$$  /$$       /$$$$$$$$
| $$  | $$| $$$ | $$| $$__  $$| $$_____/ /$$__  $$ /$$__  $$| $$  | $$ /$$__  $$| $$__  $$| $$      | $$_____/
| $$  | $$| $$$$| $$| $$  \ $$| $$      | $$  \ $$| $$  \__/| $$  | $$| $$  \ $$| $$  \ $$| $$      | $$      
| $$  | $$| $$ $$ $$| $$$$$$$/| $$$$$   | $$$$$$$$| $$      | $$$$$$$$| $$$$$$$$| $$$$$$$ | $$      | $$$$$   
| $$  | $$| $$  $$$$| $$__  $$| $$__/   | $$__  $$| $$      | $$__  $$| $$__  $$| $$__  $$| $$      | $$__/   
| $$  | $$| $$\  $$$| $$  \ $$| $$      | $$  | $$| $$    $$| $$  | $$| $$  | $$| $$  \ $$| $$      | $$      
|  $$$$$$/| $$ \  $$| $$  | $$| $$$$$$$$| $$  | $$|  $$$$$$/| $$  | $$| $$  | $$| $$$$$$$/| $$$$$$$$| $$$$$$$$
 \______/ |__/  \__/|__/  |__/|________/|__/  |__/ \______/ |__/  |__/|__/  |__/|_______/ |________/|________/
)";


[[noreturn]]
void _TODO(std::string file, int64_t line, std::string function, const std::string& str) {
	std::cout << "> CallStack !!!" << std::endl;
	for (auto i : getCallStack()) {
		std::cout << " > " << i << std::endl;
	}
	std::cout << std::endl;
	std::cout << TODO_logo;
	std::cout << "\tLocation : " << file << ":" << line << std::endl;
	std::cout << "\tFunction : " << function << std::endl;
	std::cout << "\t\tTODO: " << str << std::endl;
	exit(1);
}

void _unreachable(std::string file, int64_t line, std::string function, const std::string& str)
{
	std::cout << "> CallStack !!!" << std::endl;
	for (auto i : getCallStack()) {
		std::cout << " > " << i << std::endl;
	}
	std::cout << std::endl;
	std::cout << UNREACHABLE_logo;
	std::cout << "\tLocation : " << file << ":" << line << std::endl;
	std::cout << "\tFunction : " << function << std::endl;
	std::cout << "\t\tAssert: " << str << std::endl;
	exit(2);
}

void _assert(std::string file, int64_t line, std::string function, std::string cond_str, bool cond, const std::string& str)
{
	if (cond)  return;
	std::cout << "> CallStack !!!" << std::endl;
	for (auto i : getCallStack()) {
		std::cout << " > " << i << std::endl;
	}
	std::cout << std::endl;
	std::cout << ASSERT_logo;
	std::cout << "\tLocation : " << file << ":" << line << std::endl;
	std::cout << "\tFunction : " << function << std::endl;
	std::cout << "\tCondition : " << cond_str << std::endl;
	std::cout << "\t\tAssert: " << str << std::endl;
	exit(2);
}
