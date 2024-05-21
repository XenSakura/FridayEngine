/*****************************************************************//**
 * @file   Engine.cpp
 * @brief  Main Game Engine file
 * 
 * @author Sakura
 * @date   April 2024
 *********************************************************************/
#include "Engine.h"
#include "..\UnitTests.h"
#include <iostream>
/**
 * @brief Engine Constructor.
 * 
 */
Engine::Engine()
    : prevTime(std::chrono::steady_clock::now()),
    renderInstance(nullptr)
{
    renderInstance = std::make_unique<RenderSystem>();
    window = &renderInstance.get()->GetWindow();
#ifdef _DEBUG
    RunUnitTests();
#endif
}

/**
 * @brief Engine's main run loop.
 * 
 */
void Engine::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - prevTime;
        float dt = deltaTime.count();
        renderInstance->render();
        prevTime = currentTime;
    }
}
/**
 * @brief Engine Destructor.
 * 
 */
Engine::~Engine()
{
    
}
