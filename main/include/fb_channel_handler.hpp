#pragma once



#include <functional>
#include <span>

#include "fb_debug.hpp"



namespace fb
{
	namespace util
	{
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