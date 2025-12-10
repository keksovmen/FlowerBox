#include "fb_dmx_light_maper_obj.hpp"

#include <ranges>

#include "fb_dmx_light_box_obj.hpp"
#include "fb_dmx_light_hw_obj.hpp"
#include "fb_globals.hpp"
#include "fb_settings.hpp"
#include "fb_switch.hpp"



using namespace fb;
using namespace project;



static box::Switch _boxRgbSwitch(box::Tid::SWITCH_RGB,
	[](){return getHwRgbSwitch().isOn();});



static int _value_to_rgbw_gradient(int value) {
	// Ограничиваем значение в диапазоне 0-100
	if (value < 0) value = 0;
	if (value > 100) value = 100;
	
	uint8_t r, g, b = 0, w = 0;
	// Вычисляем компоненты R и G
	if (value <= 50) {
		// Красный → Жёлтый (R=255, G растёт от 0 до 255)
		float ratio = (float)value / 50.0f;
		r = 255;
		g = (uint8_t)(255 * ratio);
	} else {
		// Жёлтый → Зелёный (R уменьшается от 255 до 0, G=255)
		float ratio = (float)(value - 50) / 50.0f;
		r = (uint8_t)(255 * (1.0f - ratio));
		g = 255;
	}
	
	// Формируем 32-битное значение WRGB
	return (w << 24) | (r << 16) | (g << 8) | b;
}

static int _rgbw_to_value_gradient(int rgbw) {
	// Извлекаем компоненты цвета
	// uint8_t w = (rgbw >> 24) & 0xFF;
	uint8_t r = (rgbw >> 16) & 0xFF;
	uint8_t g = (rgbw >> 8) & 0xFF;
	// uint8_t b = rgbw & 0xFF;
	
	if (r == 255 && g == 0) return 0;    // Чистый красный
	if (r == 255 && g == 255) return 50; // Чистый жёлтый
	if (r == 0 && g == 255) return 100;  // Чистый зелёный

	// Если больше зелёного (G > R → жёлтый-зелёная зона)
	if (g > r) {
		float ratio = (float)g / 255.0f;
		return 50 + (int)(50.0f * ratio); // 50-100
	}else {
	// Иначе красный-жёлтая зона
		float ratio = (float)g / 255.0f;
		return (int)(50.0f * ratio); // 0-50
	}
}



static void _initRgbSwitch()
{
	fb::project::util::createAndRegisterForceProperty(getHwRgbSwitch(), _boxRgbSwitch);
	auto* colorProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_RGB_VALUE,
		[](int val){
			getHwRgbSwitch().setColor(val);
			return true;
		}, getHwRgbSwitch().getColor()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(colorProperty));
	_boxRgbSwitch.addPropertyDependency(colorProperty->getId());

	auto* gradientProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_RGB_GRADIENT_MAFIA,
		[](int val){
			getHwRgbSwitch().setColor(_value_to_rgbw_gradient(val));
			return true;
		}, _rgbw_to_value_gradient(getHwRgbSwitch().getColor())
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(gradientProperty));
	_boxRgbSwitch.addPropertyDependency(gradientProperty->getId());


	auto* dmxAddressProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_DMX_ADDRESS,
		[](int val){
			getHwRgbSwitch().setDmxAddress(val);
			return true;
		}, getHwRgbSwitch().getDmxAddress()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(dmxAddressProperty));
	_boxRgbSwitch.addPropertyDependency(dmxAddressProperty->getId());
}

static void _initMp3Sensor()
{
	//must be write only
	auto* playProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_MP3_PLAY,
		[](int val){
			return getHwMp3Sensor().play(val);
		}, 0
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(playProperty));
	getBoxMp3Sensor().addPropertyDependency(playProperty->getId());


	auto* stopProperty = new box::PropertyNone(
		box::Tid::PROPERTY_SWITCH_MP3_STOP,
		[](std::string val){
			getHwMp3Sensor().stop();
			return true;
		});

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(stopProperty));
	getBoxMp3Sensor().addPropertyDependency(stopProperty->getId());


	auto* volumeProperty = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_MP3_VOLUME,
		[](int val){
			return getHwMp3Sensor().setVolume(val);
		}, getHwMp3Sensor().getVolume()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(volumeProperty));
	getBoxMp3Sensor().addPropertyDependency(volumeProperty->getId());


	auto* volumeLoop = new box::PropertyInt(box::Tid::PROPERTY_SWITCH_MP3_LOOP,
		[](int val){
			return getHwMp3Sensor().setLoop(static_cast<bool>(val));
		}, getHwMp3Sensor().isLooping()
	);

	getBox().addProperty(std::unique_ptr<box::PropertyIface>(volumeLoop));
	getBoxMp3Sensor().addPropertyDependency(volumeLoop->getId());
}



void project::initMaperObjs()
{
	getBox().addSwitch(&_boxRgbSwitch);


	_initRgbSwitch();
	_initMp3Sensor();
	util::createAndRegisterDefaultBoxProperties();
}

int project::mapBoxSensorIdToAddres(int id)
{
	if(id == getBoxMp3Sensor().getId()){
		return reinterpret_cast<int>(&getHwMp3Sensor());
	}

	assert(0);
}

int project::mapBoxSwitchIdToAddres(int id)
{
	if(id == _boxRgbSwitch.getId())
	{
		return reinterpret_cast<int>(&_boxRgbSwitch);
	}

	assert(0);
}

box::PropertyIface& project::getRgbProperty()
{
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(1));
}

box::PropertyIface& project::getRgbSwitchProperty()
{
	return *getBox().getProperty(_boxRgbSwitch.getPropertyDependencies().at(0));
}

box::PropertyIface& project::getMp3PlayProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(0));
}

box::PropertyIface& project::getMp3StopProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(1));
}

box::PropertyIface& project::getMp3ValueProperty()
{
	return *getBox().getProperty(getBoxMp3Sensor().getPropertyDependencies().at(2));
}
