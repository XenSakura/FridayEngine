#pragma once
#include <chrono>
class Engine
{
public:
	Engine();
	void Update();
	~Engine();
private:
	std::chrono::steady_clock::time_point prevTime;
};