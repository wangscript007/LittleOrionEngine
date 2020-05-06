#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__

#include "Script.h"

#include "EnemyManager.h"

class ComponentAnimation;

enum class EnemyType
{
	MUSHDOOM
};

class EnemyController : public Script
{
public:
	EnemyController();
	~EnemyController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	void TakeDamage(float damage);

	EnemyManager* punterito = nullptr;

protected:
	void Move();
	bool Attack();
	bool PlayerInSight();
	void OnCollisionEnter();
	void Die();
	virtual void Respawn(){}

protected:
	EnemyType type;
	GameObject* player = nullptr;
	ComponentAnimation* animation = nullptr;

	float rot_speed = 0.01f;
	float move_speed = 0.2f;
	float attack_speed;
	float attack_power;
	float attack_range;
	float health_points = 100.f;
	float stopping_distance = 0.1f;
	float detected_player_distance;

	bool is_dead = false;
	bool is_attacking = false;
};

extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script

#endif