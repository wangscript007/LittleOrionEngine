#ifndef _EDITORACTIONMODIFYLIGHT_H_
#define _EDITORACTIONMODIFYLIGHT_H_

#include "EditorAction.h"
#include "Component/ComponentLight.h"

class EditorActionModifyLight : public EditorAction
{
public:
	EditorActionModifyLight(ComponentLight* comp, float col[3], float intensity);
	~EditorActionModifyLight();

	void Undo();
	void Redo();

private:
	ComponentLight* component;
	float color[3];
	float intensity;

};

#endif _EDITORACTIONMODIFYLIGHT_H_