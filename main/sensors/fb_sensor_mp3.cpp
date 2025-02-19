#include "fb_sensor_mp3.hpp"

#include "DFRobotDFPlayerMini.h"



using namespace fb;
using namespace sensor;



Mp3Sensor::Mp3Sensor(int port, int rxPin, int txPin)
	: _uart(static_cast<uart_port_t>(port), rxPin, txPin, 9600),
	// _player([this](auto span, auto timeoutMs){
	// 	const bool result = _uart.read(span, timeoutMs);
	// 	FB_DEBUG_LOG_W_OBJ("READ: 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X",
	// 		span[0], span[1], span[2], span[3], span[4], span[5], span[6], span[7], span[8], span[9]);
	// 	return result;
	// }, [this](auto span){
	// 	FB_DEBUG_LOG_W_OBJ("WRITE: 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X",
	// 		span[0], span[1], span[2], span[3], span[4], span[5], span[6], span[7], span[8], span[9]);
	// 	return _uart.write(span);
	// })
	_player(std::bind(&interfaces::Uart::read, &_uart, std::placeholders::_1, std::placeholders::_2),
		std::bind(&interfaces::Uart::write, &_uart, std::placeholders::_1))
{

}

const char* Mp3Sensor::getName() const
{
	return "Mp3Sensor";
}

bool Mp3Sensor::play(int fileId)
{
	FB_DEBUG_ENTER_W_OBJ();

	FB_DEBUG_LOG_I_OBJ("Play audio(%d)", fileId);

	if(!isInit()){
		return false;
	}

	if(fileId < 0 || fileId > getFilesCount()){
		return false;
	}

	return _player.writePlay(fileId);
}

void Mp3Sensor::stop()
{
	if(!isInit()){
		return;
	}
	
	FB_DEBUG_LOG_I_OBJ("STOP audio");

	_player.writeStop();
}

bool Mp3Sensor::setVolume(int volume)
{
	FB_DEBUG_LOG_I_OBJ("Set volume(%d)", volume);

	if(!isInit()){
		return false;
	}

	if(volume < 0 || volume > 30){
		return false;
	}

	const bool result = _player.writeVolume(volume);
	if(result){
		_volume = volume;
	}

	return result;
}

int Mp3Sensor::getFilesCount() const
{
	return _filesCount;
}

int Mp3Sensor::getVolume() const
{
	return _volume;
}

bool Mp3Sensor::_doInit()
{
	const auto files = _player.readTotalFiles();
	if(!files){
		FB_DEBUG_LOG_E_OBJ("Failed to read total files");
		return false;
	}
	_filesCount = files.value();

	const auto volume = _player.readVolume();
	if(!volume){
		FB_DEBUG_LOG_E_OBJ("Failed to read volume");
		return false;
	}

	_volume = volume.value();

	return true;
}

SensorIface::UpdateResult Mp3Sensor::_doUpdate()
{
	// while(DF_available()){
		//TODO: add calback in to handle message function of DF API
		// DF_parseStack();
		// const uint8_t cmd = DF_readCommand();
		// const uint16_t arg = DF_read();

		// FB_DEBUG_LOG_I_OBJ("Find cmd(arg) = %u(%u)", cmd, arg);

		//DO something with it
	// }

	return UpdateResult::OK;
}
