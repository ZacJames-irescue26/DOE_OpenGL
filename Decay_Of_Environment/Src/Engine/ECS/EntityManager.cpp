#include "pch.h"
#include "EntityManager.h"


EntityManager::EntityManager()
{

}

void EntityManager::update()
{

	//TODO add entities from m_entitiesTOAdd

	for (auto e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);

	}
	m_toAdd.clear();



	//for (auto i = m_entities.begin(); i < m_entities.end(); ++i) {
	//	if (i->get()->m_alive == false) {
	//		m_entities.erase(i);
	//		//auto it = m_entityMap.find(i->get()->id());
	//		//i--;
	//	}
	//}



	/*for (auto& e : vec)
	{
		if (!e->isActive())
		{
			m_entityMap.erase(e->tag());
		}
	
	}*/

	removeDeadEntities(m_entities);

	for (auto& [tag, EntityVec] : m_entityMap)
	{
		removeDeadEntities(EntityVec);
	}

}
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_toAdd.push_back(e);
	return e;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap.find(tag)->second;
}



std::shared_ptr<Entity> EntityManager::GetEntityByID(const int id)
{
	for (auto& entity : m_entities)
	{
		if (entity->id() == id)
		{
			return entity;
		}
	}
}

void EntityManager::Reset()
{
	m_entities.clear();
	m_toAdd.clear();
	m_toRemove.clear();
	m_entityMap.clear();
	m_totalEntities = 0;
}

//std::shared_ptr<Entity> EntityManager::GetEntity(const std::string& tag)
//{
//
//}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	for (auto e : vec)
	{

		/*auto removeEntities = [&](std::shared_ptr<Entity>& e) -> bool
		{
			std::cout << "e is :" << e->is_Alive() << std::endl;
			return !e->is_Alive();
		};*/
		//std::cout << "e is :" << e->m_alive << std::endl;
		//std::cout << vec.size() << std::endl;


		if (!e->m_active)
		{

			//std::cout << e.get() << std::endl;
			//std::cout << "e is :" << e->m_alive << std::endl;

			std::cout << e->m_tag << "is dead" << std::endl;
			auto iterator = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& e) {
				return !e->m_active; });
			vec.erase(iterator, vec.end());
			m_entityMap.erase(e->m_tag);

		}
	}
}

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}