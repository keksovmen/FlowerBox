#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>




static std::unordered_map<std::string, std::string> _map_query(std::string_view query)
{
	std::unordered_map<std::string, std::string> result;
	if(query.empty()){
		return result;
	}

	auto current = query;

	while(true){
		auto equals = current.find("=");
		if(equals == std::string_view::npos){
			//no more params
			break;
		}
	
		std::string_view key(current.begin(), equals);
		std::string_view value(key.end() + 1);
	
		auto nextParam = value.find("&");
		if(nextParam != std::string_view::npos){
			value = std::string_view(key.end() + 1, nextParam);
		}
	
		result[std::string(key)] = value;
		current = value.end() + 1;
	}

	return result;
}



int main()
{
	std::string_view query = "loh=&host=123";
	auto result = _map_query(query);

	for(auto r : result){
		std::cout << r.first << "\t" << r.second << std::endl;
	}
}