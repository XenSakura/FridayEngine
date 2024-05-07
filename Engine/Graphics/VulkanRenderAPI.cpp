#include "VulkanRenderAPI.h"
#include <algorithm> //
#include <stdexcept> // exceptions
#include <iostream> //cerr
#include <set> //set data structure
#include <optional>
#include <fstream>

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional <uint32_t> presentFamily;

    bool isComplete() const
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


//Function declarations
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
void SetupDebugMessenger(RenderData& data);
void CreateInstance(RenderData& data);
std::vector<const char*> GetRequiredExtensions();
bool CheckValidationLayerSupport();
bool isDeviceSuitable(RenderData& data, VkPhysicalDevice device);
void PickPhysicalDevice(RenderData& data);
QueueFamilyIndices FindQueueFamilies(RenderData& data, VkPhysicalDevice& device);
void CreateLogicalDevice(RenderData& data);
void CreateSurface(RenderData& data);
void CreateSwapChain(RenderData& data);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D ChooseSwapExtent(RenderData& data, const VkSurfaceCapabilitiesKHR& capabilities);
SwapChainSupportDetails QuerySwapChainSupport(RenderData& data, VkPhysicalDevice& device);
void CreateImageViews(RenderData& data);
void CreateRenderPass(RenderData& data);
void CreateGraphicsPipeline(RenderData& data);
VkShaderModule CreateShaderModule(RenderData& data, const std::vector<char>& code);
static std::vector<char> readFile(const std::string& filename);
void CreateFrameBuffers(RenderData& data);
void CreateCommandPool(RenderData& data);
void CreateCommandBuffers(RenderData& data);
void RecordCommandBuffer(RenderData& data, VkCommandBuffer commandBuffer, uint32_t imageIndex);
void CreateSyncObjects(RenderData& data);
void RecreateSwapChain(RenderData& data);
void CleanupSwapChain(RenderData& data);
static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



/**
 * @brief Create a debug messenger object to receive validation layer messages.
 *
 * @param instance The Vulkan instance.
 * @param pCreateInfo Pointer to the debug messenger creation info.
 * @param pAllocator Pointer to allocation callbacks.
 * @param pDebugMessenger Pointer to store the debug messenger object.
 * @return VkResult Result of the debug messenger creation.
 */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/**
 * @brief Destroy a debug messenger object.
 *
 * @param instance The Vulkan instance.
 * @param debugMessenger The debug messenger object.
 * @param pAllocator Pointer to allocation callbacks.
 */
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

/**
 * @brief Setup Vulkan components.
 *
 * @param data Render data structure containing Vulkan components.
 */
void VulkanSetup(RenderData& data)
{
    glfwSetWindowUserPointer(data.window, &data);
    CreateInstance(data);
    SetupDebugMessenger(data);
    CreateSurface(data);
    PickPhysicalDevice(data);
    CreateLogicalDevice(data);
    CreateSwapChain(data);
    CreateImageViews(data);
    CreateRenderPass(data);
    CreateGraphicsPipeline(data);
    CreateFrameBuffers(data);
    CreateCommandPool(data);
    CreateCommandBuffers(data);
    CreateSyncObjects(data);
}

/**
 * @brief Cleanup Vulkan resources.
 *
 * @param data Render data structure containing Vulkan components.
 */
void VulkanCleanup(RenderData& data)
{
    vkDeviceWaitIdle(data.device);
    CleanupSwapChain(data);

    vkDestroyPipeline(data.device, data.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(data.device, data.pipelineLayout, nullptr);

    vkDestroyRenderPass(data.device, data.renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroySemaphore(data.device, data.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(data.device, data.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(data.device, data.inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(data.device, data.commandPool, nullptr);
    
    vkDestroyDevice(data.device, nullptr);

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(data.instance, data.debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(data.instance, data.surface, nullptr);

    vkDestroyInstance(data.instance, nullptr);
}

/**
 * @brief Vulkan debug callback function to handle validation layer messages.
 *
 * @param messageSeverity Severity of the message.
 * @param messageType Type of the message.
 * @param pCallbackData Pointer to debug callback data.
 * @param pUserData Pointer to user data.
 * @return VkBool32 Boolean value indicating message handling.
 */
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
    auto app = reinterpret_cast<RenderData*>(glfwGetWindowUserPointer(window));
    app->frameBufferResized = true;
}

/**
 * @brief Populate debug messenger creation info.
 *
 * @param createInfo Reference to the debug messenger creation info structure.
 */
void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

/**
 * @brief Setup debug messenger for validation layers.
 *
 * @param data Render data structure containing Vulkan components.
 */
void SetupDebugMessenger(RenderData& data)
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(data.instance, &createInfo, nullptr, &data.debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

/**
 * @brief Creates a Vulkan instance.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateInstance(RenderData& data)
{
    // Check if validation layers are enabled and available
    if (enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    // Set up application and engine information
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "FridayEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "SaturdayEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Create Vulkan instance
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get required instance extensions
    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Set up debug messenger if validation layers are enabled
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // Create Vulkan instance with specified configurations
    if (vkCreateInstance(&createInfo, nullptr, &data.instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

/**
 * @brief Retrieves the required extensions for the Vulkan instance.
 *
 * @return A vector containing the required extensions.
 */
std::vector<const char*> GetRequiredExtensions()
{
    // Get required extensions for interfacing with the windowing system
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Add debug utils extension if validation layers are enabled
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Return the list of required extensions
    return extensions;
}

/**
 * @brief Checks if the required validation layers are supported by the Vulkan instance.
 *
 * @return True if all validation layers are supported, false otherwise.
 */
bool CheckValidationLayerSupport()
{
    // Retrieve available instance layer properties
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check if all required validation layers are present
    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        // Iterate through available layers to check if the required layer is present
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        // If any required layer is not found, return false
        if (!layerFound)
        {
            return false;
        }
    }

    // Return true if all required validation layers are present
    return true;
}

/**
 * @brief Checks if a physical device is suitable for rendering.
 *
 * @param data The RenderData struct containing rendering data.
 * @param device The Vulkan physical device to check.
 * @return True if the device is suitable, false otherwise.
 */
bool isDeviceSuitable(RenderData& data, VkPhysicalDevice device)
{
    // Find queue families supported by the device
    QueueFamilyIndices indices = FindQueueFamilies(data, device);
    return indices.isComplete();
}

/**
 * @brief Picks a suitable physical device for rendering.
 *
 * @param data The RenderData struct containing rendering data.
 */
void PickPhysicalDevice(RenderData& data)
{
    // Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(data.instance, &deviceCount, nullptr);

    // Check if any GPU with Vulkan support is found
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    // Retrieve the list of physical devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(data.instance, &deviceCount, devices.data());

    // Select the first suitable device
    for (const auto& device : devices) {
        if (isDeviceSuitable(data, device)) {
            data.physicalDevice = device;
            break;
        }
    }

    // Check if a suitable GPU is found
    if (data.physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

/**
 * @brief Finds queue families supported by a physical device.
 *
 * @param data The RenderData struct containing rendering data.
 * @param device The Vulkan physical device to query.
 * @return The QueueFamilyIndices struct containing indices of supported queue families.
 */
QueueFamilyIndices FindQueueFamilies(RenderData& data, VkPhysicalDevice& device)
{
    QueueFamilyIndices indices;

    // Query queue family properties
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        // Check if the queue family supports graphics operations
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // Check if the queue family supports presentation
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, data.surface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        // Break loop if both graphics and presentation families are found
        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

/**
 * @brief Creates the logical device used for Vulkan rendering operations.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateLogicalDevice(RenderData& data)
{
    // Find queue families supported by the physical device
    QueueFamilyIndices indices = FindQueueFamilies(data, data.physicalDevice);

    // Configure device queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Specify device features and extensions
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Enable validation layers if necessary
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    // Create the logical device
    if (vkCreateDevice(data.physicalDevice, &createInfo, nullptr, &data.device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    // Retrieve queue handles from the logical device
    vkGetDeviceQueue(data.device, indices.graphicsFamily.value(), 0, &data.graphicsQueue);
    vkGetDeviceQueue(data.device, indices.presentFamily.value(), 0, &data.presentQueue);
}

/**
 * @brief Creates the surface used for rendering.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateSurface(RenderData& data)
{
    if (glfwCreateWindowSurface(data.instance, data.window, nullptr, &data.surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
}

/**
 * @brief Creates the swap chain used for presenting rendered images to the screen.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateSwapChain(RenderData& data)
{
    // Query swap chain support details
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(data, data.physicalDevice);

    // Choose surface format, presentation mode, and extent
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(data, swapChainSupport.capabilities);

    // Determine the number of images in the swap chain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Configure swap chain creation parameters
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = data.surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(data, data.physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create the swap chain
    if (vkCreateSwapchainKHR(data.device, &createInfo, nullptr, &data.swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Retrieve swap chain images
    vkGetSwapchainImagesKHR(data.device, data.swapChain, &imageCount, nullptr);
    data.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(data.device, data.swapChain, &imageCount, data.swapChainImages.data());

    // Store swap chain properties in RenderData struct
    data.swapChainImageFormat = surfaceFormat.format;
    data.swapChainExtent = extent;
}

/**
 * @brief Chooses the swap chain surface format.
 *
 * @param availableFormats The available surface formats.
 * @return VkSurfaceFormatKHR The chosen surface format.
 */
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        // Check if the format and color space match the preferred ones
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            // Return the preferred format
            return availableFormat;
        }
    }

    // If the preferred format is not found, return the first available format
    return availableFormats[0];
}

/**
 * @brief Chooses the swap chain presentation mode.
 *
 * @param availablePresentModes The available presentation modes.
 * @return VkPresentModeKHR The chosen presentation mode.
 */
VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        // Check if the present mode matches the preferred one
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            // Return the preferred present mode
            return availablePresentMode;
        }
    }

    // If the preferred present mode is not found, return FIFO as fallback
    return VK_PRESENT_MODE_FIFO_KHR;
}

/**
 * @brief Chooses the swap chain extent.
 *
 * @param data The RenderData struct containing rendering data.
 * @param capabilities The surface capabilities.
 * @return VkExtent2D The chosen swap chain extent.
 */
VkExtent2D ChooseSwapExtent(RenderData& data, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        // Return the current extent if it is defined
        return capabilities.currentExtent;
    }
    else
    {
        // Retrieve window framebuffer size
        int width, height;
        glfwGetFramebufferSize(data.window, &width, &height);

        // Clamp the framebuffer size within the supported range
        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        // Return the clamped extent
        return actualExtent;
    }
}

/**
 * @brief Queries swap chain support details.
 *
 * @param data The RenderData struct containing rendering data.
 * @param device The physical device to query.
 * @return SwapChainSupportDetails Swap chain support details.
 */
SwapChainSupportDetails QuerySwapChainSupport(RenderData& data, VkPhysicalDevice& device)
{
    SwapChainSupportDetails details;

    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, data.surface, &details.capabilities);

    // Query surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, data.surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, data.surface, &formatCount, details.formats.data());
    }

    // Query present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, data.surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, data.surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

/**
 * @brief Creates image views for swap chain images.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateImageViews(RenderData& data)
{
    // Resize the vector to hold image views for each swap chain image
    data.swapChainImageViews.resize(data.swapChainImages.size());

    // Create image views for each swap chain image
    for (size_t i = 0; i < data.swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = data.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = data.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        // Create image view for the current swap chain image
        if (vkCreateImageView(data.device, &createInfo, nullptr, &data.swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image views!");
        }
    }
}

/**
 * @brief Creates a graphics pipeline.
 *
 * @param data The RenderData struct containing rendering data.
 */
void CreateGraphicsPipeline(RenderData& data)
{
    // Load vertex and fragment shader code
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    // Create shader modules
    VkShaderModule vertShaderModule = CreateShaderModule(data, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(data, fragShaderCode);

    // Configure shader stages
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    // Combine shader stages
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Configure pipeline vertex input state
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    // Configure pipeline input assembly state
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Configure pipeline viewport state
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Configure pipeline rasterization state
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Configure pipeline multisample state
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Configure pipeline color blend attachment state
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    // Configure pipeline color blend state
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Configure pipeline dynamic state
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Configure pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    // Create pipeline layout
    if (vkCreatePipelineLayout(data.device, &pipelineLayoutInfo, nullptr, &data.pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Configure graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = data.pipelineLayout;
    pipelineInfo.renderPass = data.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    // Create graphics pipeline
    if (vkCreateGraphicsPipelines(data.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &data.graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // Destroy shader modules
    vkDestroyShaderModule(data.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(data.device, vertShaderModule, nullptr);
}

/**
 * @brief Creates a shader module from SPIR-V code.
 *
 * @param data The RenderData struct containing rendering data.
 * @param code The SPIR-V shader code.
 * @return VkShaderModule The created shader module.
 */
VkShaderModule CreateShaderModule(RenderData& data, const std::vector<char>& code)
{
    // Configure shader module creation
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    // Create shader module
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(data.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    // Return the created shader module
    return shaderModule;
}
/**
 * @brief Creates a render pass.
 *
 * This function creates a render pass object with a single color attachment.
 *
 * @param data The rendering data containing the Vulkan device and swap chain image format.
 * @throws std::runtime_error if the creation of the render pass fails.
 */
void CreateRenderPass(RenderData& data)
{
    // Define color attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = data.swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Define color attachment reference for subpass
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Define subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // Create render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // Create render pass
    if (vkCreateRenderPass(data.device, &renderPassInfo, nullptr, &data.renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}
/**
 * @brief Creates frame buffers for rendering.
 *
 * This function creates frame buffers for rendering using the provided render data.
 * Each frame buffer corresponds to an image view in the swap chain.
 *
 * @param data The render data containing necessary information for creating frame buffers.
 */
void CreateFrameBuffers(RenderData& data)
{
    // Resize vector to hold framebuffers for each swap chain image view
    data.swapChainFramebuffers.resize(data.swapChainImageViews.size());

    // Create framebuffers for each swap chain image view
    for (size_t i = 0; i < data.swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] =
        {
            data.swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = data.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = data.swapChainExtent.width;
        framebufferInfo.height = data.swapChainExtent.height;
        framebufferInfo.layers = 1;

        // Create framebuffer
        if (vkCreateFramebuffer(data.device, &framebufferInfo, nullptr, &data.swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
/**
 * @brief Creates a command pool for managing command buffers.
 *
 * This function creates a command pool associated with a specific queue family.
 *
 * @param data The RenderData struct containing Vulkan device and physical device info.
 */
void CreateCommandPool(RenderData& data)
{
    // Find queue families supported by the physical device
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(data, data.physicalDevice);

    // Configure command pool creation
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    // Create command pool
    if (vkCreateCommandPool(data.device, &poolInfo, nullptr, &data.commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}
/**
 * @brief Allocates a command buffer from the command pool.
 *
 * This function allocates a command buffer from the command pool.
 *
 * @param data The RenderData struct containing Vulkan device and command pool info.
 */
void CreateCommandBuffers(RenderData& data)
{
    data.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    // Configure command buffer allocation
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = data.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)data.commandBuffers.size();

    // Allocate command buffer
    if (vkAllocateCommandBuffers(data.device, &allocInfo, data.commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}
/**
 * @brief Records commands into a command buffer for rendering.
 *
 * This function records commands into a command buffer for rendering a frame.
 *
 * @param data The RenderData struct containing Vulkan device and rendering info.
 * @param commandBugger The command buffer to record commands into.
 * @param imageIndex The index of the swap chain image.
 */
void RecordCommandBuffer(RenderData& data, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // Begin recording command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = data.renderPass;
    renderPassInfo.framebuffer = data.swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = data.swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind graphics pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, data.graphicsPipeline);

    // Set viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)data.swapChainExtent.width;
    viewport.height = (float)data.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Set scissor
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = data.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Draw
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // End render pass
    vkCmdEndRenderPass(commandBuffer);

    // End recording command buffer
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}
/**
 * @brief Reads binary data from a file.
 *
 * @param filename The path to the file.
 * @return std::vector<char> The binary data read from the file.
 */
static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!" + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
/**
 * @brief Creates synchronization objects for coordinating rendering operations.
 *
 * This function creates semaphores and fences used for synchronizing rendering operations.
 * Semaphores are used to synchronize operations between command queues, while fences are used
 * to synchronize the CPU with rendering operations on the GPU.
 *
 * @param data The RenderData struct containing Vulkan device and synchronization object info.
 */
void CreateSyncObjects(RenderData& data)
{
    data.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    data.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    data.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(data.device, &semaphoreInfo, nullptr, &data.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(data.device, &semaphoreInfo, nullptr, &data.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(data.device, &fenceInfo, nullptr, &data.inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}
/**
 * @brief Recreates the swap chain to adapt to changes in window dimensions or configuration.
 *
 * This function is called when the current swap chain is no longer compatible with the window's dimensions
 * or configuration, typically due to a window resize event. It waits for the device to finish any pending
 * operations, recreates the swap chain with updated dimensions and configuration, creates image views for
 * the new swap chain images, and creates framebuffers for rendering using the new swap chain images.
 *
 * @param data The render data structure containing Vulkan objects and settings.
 */
void RecreateSwapChain(RenderData& data) 
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(data.window, &width, &height);
    while (width == 0 || height == 0) 
    {
        glfwGetFramebufferSize(data.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(data.device);

    CleanupSwapChain(data);

    CreateSwapChain(data);
    CreateImageViews(data);
    CreateFrameBuffers(data);
}

void CleanupSwapChain(RenderData& data)
{
    for ( auto framebuffer : data.swapChainFramebuffers) {
        vkDestroyFramebuffer(data.device, framebuffer, nullptr);
    }

    for (auto imageView : data.swapChainImageViews) {
        vkDestroyImageView(data.device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(data.device, data.swapChain, nullptr);
}
/**
 * @brief Performs the Vulkan rendering process for a single frame.
 *
 * This function executes the Vulkan rendering process for a single frame, including
 * acquiring swap chain images, recording command buffers, submitting command buffers
 * to the graphics queue, and presenting rendered images to the screen.
 *
 * @param data The RenderData struct containing Vulkan device and rendering info.
 */
void VulkanRender(RenderData& data)
{
    // Wait for the fence associated with the current frame to signal that it's safe to start rendering
    vkWaitForFences(data.device, 1, &data.inFlightFences[data.currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire the index of the next available image from the swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(data.device, data.swapChain, UINT64_MAX, data.imageAvailableSemaphores[data.currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) 
    {
        RecreateSwapChain(data);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Reset the fence to prepare it for the next frame
    vkResetFences(data.device, 1, &data.inFlightFences[data.currentFrame]);

    // Reset the command buffer for the current frame
    vkResetCommandBuffer(data.commandBuffers[data.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

    // Record the commands into the command buffer for the current frame
    RecordCommandBuffer(data, data.commandBuffers[data.currentFrame], imageIndex);

    // Configure submission information for the command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { data.imageAvailableSemaphores[data.currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &data.commandBuffers[data.currentFrame];

    VkSemaphore signalSemaphores[] = { data.renderFinishedSemaphores[data.currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Submit the command buffer to the graphics queue for execution
    if (vkQueueSubmit(data.graphicsQueue, 1, &submitInfo, data.inFlightFences[data.currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Configure presentation information
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { data.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(data.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || data.frameBufferResized) {
        data.frameBufferResized = false;
        RecreateSwapChain(data);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    // Move to the next frame to prepare for rendering the next frame
    data.currentFrame = (data.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
