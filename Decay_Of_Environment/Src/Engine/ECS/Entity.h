#pragma once
#include "Components.h"

#include <tuple>
#include <string>
#include "EntityManager.h"

class EntityManager;

typedef std::tuple<
	CTransform,
	CModel,
	CInput,
	CUnitstats,
	CBuildingStats,
	CLight,
	CTest,
	CAnimation,
	CCamera>
	ComponentTuple;



class Entity
{
	friend class EntityManager;


	bool m_active = true;
	std::string m_tag = "default";
	size_t m_id = 0;

	ComponentTuple m_components;
public:
	Entity();
	Entity(const size_t& id, const std::string& tag);


public:
	void destroy();
	size_t id() const;
	bool isActive() const;
	std::string& tag();

	template <typename T>
	bool hasComponent() const
	{
		return getComponent<T>().has;
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		auto& component = getComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}
	template <typename T>
	T& getComponent()
	{
		return std::get<T>(m_components);
	}
	template <typename T>
	const T& getComponent() const
	{
		return std::get<T>(m_components);
	}
	template <typename T>
	void removeComponent()
	{
		getComponent<T>() = T();
	}
};