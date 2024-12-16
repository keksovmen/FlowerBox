#include "fb_uart.hpp"



using namespace fb;
using namespace interfaces;



Uart::Uart(uart_port_t port, int rx, int tx, int baudrate)
	: _port(port)
{
	const uart_config_t cfg = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.source_clk = UART_SCLK_DEFAULT
	};
	ESP_ERROR_CHECK(uart_driver_install(_port, 256, 256, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(_port, &cfg));
	ESP_ERROR_CHECK(uart_set_pin(_port, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

Uart::~Uart()
{
	uart_driver_delete(_port);
}

bool Uart::read(std::span<uint8_t> out, int timeoutMs)
{
	return uart_read_bytes(_port, out.data(), out.size_bytes(), pdMS_TO_TICKS(timeoutMs))
		== out.size_bytes();
}

bool Uart::write(std::span<uint8_t> in)
{
	return uart_write_bytes(_port, in.data(), in.size_bytes())
		== in.size_bytes();
}
