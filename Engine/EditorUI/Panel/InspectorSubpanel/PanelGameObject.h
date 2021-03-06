#ifndef _PANELGAMEOBJECT_H_
#define _PANELGAMEOBJECT_H_

#include "EditorUI/Panel/Panel.h"
#include "PanelComponent.h"

class GameObject;

class PanelGameObject : public Panel
{
public:
	PanelGameObject();
	~PanelGameObject() = default;

	void Render(GameObject* game_object);
	void ShowPrefabMenu(GameObject* game_object);
public:
	PanelComponent component_panel;

};

#endif //_PANELGAMEOBJECT_H_

