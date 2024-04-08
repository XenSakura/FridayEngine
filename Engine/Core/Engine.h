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
};