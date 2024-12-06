#include "fb_templates.hpp"

#include <cstdio>
#include <cstring>



using namespace fb;
using namespace templates;



Engine::Engine(char* buffer, int bufferSize, const DataConsumerCb& onDataConsume)
	: _buffer(buffer), _bufferSize(bufferSize), _dataConsumer(onDataConsume)
{

}

bool Engine::process(const std::string& fileName)
{
	auto* f = std::fopen(fileName.c_str(), "r");
	if(!f){
		return false;
	}

	State* state = &_stateInitial;

	while(!std::feof(f) && state != &_stateErr){
		state = &(state->process(*this, f));
	}

	bool result;
	if(state == &_stateErr){
		std::printf("Failed process for: %s\n", fileName.c_str());
		result = false;

	}else{
		std::printf("Finished process for: %s\n", fileName.c_str());
		result = true;
	}

	std::fclose(f);

	return result;
}

bool Engine::addIntArg(int val, const std::string& key)
{
	_args[key] = val;

	return true;
}

void Engine::_putToBuffer(char symbol)
{
	_buffer[_currentBuffer] = symbol;
	_currentBuffer++;

	if(_currentBuffer == _bufferSize){
		//flash
		_flush();
	}
}

void Engine::_putStrToBuffer(const std::string& str)
{
	for(int i = 0; i < str.length(); i++){
		_putToBuffer(str[i]);
	}
}

void Engine::_flush()
{
	if(_currentBuffer > 0){
		_dataConsumer(_buffer, _currentBuffer);
		_currentBuffer = 0;
	}
}

bool Engine::_getIntArg(int* out, const std::string& key)
{
	if(!_args.contains(key)){
		return false;
	}

	*out = _args.at(key);
	return true;
}



Engine::State& Engine::StateInitial::process(Engine& context, FILE* f)
{
	while(!std::feof(f)){
		const int val = std::fgetc(f);
		if(val == EOF){
			break;
		}

		if(val == '{'){
			std::printf("Possible start up sequence at %ld\n", std::ftell(f));
			return context._stateStart;

		}else{
			context._putToBuffer(val);
		}
	}

	context._flush();

	std::printf("EOF ocurred\n");

	return context._stateInitial;	
}

Engine::State& Engine::StateStart::process(Engine& context, FILE* f)
{
	const int val = std::fgetc(f);
	if(val == EOF){
		std::printf("Unexpected EOF in StateStart, %d\n", val);
		return context._stateErr;
	}

	if(val == '{'){
		std::printf("Found command begin sequence at %ld\n", std::ftell(f));
		return context._stateKeyword;

	}else{
		std::printf("Start up sequence not found\n");
		context._putToBuffer('{');
		context._putToBuffer(val);
		return context._stateInitial;
	}
}

Engine::State& Engine::StateKeyword::process(Engine& context, FILE* f)
{
	const int count = std::fscanf(f, "%s", context._cmdBuffer);
	if(count == 0){
		std::printf("Did not find keyword after initial sequence!\n");
		return context._stateErr;

	}else if (count == EOF){
		std::printf("Unexpected EOF\n");
		return context._stateErr;

	}else{
		std::printf("Found keyword: %s\n", context._cmdBuffer);
		return context._stateArgs;
	}
}

Engine::State& Engine::StateArgs::process(Engine& context, FILE* f)
{
	const int count = std::fscanf(f, "%s", context._argBuffer);
	if(count == 0){
		std::printf("Did not find args after keyword!\n");
		return context._stateErr;

	}else if (count == EOF){
		std::printf("Unexpected EOF\n");
		return context._stateErr;

	}else{
		std::printf("Found args: %s\n", context._argBuffer);
		return context._stateEnd;
	}
}

Engine::State& Engine::StateEnd::process(Engine& context, FILE* f)
{
	char buffer[3];
	const int count = std::fscanf(f, "%2s", buffer);
	if(count == 0){
		std::printf("Did not find end sequence after args!\n");
		return context._stateErr;

	}else if (count == EOF){
		std::printf("Unexpected EOF\n");
		return context._stateErr;

	}else{
		std::printf("Found end sequence at: %ld\n", std::ftell(f));
		return context._stateProcess;
	}
}

Engine::State& Engine::StateProcess::process(Engine& context, FILE* f)
{
	if(std::strcmp(context._cmdBuffer, "puti") == 0){
		std::printf("Executing PUT command\n");
		int arg;
		if(!context._getIntArg(&arg, std::string(context._argBuffer))){
			std::printf("Couldn't find arg with key: %s\n", context._argBuffer);
			return context._stateErr;
		}

		context._putStrToBuffer(std::to_string(arg));

	}else{
		std::printf("Unexpected keyword: %s\n", context._cmdBuffer);
		return context._stateErr;
	}

	return context._stateInitial;
}

Engine::State& Engine::StateErr::process(Engine& context, FILE* f)
{
	return context._stateErr;	
}