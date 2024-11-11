#include "fb_ram_storage.hpp"



using namespace fb;
using namespace storage;



const char* RamStorage::getName() const
{
	return "RamStorage";
}

bool RamStorage::init()
{
	return true;
}

bool RamStorage::hasKey(const std::string& partition, const std::string& key) const
{
	return _data.contains(partition) && _data.at(partition).contains(key);
}

bool RamStorage::writeValue(const std::string& partition, const std::string& key, const std::string& value)
{
	if(!_data.contains(partition)){
		_data[partition] = Page();
	}

	if(!_data[partition].contains(key)){
		_data[partition][key] = Entry();
	}

	_data[partition][key].second = value;

	return true;
}

bool RamStorage::writeValue(const std::string& partition, const std::string& key, int value)
{
	if(!_data.contains(partition)){
		_data[partition] = Page();
	}

	if(!_data[partition].contains(key)){
		_data[partition][key] = Entry();
	}

	_data[partition][key].first = value;

	return true;
}

bool RamStorage::readValue(const std::string& partition, const std::string& key, std::string& out) const
{
	if(!_data.contains(partition)){
		return false;
	}

	if(!_data.at(partition).contains(key)){
		return false;
	}

	out = _data.at(partition).at(key).second;

	return true;
}

bool RamStorage::readValue(const std::string& partition, const std::string& key, int& out) const
{
	if(!_data.contains(partition)){
		return false;
	}

	if(!_data.at(partition).contains(key)){
		return false;
	}

	out = _data.at(partition).at(key).first;

	return true;
}
