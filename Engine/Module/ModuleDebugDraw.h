#ifndef _MODULEDEBUGDRAW_H_
#define _MODULEDEBUGDRAW_H_

#define ENGINE_EXPORTS

#include "Module.h"

#include <GL/glew.h>
#include <MathGeoLib.h>

class Billboard;
class ComponentCamera;
class GameObject;
class Grid;
class IDebugDrawOpenGLImplementation;

class ModuleDebugDraw : public Module
{
public:
	ModuleDebugDraw() = default;
	~ModuleDebugDraw() = default;

	bool Init() override;
	bool CleanUp() override;

	void RenderDebugDraws(const ComponentCamera& camera);
	ENGINE_API void RenderSingleAABB(AABB& aabb) const;

	void RenderNavMesh(ComponentCamera& cam) const;
	void RenderQuadTree() const;
	void RenderOcTree() const;
	void RenderAABBTree() const;
	void RenderSelectedGameObjectHelpers() const;
	void RenderBoundingBoxes() const;
	void RenderGlobalBoundingBoxes() const;
	void RenderPathfinding() const;
	void RenderGrid() const;
	void RenderBillboards() const;
	void RenderTangentsAndBitangents() const;

private:
	void RenderOutline() const;

	void RenderCameraFrustum() const;
	void RenderLightGizmo() const;
	void RenderBones() const;
	void RenderBone(const GameObject* current_bone, const GameObject* last_bone, const float3& color) const;

private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

	const float MIN_MAGNITUDE_ORDER_GRID = 0;
	const float MAX_MAGNITUDE_ORDER_GRID = 20;

	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;

	Grid* grid = nullptr;
};

#endif //_MODULEDEBUGDRAW_H_