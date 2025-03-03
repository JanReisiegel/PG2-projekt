// icp.cpp 
// author: JJ

#include <iostream>

#include "app.hpp"
#include <Windows.h>

void lockToSingleCore()
{
	HANDLE hProcess = GetCurrentProcess();
	SetProcessAffinityMask(hProcess, 1);
}

// define our application
App app;

int main()
{
	//lockToSingleCore();
	//std::cout << "Starting app in single core mode...\n" << std::endl;

    try {
        if (app.init())
            return app.run();
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
