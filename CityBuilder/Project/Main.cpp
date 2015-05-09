#include <Windows.h>

#include "Engine.h"

int main()
{
	Engine* engine = new Engine(800, 800);

	try
	{
		engine->Go();
	}
	catch(char* e)
	{
		MessageBoxA(NULL, e, "Exception Occured", MB_OK | MB_ICONERROR);
	}
}
