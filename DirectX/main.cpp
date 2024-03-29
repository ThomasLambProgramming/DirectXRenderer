#include "ApplicationDirector.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	ApplicationDirector* System;
	bool result;

	// Create the system object.
	System = new ApplicationDirector;

	// Initialize and run the system object.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}