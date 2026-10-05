#pragma once



#include <array>
#include <functional>
#include <optional>
#include <span>

#include "fb_clock.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace player
	{
		enum class Cmd : uint8_t
		{
			PLAY_TRACK = 0x03,
			SET_VOLUME = 0x06,
			SET_LOOP_MODE = 0x08,
			SET_PLAYBACK_MODE = 0x09,
			RESET = 0x0C,
			STOP_AUDIO = 0x16,
			GET_VOLUME = 0x43,
			GET_SD_FILE_COUNT = 0x48,
		};



		class Packet
		{
			public:
				static constexpr int PACKET_SIZE = 10;



				using Buffer = std::array<uint8_t, PACKET_SIZE>;



				Packet(Cmd cmd);
				Packet(Cmd cmd, uint16_t arg);
				Packet(std::span<uint8_t, PACKET_SIZE> data);

				Packet& setCmd(Cmd cmd);
				Packet& setArg(uint16_t arg);
				Packet& calculateCRC();

				Buffer& getRaw();
				const Buffer& getRaw() const;
				uint8_t getCmd() const;
				uint16_t getArg() const;
				bool checkCRC() const;



			private:
				Buffer _data;


				void _defaultInitData();
				uint16_t _calculateCrc() const;
		};



		class DfPlayer : public debug::Named
		{
			public:
				/**
				 * @brief read specified amount of bytes
				 *  
				 * @param out read in to here and specified amount
				 * @param timeoutMs time to wait for amount
				 * 
				 * @return true if read count
				 * 			false failed to read for given time
				 */
				using ReadCb = std::function<bool(std::span<uint8_t> out, int timeoutMs)>;

				/**
				 * @brief writes data bytes
				 * 
				 * @param data bytes to write
				 * 
				 * @return true if write successeded
				 * 			false some problems occured
				 */

				using WriteCb = std::function<bool(std::span<uint8_t> data)>;



				DfPlayer(ReadCb read, WriteCb write);

				

				virtual const char* getName() const override;

				std::optional<int> readTotalFiles();				
				std::optional<int> readVolume();

				bool writeVolume(int volume);
				bool writePlay(int fileId);
				bool writeStop();
				bool writeSetLoopFile();
				bool writeSetLoopFolder();
				bool writeDisableLoop();
				//do not use it is not working correctly due to firmware of dfplayer
				bool writeReset();
				bool writePlaybackCD();

			private:
				static constexpr int _DELAY_BETWEEN_CMD_MS = 150;



				const ReadCb _readCb;
				const WriteCb _writeCb;
				clock::Timestamp _lastCmdMs = 0;



				//proxy to handle delay between commands for reliability
				bool _write(std::span<uint8_t> data);
				bool _read(std::span<uint8_t> out, int timeoutMs);
				void _handleTimeDelay();
		};
	}
}