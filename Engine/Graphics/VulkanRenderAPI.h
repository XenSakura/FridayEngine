#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
#include <vector>
//if making your own API, fill out renderData with what your renderer needs
struct RenderData
{
    GLFWwindow* window;

    VkInstance instance;

    VkDevice device;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkQueue graphicsQueue;

    VkQueue presentQueue;

    VkSwapchainKHR swapChain;

    std::vector<VkImage> swapChainImages;

    VkFormat swapChainImageFormat;

    VkExtent2D swapChainExtent;

    VkDebugUtilsMessengerEXT debugMessenger;

    std::vector<VkImageView> swapChainImageViews;

    VkSurfaceKHR surface;
    
    VkRenderPass renderPass;

    VkPipeline graphicsPipeline;

    VkPipelineLayout pipelineLayout;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;

    std::vector<VkSemaphore> renderFinishedSemaphores;

    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 0;

    bool frameBufferResized = false;

    VkBuffer vertexBuffer;

    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;

    VkDeviceMemory indexBufferMemory;

    VkBuffer stagingBuffer;

    VkDeviceMemory stagingBufferMemory;

    VkImage textureImage;

    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;

    VkSampler textureSampler;
};

void VulkanSetup(RenderData& data);
void VulkanRender(RenderData& data);
void VulkanCleanup(RenderData& data);
