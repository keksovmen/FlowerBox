#include "fb_channel_handler.hpp"



using namespace fb;
using namespace util;



ChannelHandler::ChannelHandler(Consumer consumer)
	: _consumer(consumer)
{

}

const char* ChannelHandler::getName() const
{
	return "ChannelHandler";
}

void ChannelHandler::tick()
{
	uint8_t buff[512] = {0};
	int i = 0;
	for(auto& c : _channels){
		const uint8_t val = c.function(c.state);
		buff[i++] = val;

		if(c.state.increment()){
			//change to constant state
			if(c.state.cycle){
				c.function = (c.function == _asendValueF) ? _desandValueF : _asendValueF;
			}else{
				c.function = _constValueF;
				c.state.currentValue = val;
			}
		}
	}

	std::invoke(_consumer, std::span<uint8_t>(buff, sizeof(buff)));
}

void ChannelHandler::setChannelConst(int channel, uint8_t val)
{
	if(channel < 0 || channel > 512){
		FB_DEBUG_LOG_E_OBJ("Illegal channel: %d", channel);
		return;
	}

	_channels[channel].function = _constValueF;
	_channels[channel].state.currentValue = val;
}

void ChannelHandler::setChannelDynamic(int channel, uint8_t from, uint8_t to, uint32_t periodTicks, bool cycle)
{
	if(channel < 0 || channel > 512){
		FB_DEBUG_LOG_E_OBJ("Illegal channel: %d", channel);
		return;
	}

	if(periodTicks <= 1){
		FB_DEBUG_LOG_E_OBJ("Illegal period: %u", periodTicks);
		return;
	}

	_channels[channel].state.currentValue = to;	//for const case when to == from, set value to any
	_channels[channel].state.minValue = std::min(to, from);
	_channels[channel].state.maxValue = std::max(to, from);
	_channels[channel].state.cycle = cycle;
	_channels[channel].state.currentTick = 0;
	_channels[channel].state.periodTicks = periodTicks;
	_channels[channel].function = _getDirectionFunction(to - from);
}

bool ChannelHandler::State::increment(){
	currentTick++;
	if(currentTick > periodTicks){
		currentTick = 0;
		return true;
	}

	return false;
}

uint8_t ChannelHandler::_constValueF(const State& state)
{
	return state.currentValue;
}

uint8_t ChannelHandler::_triangleValueF(const State& state)
{
	const uint32_t range = state.maxValue - state.minValue;
	const uint32_t halfPeriod = state.periodTicks / 2;
	//second half time when light is dimming
	if(state.currentTick >= halfPeriod){
		//light fades during T/2
		const uint32_t currentPosition = ((state.currentTick - halfPeriod) * 1000) / halfPeriod;
		const uint8_t result = state.maxValue - ((range * currentPosition) / 1000);
		return result;

	}else{
		//light grows during T/2
		const uint32_t currentPosition = (state.currentTick * 1000) / halfPeriod;
		const uint8_t result = state.minValue + ((range * currentPosition) / 1000);
		return result;
	}
}

uint8_t ChannelHandler::_asendValueF(const State& state)
{
	const uint32_t range = state.maxValue - state.minValue;
	const uint32_t currentPosition = (state.currentTick * 1000) / state.periodTicks;
	
	return state.minValue + ((range * currentPosition) / 1000);
}

uint8_t ChannelHandler::_desandValueF(const State& state)
{
	const uint32_t range = state.maxValue - state.minValue;
	const uint32_t currentPosition = (state.currentTick * 1000) / state.periodTicks;

	return state.maxValue - ((range * currentPosition) / 1000);
}

ChannelHandler::LogicFunction ChannelHandler::_getDirectionFunction(int delta)
{
	if(delta > 0){
		return _asendValueF;
	}else if(delta < 0){
		return _desandValueF;
	}else{
		return _constValueF;
	}
}