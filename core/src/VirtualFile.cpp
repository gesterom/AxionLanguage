#include "VirtualFile.h"
#include <fstream>
#include "TODO.h"

VirtualFile::VirtualFile(std::string file_name)
{
	std::ifstream file(file_name);
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
	ASSERT(start >= 0 and start <= end, "start check");
	ASSERT(end >= 0  and end < bytes.size(), "end check");
	std::string res;
	res.reserve(end-start);
	for (uint64_t i = start; i < end; i++) {
		res+=bytes[i];
	}
	return res;
}

uint8_t VirtualFile::get(uint64_t offset)
{
	ASSERT(offset>=0 and offset < bytes.size(),"offset check");
	return bytes[offset];
}

uint64_t VirtualFile::size() const noexcept
{
	return this->bytes.size();
}
