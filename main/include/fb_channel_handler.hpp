#pragma once



#include <functional>
#include <span>

#include "fb_debug.hpp"
#include "fb_lock_wrapper.hpp"



namespace fb
{
	namespace util
	{
		struct BaseEntry
		{
			uint8_t val;
			uint16_t timeTicks;

			BaseEntry(uint8_t v, uint32_t timeMs);
		};

		struct RepeatEntry
		{
			static constexpr int16_t INFINITE = -1;

			uint8_t from;
			uint8_t to;
			int16_t repeatTimes;
			int16_t counter = 0;
		};

		struct ChannelState
		{
			uint16_t channelIndex;

			uint8_t index = 0;
			uint32_t tickCount = 0;
		};

		class ChannelEntry
		{
			public:
				ChannelState state;
				std::vector<BaseEntry> baseEntries;
				std::vector<RepeatEntry> repeatEntries;

				ChannelEntry(uint16_t channel, std::vector<BaseEntry>&& bEntries, std::vector<RepeatEntry>&& rEntries);

				bool operator==(const ChannelEntry& e);
		};



		class SceneryHandler : public debug::Named
		{
			public:
				using Consumer = std::function<void(uint16_t index, uint8_t val)>;

				static constexpr int TICK_VALUE_MS = 20;



				SceneryHandler(Consumer consumer);
				void init();

				virtual const char* getName() const override;

				//must be called 50 times per second or each TICK_VALUE_MS
				void tick();

				void addScenery(uint16_t channel, std::vector<BaseEntry> baseEntries, std::vector<RepeatEntry> repeatEntries);
				void clearAll();
			
			private:
				const Consumer _consumer;

				std::vector<ChannelEntry> _entries;
				SemaphoreHandle_t _mutex = nullptr;


				
				int _findRepeatPoint(ChannelEntry& entry, int index) const;
				uint8_t _calculatePosition(int from, int to, uint32_t periodTicks, uint32_t currentTicks);
		};



		class ChannelHandler : public debug::Named
		{
			public:
				using Consumer = std::function<void(std::span<uint8_t>)>;



				ChannelHandler(Consumer consumer);

				virtual const char* getName() const override;

				void tick();
				void setChannelConst(int channel, uint8_t val);
				void setChannelDynamic(int channel, uint8_t from, uint8_t to, uint32_t periodTicks, bool cycle);

			private:
				struct State
				{
					uint8_t currentValue = 0;
					uint8_t minValue = 0;
					uint8_t maxValue = 255;
					bool cycle = false;
					uint32_t currentTick = 0;
					uint32_t periodTicks = 0;

					//return true if goes past the periodTicks
					bool increment();
				};



				using LogicFunction = uint8_t(*)(const State& state);



				struct Channel
				{
					State state;
					LogicFunction function = _constValueF;
				};
			
			private:
				const Consumer _consumer;
				Channel _channels[512];



				static uint8_t _constValueF(const State& state);
				static uint8_t _triangleValueF(const State& state);
				static uint8_t _asendValueF(const State& state);
				static uint8_t _desandValueF(const State& state);
				static LogicFunction _getDirectionFunction(int delta);
		};
	}
}