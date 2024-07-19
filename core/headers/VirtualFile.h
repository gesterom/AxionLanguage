#pragma once
#include <string>
#include <vector>

class VirtualFile
{
	std::vector<uint8_t> bytes;
	public:
	VirtualFile(std::string file_name);
	std::string get(uint64_t start,uint64_t end);
	uint8_t get(uint64_t offset);
	uint64_t size()const noexcept;
};

