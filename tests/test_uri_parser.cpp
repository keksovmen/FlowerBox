#include <string>
#include <cassert>



int main()
{
	const char* path = "switch/1";
	auto uri = std::string(path);
	int index = uri.find_last_of("/") + 1;
	int id = std::atoi(uri.substr(index).c_str());
	assert(id == 1);

	path = "/switch/1";
	uri = std::string(path);
	id = std::atoi(uri.substr(uri.find_last_of("/", 0)).c_str());
	assert(id == 1);
}