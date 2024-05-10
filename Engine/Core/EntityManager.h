#pragma once
#include "../System.h"
#include <functional>
#include <utility>
#include <string>
#include <any>
#include <typeindex>

using Entity = unsigned;

class EntityManager
{
	void Deserialize();
	void Update(float dt);
	void Serialize();
	//updating must always happen before rendering? but how do we split this? post and preupdate buckets
	void Render();

	template <typename T>
	void AddComponent(Entity& entity, T component)
	{
		componentArrays[typeid(T)].push_back(&component);
	}


private:
	EntityManager();
	~EntityManager();
	std::vector<Entity> entities;
	std::unordered_map<std::type_index, std::vector<std::any>> componentArrays;
	std::vector<System> Systems;
};
