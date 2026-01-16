#include "fb_wrapper_db135.hpp"



using namespace fb;
using namespace wrappers;



WrapperDb135::WrapperDb135(int scl, int sda, int cs)
	: _gpioScl(scl), _gpioSda(sda), _gpioCs(cs)
{

}


void WrapperDb135::init()
{
	//TODO: test no init structure what will be affected by random
	spi_bus_config_t busCfg;
	busCfg.mosi_io_num = _gpioSda;
	busCfg.miso_io_num = -1;
	busCfg.sclk_io_num = _gpioScl;
	busCfg.quadwp_io_num = -1;
	busCfg.quadhd_io_num = -1;
	busCfg.max_transfer_sz = 4;   // small, we only send 2 bytes

    // Initialize SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &busCfg, SPI_DMA_DISABLED));

    spi_device_interface_config_t devcfg = {
        .command_bits     = 0,
        .address_bits     = 0,
        .dummy_bits       = 0,
        .mode             = 0,
        .clock_source     = SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos   = 0,
        .cs_ena_pretrans  = 8,
        .cs_ena_posttrans = 0,
        .clock_speed_hz   = 10 * 1000 * 1000,   // example 10 MHz
        .input_delay_ns   = 0,
        .sample_point     = SPI_SAMPLING_POINT_PHASE_0, // or default for your IDF
        .spics_io_num     = _gpioCs,
        .flags            = 0,
        .queue_size       = 1,
        .pre_cb           = NULL,
        .post_cb          = NULL,
    };

    // Add device to bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &_dev));
}

void WrapperDb135::setValue(bool value)
{
	_state = value ? 0xFFFF : 0;
	_sendState();
}

void WrapperDb135::setValue(int value)
{
	_state = value;
	_sendState();
}

void WrapperDb135::setPin(int pin, bool isOn)
{
	if(isOn){
		_state |= 1 << pin;
	}else{
		_state &= ~(1 << pin);
	}

	_sendState();
}

uint16_t WrapperDb135::getValue() const
{
	return _state;
}

void WrapperDb135::_sendState()
{
	//first byte is [8; 15] outputs, second byte is [0; 7] outputs
	//bit position is equal, 0 bit is 0 output
	//so need to know first byte is high value, despite it sending first
	//it means it is MSB first if looking at 2 bytes

	uint8_t data[2] = {static_cast<uint8_t>(_state >> 8), static_cast<uint8_t>(_state & 0xFF)};

	//TODO: test no init structure what will be affected by random
    spi_transaction_t t;
	t.flags     = 0;
	t.cmd       = 0;
	t.length    = 16;        // 16 bits total
	t.rxlength  = 0;
	t.tx_buffer = data;
	t.rx_buffer = NULL;

    // One transaction: bytes go out back-to-back, no gap on the bus
    ESP_ERROR_CHECK(spi_device_polling_transmit(_dev, &t));
}