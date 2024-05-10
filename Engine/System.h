#pragma once
#include <functional>
struct System
{
	std::function<void()> SystemInit;
	std::function<void(std::vector<void*>, float dt)> SystemUpdate;
	std::function<void(std::vector<void*>)> SystemRender;
	std::function<void()> SystemExit;
};
struct Component1; 
struct Component2;

void SystemInit()
{

}

void SystemUpdate(std::vector<Component1>& component1, std::vector<Component2>& comppnent2, float dt)
{

}
