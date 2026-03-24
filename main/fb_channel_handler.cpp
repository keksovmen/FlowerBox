#include "fb_channel_handler.hpp"

#include <algorithm>



using namespace fb;
using namespace util;



BaseEntry::BaseEntry(uint8_t v, uint32_t timeMs)
	: val(v), timeTicks(timeMs / SceneryHandler::TICK_VALUE_MS)
{

}



ChannelEntry::ChannelEntry(uint16_t channel, std::vector<BaseEntry>&& bEntries, std::vector<RepeatEntry>&& rEntries)
	: state{channel}, baseEntries(std::move(bEntries)), repeatEntries(std::move(rEntries))
{

}

bool ChannelEntry::operator==(const ChannelEntry& e)
{
	return this->state.channelIndex == e.state.channelIndex;
}



SceneryHandler::SceneryHandler(Consumer consumer)
	: _consumer(consumer)
{

}

void SceneryHandler::init()
{
	_mutex = xSemaphoreCreateMutex();
	assert(_mutex);
}

const char* SceneryHandler::getName() const
{
	return "SceneryHandler";
}

void SceneryHandler::tick()
{
	LockWrapper lock(_mutex, portMAX_DELAY);
	
	std::vector<int> removeList;

	for(int i = 0; i < _entries.size(); i++)
	{
		ChannelEntry& entry = _entries[i];

		//check if there is no further go
		if(entry.state.index >= entry.baseEntries.size()){
			//remove this entry from the list and continue
			removeList.push_back(i);
			continue;
		}

		//send current value here could use switch to use different interpolation instead of linear
		_consumer(entry.state.channelIndex,
			_calculatePosition(entry.baseEntries[(entry.state.index == 0) ? 0 : entry.state.index - 1].val,
				entry.baseEntries[entry.state.index].val,
				entry.baseEntries[entry.state.index].timeTicks,
				entry.state.tickCount));
		
		//increment tick count and check if it is overflowed for next steps
		entry.state.tickCount++;
		if(entry.state.tickCount <= entry.baseEntries[entry.state.index].timeTicks){
			continue;
		}

		//reset tick count for next entry
		entry.state.tickCount = 0;

		//check if this is repeat point and we should go back
		const int repeatIndex = _findRepeatPoint(entry, entry.state.index);
		if(repeatIndex == -1){
			//there is no repeat for this node
			entry.state.index++;
		}else{
			//we have repeat point
			if(entry.repeatEntries[repeatIndex].repeatTimes == RepeatEntry::INFINITE){
				//this point has no counter, so loop is infinite
				//set index to to and continue
				entry.state.index = entry.repeatEntries[repeatIndex].to;
				continue;
			}

			//check if counter is reached the goal
			if(entry.repeatEntries[repeatIndex].repeatTimes == (entry.repeatEntries[repeatIndex].counter + 1)){
				//it reached the goal, so proceed to next node
				entry.state.index++;
				//set to zero in case if there is greater repeat loop that will repeat this loop
				entry.repeatEntries[repeatIndex].counter = 0;
			}else{
				//not reached the goal so increment counter
				entry.repeatEntries[repeatIndex].counter++;
				//and set current index to given
				entry.state.index = entry.repeatEntries[repeatIndex].to;
			}
		}
	}

	//remove, starting from back
	std::reverse(removeList.begin(), removeList.end());
	for(int index : removeList){
		_entries.erase(_entries.begin() + index);
		// FB_DEBUG_LOG_I_OBJ("Finished with %d", index);
	}
}

void SceneryHandler::addScenery(uint16_t channel, std::vector<BaseEntry> baseEntries, std::vector<RepeatEntry> repeatEntries)
{
	LockWrapper wrapper(_mutex, portMAX_DELAY);

	//first find if such channel is already exists if so replace
	auto iter = std::find_if(_entries.begin(), _entries.end(), [channel](const ChannelEntry& e){return e.state.channelIndex == channel;});
	if(iter == _entries.end()){
		//push back
		_entries.push_back(ChannelEntry{channel, std::move(baseEntries), std::move(repeatEntries)});
		FB_DEBUG_LOG_I_OBJ("Added scenery for %d", channel);
	}else{
		//replace
		const int index = iter - _entries.begin();
		_entries[index] = ChannelEntry{channel, std::move(baseEntries), std::move(repeatEntries)};
		FB_DEBUG_LOG_I_OBJ("Replaced scenery for %d", channel);
	}

}


int SceneryHandler::_findRepeatPoint(ChannelEntry& entry, int index) const
{
	auto iter = std::find_if(entry.repeatEntries.begin(), entry.repeatEntries.end(), [index](const RepeatEntry& rep){return rep.from == index;});
	if(iter == entry.repeatEntries.end()){
		return -1;
	}

	//must return index of the found value
	return iter - entry.repeatEntries.begin();
}

uint8_t SceneryHandler::_calculatePosition(int from, int to, uint32_t periodTicks, uint32_t currentTicks)
{
	// case for immediate values, often it is the first value in animation array
	if(periodTicks == 0){
		return from;
	}

	const int direction = to - from;

	const int currentPosition = (currentTicks * 1000) / periodTicks;
	//calculation must be signed and only at the last moment converted and truncated to the result type
	const uint8_t result = from + ((direction * currentPosition) / 1000);
	return result;
}



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