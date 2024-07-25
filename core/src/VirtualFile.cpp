#include "VirtualFile.h"
#include <fstream>
#include "TODO.h"

VirtualFile::VirtualFile(std::string file_name)
{
	std::ifstream file(file_name);
	if (not file.good()) exit(1);
	file.seekg(0,std::ios::end);
	uint64_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	bytes.reserve(size);
	do{
		bytes.push_back(file.get());
	}while(file.good());
}

std::string VirtualFile::get(uint64_t start, uint64_t end)
{
	if(end <= start ) return "";
	std::string res;
	res.reserve(end-start);
	for (uint64_t i = start; i < end and start < end and i < bytes.size(); i++) {
		res+=bytes[i];
	}
	return res;
}

uint8_t VirtualFile::get(uint64_t offset)
{
	if(offset<0 or offset >= bytes.size()) return 0xff;
	return bytes[offset];
}

uint64_t VirtualFile::size() const noexcept
{
	return this->bytes.size();
}
