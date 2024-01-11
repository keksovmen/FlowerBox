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
	engine.addIntArg(228, "LOH");

	const bool result = engine.process("H:\\Projects\\Embeded\\esp\\FlowerBox\\spiffs_image\\index.html");
	std::cout << std::endl << "Total consume cb:" << _counter << "\t" << "Result: " << result << std::endl; 
}