#pragma once
#include "Engine/ECS/EntityManager.h"
#include "Engine/ECS/Entity.h"
//enum UnitType
//{
//	Worker,
//	Trooper
//};
//
//struct UnitStats
//{
//	std::shared_ptr<Model> UnitModel = nullptr;
//	int Team = 0;
//	int Owner = 0;
//	float MaxHealth = 10.0f;
//	float Currenthealth = 10.0f;
//	float AttackDamage = 2.0f;
//	float AttackRange = 1.0f;
//	float BuildTime = 10.0f;
//};

//class Unit : public Entity
//{
//public:
//	Unit(UnitType type, UnitStats stats, const std::string& tag);
//	UnitStats Stats;
//	Unit* Target;
//	UnitType Type;
//	std::string m_Tag;
//
//};