#include "fb_globals.hpp"

#include <memory>

#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "fb_main_state_manager.hpp"



using namespace fb;
using namespace global;



static std::unique_ptr<event::EventManager> _eventManager;
static std::unique_ptr<pins::PinManager> _pinManager;
static std::unique_ptr<state::StateManager> _stateManager;
static std::unique_ptr<box::Box> _flowerBox;



void global::init()
{
	assert(!_eventManager);
	_eventManager = std::make_unique<event::EventManager>();
	_pinManager = std::make_unique<pins::PinManager>();
	_stateManager = std::make_unique<state::MainStateManager>();

	_flowerBox = std::make_unique<box::Box>("TEST_NAME", "0.0.1", getUniqueId());
	_flowerBox->addProperty({"First prop", "Description", "Int", 0, 0, 0, 100, 30});
	_flowerBox->addSensor({"Sensor 1", "Description", "Int", 0, 1, 0, 100});
	_flowerBox->addSwitch({"Switch 1", "Description", 1, 2, true, {0}, {0}});
}

event::EventManager* global::getEventManager()
{
	return _eventManager.get();
}

pins::PinManager* global::getPinManager()
{
	return _pinManager.get();
}

state::StateManager* global::getStateManager()
{
	return _stateManager.get();
}

id::UniqueId global::getUniqueId()
{
	uint8_t mac[8] = {0};
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, 6 * 8));

	return *reinterpret_cast<id::UniqueId*>(mac);
}

box::Box* global::getFlowerBox()
{
	return _flowerBox.get();
}