#pragma once
#include "Entity.h"
#include <map>
#include <vector>
#include <memory>

class Entity;

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map <std::string, EntityVec> EntityMap;

class EntityManager
{


	EntityVec m_entities;
	EntityVec m_toAdd;
	EntityVec m_toRemove;
	EntityMap m_entityMap;
	size_t m_totalEntities = 0;
	
	void removeDeadEntities(EntityVec& vec);
public:

	EntityManager();
	void update();
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	EntityVec& getEntities();
	EntityVec& getEntities(const std::string& tag);
	std::shared_ptr<Entity> GetEntity(const std::string& tag) { return m_entityMap.find(tag)->second[0]; }
	std::shared_ptr<Entity> GetEntityByID(const int id);
	void Reset();

};