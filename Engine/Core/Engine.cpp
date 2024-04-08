/*****************************************************************//**
 * @file   Engine.cpp
 * @brief  Main Game Engine file
 * 
 * @author Sakura
 * @date   April 2024
 *********************************************************************/
#include "Engine.h"
#include <vector>
#include <iostream>
#include <stdexcept>
/**
 * @brief Engine Constructor.
 * 
 */
Engine::Engine()
    : prevTime(std::chrono::steady_clock::now()),
    renderInstance(std::make_unique<RenderSystem>())
{
    GLFWSetup();
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

        renderInstance->Update(dt);

        prevTime = currentTime;
    }
}
/**
 * @brief Engine Destructor.
 * 
 */
Engine::~Engine()
{
    vkDestroyInstance(instance, nullptr);
    GLFWCleanup();
}
/**
 * @brief GLFW setup commands.
 * 
 */
void Engine::GLFWSetup()
{
    glfwInit();
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
/**
 * @brief Setting up Vulkan.
 * 
 */
void Engine::VulkanSetup()
{
    CreateInstance();
}
/**
 * @brief Helper function to handle Vulkan Setup.
 * 
 */
void Engine::CreateInstance()
{
    // Create a struct to store information about the application.
    VkApplicationInfo appInfo{};
    // Set the type of the structure.
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // Set the name of the application.
    appInfo.pApplicationName = "Hello Triangle";
    // Set the version of the application.
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    // Set the name of the engine (if any).
    appInfo.pEngineName = "No Engine";
    // Set the version of the engine (if any).
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    // Set the Vulkan API version.
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Create a struct to specify parameters of the Vulkan instance creation.
    VkInstanceCreateInfo createInfo{};
    // Set the type of the structure.
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // Set a pointer to the application info.
    createInfo.pApplicationInfo = &appInfo;

    // Get required instance extensions from GLFW.
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    // Set the number and names of enabled instance extensions.
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Set the number of enabled layers.
    createInfo.enabledLayerCount = 0;

    // Retrieve a list of available Vulkan extensions.
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "available extensions:\n";
    // Print the names of available extensions.
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    // Create a Vulkan instance using the specified parameters.
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
