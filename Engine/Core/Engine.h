/*****************************************************************//**
 * \file   Engine.h
 * \brief  Main game engine header
 * 
 * \author Sakura
 * \date   April 2024
 *********************************************************************/
#pragma once
#include <chrono>
#include <memory>
#include <vector>
#include "RenderSystem.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

class Engine
{
public:

    Engine();


    void Run();


    ~Engine();

private:

    std::chrono::steady_clock::time_point prevTime;


    std::unique_ptr<RenderSystem> renderInstance;


    GLFWwindow* window;


    const uint32_t HEIGHT = 600;


    const uint32_t WIDTH = 800;


    void GLFWSetup();


    void GLFWCleanup();


    VkInstance instance;

    void VulkanSetup();


    void CreateInstance();

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    bool CheckValidationLayerSupport();

    std::vector<const char*> GetRequiredExtensions();

    VkDebugUtilsMessengerEXT debugMessenger;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void SetupDebugMessenger();

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};