#include "fb_df_player.hpp"

#include <utility>



using namespace fb;
using namespace player;



Packet::Packet(Cmd cmd)
{
	_defaultInitData();
	setCmd(cmd);
}

Packet::Packet(Cmd cmd, uint16_t arg)
{
	_defaultInitData();
	setCmd(cmd);
	setArg(arg);
}

Packet::Packet(std::span<uint8_t, PACKET_SIZE> data)
{
	std::copy(data.begin(), data.end(), _data.data());
}

Packet& Packet::setCmd(Cmd cmd)
{
	_data[3] = std::to_underlying(cmd);

	return *this;
}

Packet& Packet::setArg(uint16_t arg)
{
	_data[5] = arg >> 8;
	_data[6] = arg;

	return *this;
}

Packet& Packet::calculateCRC()
{
	const uint16_t crc = _calculateCrc();
	_data[7] = crc >> 8;
	_data[8] = crc;

	return *this;
}

Packet::Buffer& Packet::getRaw()
{
	return _data;
}

const Packet::Buffer& Packet::getRaw() const
{
	return _data;
}

uint8_t Packet::getCmd() const
{
	return _data[3];
}

uint16_t Packet::getArg() const
{
	return (uint16_t)(_data[5] << 8) + _data[6];
}

bool Packet::checkCRC() const
{
	return _calculateCrc() == ((_data[7] << 8) + _data[8]);
}

void Packet::_defaultInitData()
{
	_data = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
}

uint16_t Packet::_calculateCrc() const
{
	uint16_t sum = 0;
	for (int i = 1; i < 7; i++)
	{
		sum += _data[i];
	}
	return -sum;
}



DfPlayer::DfPlayer(ReadCb read, WriteCb write)
	: _readCb(read), _writeCb(write)
{

}

const char* DfPlayer::getName() const
{
	return "DfPlayer";
}

std::optional<int> DfPlayer::readTotalFiles()
{
	Packet r(Cmd::GET_SD_FILE_COUNT);
	if(!_writeCb(r.calculateCRC().getRaw())){
		FB_DEBUG_LOG_E_OBJ("readTotalFiles() failed to send request");
		return {};
	}

	Packet::Buffer buff;
	if(!_readCb(buff, 1000)){
		FB_DEBUG_LOG_E_OBJ("readTotalFiles() failed read responce");
		return {};
	}

	Packet p(buff);
	FB_DEBUG_LOG_I_OBJ("readTotalFiles() = %d", p.getArg());

	if(!r.checkCRC()){
		FB_DEBUG_LOG_E_OBJ("readTotalFiles() failed to check CRC");
		return {};
	}
	
	return {p.getArg()};
}

std::optional<int> DfPlayer::readVolume()
{
	Packet r(Cmd::GET_VOLUME);
	if(!_writeCb(r.calculateCRC().getRaw())){
		FB_DEBUG_LOG_E_OBJ("readVolume() failed to send request");
		return {};
	}

	Packet::Buffer buff;
	if(!_readCb(buff, 1000)){
		FB_DEBUG_LOG_E_OBJ("readVolume() failed read responce");
		return {};
	}

	Packet p(buff);
	FB_DEBUG_LOG_I_OBJ("readVolume() = %d", p.getArg());

	if(!r.checkCRC()){
		FB_DEBUG_LOG_E_OBJ("readVolume() failed to check CRC");
		return {};
	}
	
	return {p.getArg()};
}

bool DfPlayer::writeVolume(int volume)
{
	FB_DEBUG_LOG_I_OBJ("writeVolume(%d)", volume);

	if(volume < 0 || volume > 30){
		FB_DEBUG_LOG_E_OBJ("Write volume illegal value %d", volume);
		return false;
	}

	Packet p(Cmd::SET_VOLUME, volume);
	_writeCb(p.calculateCRC().getRaw());

	return true;
}

bool DfPlayer::writePlay(int fileId)
{
	FB_DEBUG_LOG_I_OBJ("writePlay(%d)", fileId);

	if(fileId < 0){
		FB_DEBUG_LOG_E_OBJ("Write play illegal value %d", fileId);
		return false;
	}

	Packet p(Cmd::PLAY_TRACK, fileId);
	_writeCb(p.calculateCRC().getRaw());

	return true;
}

bool DfPlayer::writeStop()
{
	Packet p(Cmd::STOP_AUDIO);
	_writeCb(p.calculateCRC().getRaw());

	return true;
}

