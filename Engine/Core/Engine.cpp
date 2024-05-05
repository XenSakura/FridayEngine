/*****************************************************************//**
 * @file   Engine.cpp
 * @brief  Main Game Engine file
 * 
 * @author Sakura
 * @date   April 2024
 *********************************************************************/
#include "Engine.h"

/**
 * @brief Engine Constructor.
 * 
 */
Engine::Engine()
    : prevTime(std::chrono::steady_clock::now()),
    renderInstance(nullptr)
{
    GLFWSetup();
    renderInstance = std::make_unique<RenderSystem>(*window);
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

        prevTime = currentTime;
    }
}
/**
 * @brief Engine Destructor.
 * 
 */
Engine::~Engine()
{

    GLFWCleanup();
    
}
/**
 * @brief GLFW setup commands.
 * 
 */
void Engine::GLFWSetup()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "FridayEngine", nullptr, nullptr);
}
/**
 * @brief GLFW cleanup.
 * 
 */
void Engine::GLFWCleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
