#pragma once

#include <string>
#include <functional>
#include <utility>
#include <unordered_map>



namespace fb
{
	namespace templates
	{
		class Engine
		{
			public:
				using DataConsumerCb = std::function<void(const char* data, int size)>;

				Engine(char* buffer, int bufferSize, const DataConsumerCb& onDataConsume);

				bool process(const std::string& fileName);

				bool addIntArg(int val, const std::string& key);

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
			
			private:
				char* _buffer;
				const int _bufferSize;
				DataConsumerCb _dataConsumer;

				char _cmdBuffer[32];
				char _argBuffer[32];
				int _currentBuffer = 0;

				//TODO: make it work in provided buffer not in dynamic memory
				std::unordered_map<std::string, int> _args;

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