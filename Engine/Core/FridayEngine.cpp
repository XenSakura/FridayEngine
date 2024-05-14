// FridayEngine.cpp : Defines the entry point for the application.
//

#include "Engine.h"
#include <iostream>

int main() 
{
    Engine instance;
    try
    {
        instance.Run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
 }
    