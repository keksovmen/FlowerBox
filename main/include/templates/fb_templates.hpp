#pragma once

#include <functional>
#include <optional>
#include <string_view>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



namespace fb
{
	namespace templates
	{
		struct Argument
		{
			public:
				std::string value;
				std::vector<std::string> values;



				Argument(std::string_view val);
				Argument(const std::vector<std::string>& vals);

				// Argument& operator=(Argument&& rhs) = default;
				// Argument(const Argument& rhs) = default;

				int getAsInt() const;
				int getAsInt(int index) const;
		};



		class Engine
		{
			public:
				using DataConsumerCb = std::function<void(const char* data, int size)>;

				Engine(char* buffer, int bufferSize, const DataConsumerCb& onDataConsume);

				bool process(const std::string& fileName);

				bool addIntArg(int val, const std::string& key);
				bool addArgStr(const std::string val, const std::string& key);
				bool addArgArray(const std::vector<std::string>& vals, const std::string& key);
				bool appendArgArray(const std::string& val, const std::string& key);

			private:
				class State
				{
					public:
						virtual State& process(Engine& context, FILE* f) = 0;
				};

				class StateInitial : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateStart : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateKeyword : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateArgs : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateEnd : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateProcess : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};

				class StateErr : public State
				{
					public:
						virtual State& process(Engine& context, FILE* f) override;
				};



				void _putToBuffer(char symbol);
				void _putStrToBuffer(const std::string& str);
				void _flush();
				bool _getIntArg(int* out, const std::string& key);
				std::optional<std::string> _getStrArg(std::string_view key);
				std::optional<std::string> _getStrArrayArg(std::string_view key, int index);

				bool _beginLoop(int count, FILE* f);
				bool _incrementLoop();	//return true if count is reached
				bool _endLoop(FILE* f);
				bool _isInLoop();
			
			private:
				char* _buffer;
				const int _bufferSize;
				DataConsumerCb _dataConsumer;

				char _cmdBuffer[32];
				char _argBuffer[2][32];
				int _currentArg = 0;
				int _currentBuffer = 0;

				//for cycles
				int _loopBeginIndex = 0;
				int _loopI = 0;
				int _loopCount = 0;
				bool _inLoop = false;

				//TODO: make it work in provided buffer not in dynamic memory
				std::unordered_map<std::string, Argument> _args;

				StateInitial _stateInitial;
				StateStart _stateStart;
				StateKeyword _stateKeyword;
				StateArgs _stateArgs;
				StateEnd _stateEnd;
				StateProcess _stateProcess;
				StateErr _stateErr;
		};
	}
}