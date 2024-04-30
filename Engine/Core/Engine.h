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

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional <uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails 
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};



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

    bool CheckValidationLayerSupport();

    std::vector<const char*> GetRequiredExtensions();

    VkDebugUtilsMessengerEXT debugMessenger;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void SetupDebugMessenger();

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void PickPhysicalDevice();

    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    void CreateLogicalDevice();

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

    VkSurfaceKHR surface;
    void CreateSurface();

    bool isDeviceSuitable(VkPhysicalDevice device);

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    void CreateSwapChain();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

    std::vector<VkImageView> swapChainImageViews;

    void CreateImageViews();
    
};

