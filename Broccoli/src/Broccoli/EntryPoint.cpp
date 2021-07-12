#pragma once
#include "Core/Application.h"

bool applicationRunning = true;

int main(int argc, char** argv)
{
	while (applicationRunning)
	{
		std::cout << "Engine Starting\n";
		Broccoli::Application* app = new Broccoli::Application(argc, argv);
		app->Run();
		delete app;
		std::cout << "Engine Shutting down...\n";
	}

	return 0;
}