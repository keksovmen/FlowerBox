#pragma once



#include "fb_sensor_iface.hpp"
#include "fb_uart.hpp"
#include "fb_df_player.hpp"



namespace fb
{
	namespace sensor
	{
		class Mp3Sensor : public SensorIface
		{
			public:
				static constexpr int MIN_VOLUME = 0;
				static constexpr int MAX_VOLUME = 30;



				Mp3Sensor(int port, int rxPin, int txPin);
				Mp3Sensor(int port, int rxPin, int txPin, int busyPin);

				virtual const char* getName() const override;

				bool play(int fileId);
				void stop();
				bool setVolume(int volume);
				bool setLoop(bool state);
				//valid only if sensor is init
				int getFilesCount() const;
				int getVolume() const;
				bool isLooping() const;
				bool isPlaying() const;

			private:
				static constexpr int _UNDEFINED_PIN = -1;



				interfaces::Uart _uart;
				player::DfPlayer _player;

				
				int _busyPin = _UNDEFINED_PIN;
				int _filesCount = -1;
				int _volume = 15;
				bool _loopFlag = true;
				bool _initFlag = false;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;
		};
	}
}