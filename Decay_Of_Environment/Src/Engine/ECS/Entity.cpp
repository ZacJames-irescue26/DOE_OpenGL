#include "pch.h"
#include "Entity.h"
#include "Components.h"

Entity::Entity(const size_t& id, const std::string& tag)
{
	//CModel();
	//CInput();
	//CUnitstats();
	//CBuildingStats();
	//CLight();
	//CTest();
	//CAnimation();
	m_id = id;
	m_tag = tag;
}

Entity::Entity()
{

}

void Entity::destroy()
{
	m_active = false;
}

size_t Entity::id() const
{
	return m_id;
}

bool Entity::isActive() const
{
	return m_active;
}

std::string& Entity::tag()
{
	return m_tag;
}
