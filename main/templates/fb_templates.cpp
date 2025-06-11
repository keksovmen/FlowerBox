#include "fb_templates.hpp"

#include <cstdio>
#include <cstring>



#define _KEYWORD_PUTI "puti"
#define _KEYWORD_PUTS "puts"
#define _KEYWORD_PUTA "puta"
#define _KEYWORD_BEGIN "begin"
#define _KEYWORD_END "end"
#define _KEYWORD_IF "if"
#define _KEYWORD_ELSEIF "elseif"
#define _KEYWORD_ELSE "else"
#define _KEYWORD_ENDIF "endif"



using namespace fb;
using namespace templates;



Argument::Argument(std::string_view val)
	: value(val)
{

}

Argument::Argument(const std::vector<std::string>& vals)
	: values(vals)
{

}




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
	_args.emplace(key, Argument(std::to_string(val)));

	return true;
}

bool Engine::addArgStr(const std::string val, const std::string& key)
{
	_args.emplace(key, Argument(val));

	return true;
}

bool Engine::addArgArray(const std::vector<std::string>& vals, const std::string& key)
{
	_args.emplace(key, Argument(vals));

	return true;
}

bool Engine::appendArgArray(const std::string& val, const std::string& key)
{
	if(!_args.contains(key)){
		_args.emplace(key, Argument(std::vector<std::string>{val}));
		return true;
	}

	_args.at(key).values.push_back(val);

	return true;
}


void Engine::_putToBuffer(char symbol)
{
	if(!_isWriteAllowed()){
		return;
	}

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

	*out = std::atoi(_args.at(key).value.c_str());
	return true;
}

std::optional<std::string> Engine::_getStrArg(std::string_view key)
{
	//decomposite key in to key and array index if exists
	auto begining = key.find("[");

	if(begining != std::string_view::npos){
		if(!key.find("]")){
			std::printf("Missing ] at argument specification: %s\n", key.begin());
			return {};
		}
		//we have array index
		// auto ending = key.find("]");
		std::string_view index(key.begin() + begining + 1);
		index = std::string_view(index.begin(), index.length() - 1);

		int i = 0;
		if(index == "i"){
			i = _loopI;
		}else{
			auto iter = _getStrArg(index);
			if(!iter){
				std::printf("Missing argument with key: %s\n", index.begin());
				return {};
			}

			i = std::atoi(iter.value().c_str());
		}

		return _getStrArrayArg(key.substr(0, begining), i);
	}


	auto k = std::string(key);
	if(!_args.contains(k)){
		return {};
	}

	return {_args.at(k).value};
}

std::optional<std::string> Engine::_getStrArrayArg(std::string_view key, int index)
{
	auto k = std::string(key);
	if(!_args.contains(k)){
		return {};
	}

	if(index >= _args.at(k).values.size() || index < 0){
		return {};
	}

	return {_args.at(k).values[index]};
}

bool Engine::_beginLoop(int count, FILE* f)
{
	if(_isInLoop()){
		return false;
	}

	_inLoop = true;
	_loopCount = count;
	_loopI = 0;
	_loopBeginIndex = std::ftell(f);

	return true;
}

bool Engine::_incrementLoop()
{
	_loopI++;

	return _loopI >= _loopCount;
}

bool Engine::_endLoop(FILE* f)
{
	if(!_isInLoop()){
		return false;
	}

	if(_loopI >= _loopCount){
		_inLoop = false;
		return true;
	}

	//move file position back
	std::fseek(f, _loopBeginIndex, SEEK_SET);

	return true;
}

bool Engine::_isInLoop()
{
	return _inLoop;
}

bool Engine::_isKeyword(std::string_view expected) const
{
	return expected == std::string_view(_cmdBuffer);
}

Engine::KeywordFamily Engine::_keywordFamily() const
{
	if(_isKeyword(_KEYWORD_BEGIN) || _isKeyword(_KEYWORD_END)){
		return KeywordFamily::LOOP;
	}

	if(_isKeyword(_KEYWORD_IF) || _isKeyword(_KEYWORD_ELSEIF) ||
		_isKeyword(_KEYWORD_ELSE) || _isKeyword(_KEYWORD_ENDIF))
	{
		return KeywordFamily::IF;
	}

	return KeywordFamily::PUT;
}

bool Engine::_isWriteAllowed() const
{
	if(!_inBranching){
		return true;
	}

	if(_isBranchingCompleted){
		return false;
	}

	return _isValidBranch;
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
		
		return _switchOnKeyword(context);
	}
}

Engine::State& Engine::StateKeyword::_switchOnKeyword(Engine& context)
{
	context._currentArg = 0;
	context._expectedArgs = 0;

	if(context._isKeyword(_KEYWORD_END) || context._isKeyword(_KEYWORD_ENDIF) || context._isKeyword(_KEYWORD_ELSE)){
		//no argument cmds
		context._expectedArgs = 0;
		// return context._stateEnd;

	}else if(context._isKeyword(_KEYWORD_IF) || context._isKeyword(_KEYWORD_ELSEIF)){
		//3 arguments cmds
		context._expectedArgs = 3;
	}else if(false){
		//2 arguments cmds
		context._expectedArgs = 2;

	}else{
		//1 argument cmds
		context._expectedArgs = 1;
	}

	return context._stateArgs;
}

Engine::State& Engine::StateArgs::process(Engine& context, FILE* f)
{
	if(context._expectedArgs == 0){
		return context._stateEnd;
	}

	if(context._currentArg == context._expectedArgs){
		return context._stateEnd;
	}

	const int count = std::fscanf(f, "%s", context._argBuffer[context._currentArg]);
	if(count == 0){
		std::printf("Did not find args after keyword!\n");

		return context._stateEnd;

	}else if (count == EOF){
		std::printf("Unexpected EOF\n");

		return context._stateErr;

	}else{
		std::printf("Found args: %s\n", context._argBuffer[context._currentArg]);
		context._currentArg++;

		return context._stateArgs;
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
	switch(context._keywordFamily())
	{
		case KeywordFamily::PUT:
			return _processPutCmds(context, f);
		
		case KeywordFamily::LOOP:
			return _processLoopCmds(context, f);
		
		case KeywordFamily::IF:
			return _processIfCmds(context, f);
		
		default:
			std::printf("Unexpected keyword: %s\n", context._cmdBuffer);
			return context._stateErr;
	}
}

Engine::State& Engine::StateProcess::_processPutCmds(Engine& context, FILE* f)
{
	if(context._isKeyword(_KEYWORD_PUTI)){
		std::printf("Executing PUT command\n");
		int arg;
		if(!context._getIntArg(&arg, std::string(context._argBuffer[0]))){
			std::printf("Couldn't find arg with key: %s\n", context._argBuffer[0]);
			return context._stateErr;
		}

		context._putStrToBuffer(std::to_string(arg));

	}else if(context._isKeyword(_KEYWORD_PUTS)){
		std::printf("Executing PUT_S command\n");

		auto arg = context._getStrArg(context._argBuffer[0]);
		if(!arg){
			std::printf("Couldn't find arg with key: %s\n", context._argBuffer[0]);
			return context._stateErr;
		}

		context._putStrToBuffer(arg.value());

	}else if(context._isKeyword(_KEYWORD_PUTA)){
		std::printf("Executing PUT_A command\n");

		auto arg = context._getStrArg(context._argBuffer[0]);
		if(!arg){
			std::printf("Couldn't find arg with key: %s\n", context._argBuffer[0]);
			return context._stateErr;
		}

		context._putStrToBuffer(arg.value());

	}else{
		std::printf("Unexpected keyword: %s\n", context._cmdBuffer);
		return context._stateErr;
	}

	return context._stateInitial;
}

Engine::State& Engine::StateProcess::_processLoopCmds(Engine& context, FILE* f)
{
	if(context._isKeyword(_KEYWORD_BEGIN)){
		std::printf("Executing BEGIN command\n");

		auto arg = context._getStrArg(context._argBuffer[0]);
		if(!arg){
			std::printf("Couldn't find arg with key: %s\n", context._argBuffer[0]);
			return context._stateErr;
		}

		if(!context._beginLoop(std::atoi(arg.value().c_str()), f)){
			std::printf("Couldn't start loop with arg: %s\n", arg.value().c_str());
			return context._stateErr;
		}

	}else if(context._isKeyword(_KEYWORD_END)){
		std::printf("Executing END command\n");

		if(!context._isInLoop()){
			std::printf("Not in a loop\n");
			return context._stateErr;
		}

		if(context._incrementLoop()){
			std::printf("Loop count is reached\n");
		}

		if(!context._endLoop(f)){
			std::printf("Couldn't end loop with arg\n");
			return context._stateErr;
		}
	}else{
		std::printf("Unexpected keyword: %s\n", context._cmdBuffer);
		return context._stateErr;
	}

	return context._stateInitial;
}

Engine::State& Engine::StateProcess::_processIfCmds(Engine& context, FILE* f)
{
	if(!context._isBranchingCompleted){
		context._isBranchingCompleted = context._isValidBranch;
	}

	if(context._isKeyword(_KEYWORD_IF)){
		if(context._inBranching){
			std::printf("Already in branching mode\n");
			return context._stateErr;

		}else if(context._currentArg != 3){
			std::printf("Must be 3 arguments not: %d\n", context._currentArg);
			return context._stateErr;
		}

		//start branching
		context._inBranching = true;
		context._isBranchingCompleted = false;

		if(!_processBranching(context)){
			return context._stateErr;
		}
		

	}else if(context._isKeyword(_KEYWORD_ELSEIF)){
		if(!context._inBranching){
			std::printf("Not in branching mode\n");
			return context._stateErr;

		}else if(context._currentArg != 3){
			std::printf("Must be 3 arguments not: %d\n", context._currentArg);
			return context._stateErr;
		}

		if(!context._isBranchingCompleted && !_processBranching(context)){
			return context._stateErr;
		}
	}else if(context._isKeyword(_KEYWORD_ELSE)){
		if(!context._inBranching){
			std::printf("Not in branching mode\n");
			return context._stateErr;
		}

		context._isValidBranch = !context._isBranchingCompleted;

	}else if(context._isKeyword(_KEYWORD_ENDIF)){
		if(!context._inBranching){
			std::printf("Not in branching mode\n");
			return context._stateErr;
		}

		context._inBranching = false;
	}
	return context._stateInitial;
}

bool Engine::StateProcess::_processBranching(Engine& context)
{
	//evaluate expression
	auto argword = context._getStrArg(context._argBuffer[0]);
	if(!argword){
		std::printf("Missing argument with key: %s\n", context._argBuffer[0]);
		return false;
	}

	int arg1 = std::atoi(argword.value().c_str());
	std::string_view op = context._argBuffer[1];
	int arg2 = std::atoi(context._argBuffer[2]);

	if(op == "=="){
		context._isValidBranch = arg1 == arg2;
		std::printf("Compared result %d == %d = %d\n", arg1, arg2, context._isValidBranch);

	}else if(op == "!="){
		context._isValidBranch = arg1 != arg2;
		std::printf("Compared result %d != %d = %d\n", arg1, arg2, context._isValidBranch);

	}else{
		std::printf("Unexpected operator: %s\n", op.begin());
		return false;
	}

	return true;
}

Engine::State& Engine::StateErr::process(Engine& context, FILE* f)
{
	return context._stateErr;	
}