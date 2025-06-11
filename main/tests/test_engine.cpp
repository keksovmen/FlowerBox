#include "fb_templates.hpp"

#include <iostream>



static char _buffer[128];
static int _counter = 0;

static void _consumer(const char* data, int size)
{
	_counter++;
	std::cout.write(data, size);
}


int main()
{
	fb::templates::Engine engine(_buffer, sizeof(_buffer), _consumer);
	engine.addIntArg(3, "count");
	engine.addArgArray({"Temperature", "Humidity", "Moisture"}, "name");
	engine.addArgArray({"loh", "pidr", "sosi"}, "description");
	engine.addArgArray({"0", "20", "30"}, "id");

	const bool result = engine.process("D:\\embeded\\FlowerBox\\spiffs_image\\sensors.html");
	std::cout << std::endl << "Total consume cb:" << _counter << "\t" << "Result: " << result << std::endl;
}