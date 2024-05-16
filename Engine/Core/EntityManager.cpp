#include "EntityManager.h"

EntityManager::EntityManager()
{
	for (const auto& system : Systems)
	{
		system.SystemInit();
	}
}
void EntityManager::Deserialize()
{

}
void EntityManager::Update(float dt)
{

}
void EntityManager::Render()
{

}
void EntityManager::Serialize()
{

}
EntityManager::~EntityManager()
{
	for (const auto& system : Systems)
	{
		system.SystemExit();
	}
}