#pragma once
#include <functional>
struct System
{
	std::function<void()> SystemInit;
	std::function<void(std::vector<void*>, float dt)> SystemUpdate;
	std::function<void(std::vector<void*>)> SystemRender;
	std::function<void()> SystemExit;
};