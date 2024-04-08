#include "Engine.h"
Engine::Engine()
	:prevTime(std::chrono::steady_clock::now())
{

}

void Engine::Update()
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - prevTime;
	float dt = deltaTime.count();

	prevTime = currentTime;
}

Engine::~Engine()
{
	
}

