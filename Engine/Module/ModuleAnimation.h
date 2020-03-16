#ifndef _MODULEANIMATION_H_
#define _MODULEANIMATION_H_

#include "Module.h"

class ComponentAnimation;
class GameObject;

class ModuleAnimation : public Module
{
public:
	ModuleAnimation() = default;
	~ModuleAnimation() = default;
	
	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;
	
	ComponentAnimation* CreateComponentAnimation();
	void RemoveComponentAnimation(ComponentAnimation* animation_to_remove);

private:
	std::vector<ComponentAnimation*> animations;
};

#endif //_MODULEANIMATION_H_