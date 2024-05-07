#pragma once
#include "VulkanRenderAPI.h"

#include <thread>
#include <mutex>
#include <functional>


//change to include vector of objects to render
typedef std::function<void(RenderData&)> RenderAPIInit;
typedef std::function<void(RenderData&)> RenderAPIRender;
typedef std::function<void(RenderData&)> RenderAPIExit;

class RenderSystem
{
public:
    RenderSystem()
    {
        GLFWSetup();
        SetupFunction(data);
    }
    ~RenderSystem()
    {
        CleanupFunction(data);
        GLFWCleanup();
    }
    void render()
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        RenderFunction(data);
    }
    
    void GLFWSetup();
    void GLFWCleanup();

    GLFWwindow& GetWindow() const { return *data.window; }
private:
    RenderData data;
    std::mutex dataMutex;
    RenderAPIInit SetupFunction = VulkanSetup;
    RenderAPIRender RenderFunction = VulkanRender;
    RenderAPIExit CleanupFunction = VulkanCleanup;
    const uint32_t HEIGHT = 600;
    const uint32_t WIDTH = 800;
};