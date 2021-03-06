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
	ENGINE_API void RenderBoundingBoxes() const;
	ENGINE_API void RenderGlobalBoundingBoxes() const;

	ENGINE_API void RenderNavMesh(ComponentCamera& cam) const;

	ENGINE_API void RenderPathfinding() const;
	ENGINE_API void RenderQuadTree() const;
	ENGINE_API void RenderOcTree() const;
	ENGINE_API void RenderAABBTree() const;
	
	void RenderSelectedGameObjectHelpers() const;
	
	void RenderOutline() const;
	void RenderGrid() const;
	void RenderBillboards() const;
	void RenderTangentsAndBitangents() const;
	
	void RenderRectTransform(const GameObject* rect_owner) const;
	ENGINE_API void RenderLine(float3& a, float3& b) const;
	ENGINE_API void RenderPoint(const float3& point, float size= 1.f) const;

	ENGINE_API void RenderPhysics() const;

private:

	void RenderCameraFrustum() const;
	void RenderParticleSystem() const;
	void RenderLightGizmo() const;
	void RenderBones(GameObject * game_object) const;

private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

	const float MIN_MAGNITUDE_ORDER_GRID = 0;
	const float MAX_MAGNITUDE_ORDER_GRID = 20;

	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;

	Grid* grid = nullptr;
};

#endif //_MODULEDEBUGDRAW_H_
