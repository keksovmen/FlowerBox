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

bool RamStorage::hasKey(std::string_view partition, std::string_view key) const
{
	auto p = std::string(partition);
	auto k = std::string(key);
	return _data.contains({p}) && _data.at(p).contains(k);
}

bool RamStorage::writeValue(std::string_view partition, std::string_view key, std::string_view value)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_data.contains(p)){
		_data[p] = Page();
	}

	if(!_data[p].contains(k)){
		_data[p][k] = Entry();
	}

	_data[p][k].second = value;

	return true;
}

bool RamStorage::writeValue(std::string_view partition, std::string_view key, int64_t value)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_data.contains(p)){
		_data[p] = Page();
	}

	if(!_data[p].contains(k)){
		_data[p][k] = Entry();
	}

	_data[p][k].first = value;

	return true;
}

bool RamStorage::readValue(std::string_view partition, std::string_view key, std::string& out) const
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_data.contains(p)){
		return false;
	}

	if(!_data.at(p).contains(k)){
		return false;
	}

	out = _data.at(p).at(k).second;

	return true;
}

bool RamStorage::readValue(std::string_view partition, std::string_view key, int64_t& out) const
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_data.contains(p)){
		return false;
	}

	if(!_data.at(p).contains(k)){
		return false;
	}

	out = _data.at(p).at(k).first;

	return true;
}

void RamStorage::clearValue(std::string_view partition, std::string_view key)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_data.contains(p)){
		return;
	}

	if(!_data.at(p).contains(k)){
		return;
	}

	_data.at(p).erase(k);
}