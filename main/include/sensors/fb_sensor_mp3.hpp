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
				Mp3Sensor(int port, int rxPin, int txPin);

				virtual const char* getName() const override;

				bool play(int fileId);
				void stop();
				bool setVolume(int volume);
				bool setLoop(bool state);
				//valid only if sensor is init
				int getFilesCount() const;
				int getVolume() const;
				bool isLooping() const;

			private:
				interfaces::Uart _uart;
				player::DfPlayer _player;

				

				int _filesCount = 0;
				int _volume = 15;
				bool _loopFlag = true;



				virtual bool _doInit() override;
				virtual SensorIface::UpdateResult _doUpdate() override;
		};
	}
}