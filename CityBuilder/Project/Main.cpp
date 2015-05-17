#include <Windows.h>
#include <iostream>
#include "Engine.h"

int main()
{
	Engine* engine = new Engine(800, 800);

	try
	{
		engine->Go();
	}
	catch(std::string e)
	{
		std::cout << e << std::endl;
	}
	delete engine;
}
