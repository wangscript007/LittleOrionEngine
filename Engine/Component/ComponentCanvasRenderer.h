#ifndef _COMPONENTCANVASRENDERER_H_
#define _COMPONENTCANVASRENDERER_H_

#include "Component.h"

class GameObject;

class ComponentCanvasRenderer : public Component
{
public:
	ComponentCanvasRenderer();
	ComponentCanvasRenderer(GameObject * owner);
	~ComponentCanvasRenderer() = default;

	ComponentCanvasRenderer & operator=(const ComponentCanvasRenderer& component_to_copy);
	ComponentCanvasRenderer & operator=(ComponentCanvasRenderer&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Delete() override;

	void Render(float4x4* projection);

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;
};

#endif // !_COMPONENTCANVASRENDERER_H_