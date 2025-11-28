#include "fb_battery.hpp"



using namespace fb;
using namespace adc;



Battery::Battery(AdcPin& pin)
	: _adc(pin)
{

}

int Battery::readCharge()
{
	const int raw = _adc.readRaw(100);
	int percents = ((raw - _minRaw) * 100) / (_maxRaw - _minRaw);
	percents = percents < 0 ? 0 : percents;
	percents = percents > 100 ? 100 : percents;

	return percents;
}

void Battery::setMaxRaw(int raw)
{
	_maxRaw = raw;
}

void Battery::setMinRaw(int raw)
{
	_minRaw = raw;
}
