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
    RenderSystem(GLFWwindow& instance)
    {
        data.window = &instance;
        SetupFunction(data);
    }
    ~RenderSystem()
    {
        CleanupFunction(data);
    }
    void render()
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        RenderFunction(data);
    }
private:
    RenderData data;
    std::mutex dataMutex;
    RenderAPIInit SetupFunction = VulkanSetup;
    RenderAPIRender RenderFunction;
    RenderAPIExit CleanupFunction = VulkanCleanup;
   
};