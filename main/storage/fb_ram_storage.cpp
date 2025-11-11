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
	return _contains(_data, {p}) && _contains(_data.at(p), k);
}

bool RamStorage::writeValue(std::string_view partition, std::string_view key, std::string_view value)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_contains(_data, p)){
		_data[p] = Page();
	}

	if(!_contains(_data[p], k)){
		_data[p][k] = Entry();
	}

	_data[p][k].second = value;

	return true;
}

bool RamStorage::writeValue(std::string_view partition, std::string_view key, int64_t value)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_contains(_data, p)){
		_data[p] = Page();
	}

	if(!_contains(_data[p], k)){
		_data[p][k] = Entry();
	}

	_data[p][k].first = value;

	return true;
}

bool RamStorage::readValue(std::string_view partition, std::string_view key, std::string& out) const
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_contains(_data, p)){
		return false;
	}

	if(!_contains(_data.at(p), k)){
		return false;
	}

	out = _data.at(p).at(k).second;

	return true;
}

bool RamStorage::readValue(std::string_view partition, std::string_view key, int64_t& out) const
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_contains(_data, p)){
		return false;
	}

	if(!_contains(_data.at(p), k)){
		return false;
	}

	out = _data.at(p).at(k).first;

	return true;
}

void RamStorage::clearValue(std::string_view partition, std::string_view key)
{
	auto p = std::string(partition);
	auto k = std::string(key);

	if(!_contains(_data, p)){
		return;
	}

	if(!_contains(_data.at(p), k)){
		return;
	}

	_data.at(p).erase(k);
}

bool RamStorage::_contains(const std::unordered_map<std::string, Page>& map, const std::string& v)
{
	return map.find(v) != map.end();
}

bool RamStorage::_contains(const Page map, const std::string& v)
{
	return map.find(v) != map.end();
}
