#ifndef _COMPONENTCAMERA_H_
#define _COMPONENTCAMERA_H_

#include "Component.h"
#include "Component/ComponentAABB.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentsUI;
class ComponentCamera : public Component
{
public:
	enum class ClearMode
	{
		COLOR = 0,
		SKYBOX = 1
	};

	ComponentCamera();
	ComponentCamera(GameObject * owner);

	~ComponentCamera();

	void Update() override;
	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	float GetWidth() const;
	float GetHeigt() const;

	void RecordFrame(const float width, const float height);
	GLuint GetLastRecordedFrame() const;

	void SetFOV(const float fov);
	void SetAspectRatio(const float aspect_ratio);
	void SetNearDistance(const float distance);
	void SetFarDistance(const float distance);
	void SetOrientation(const float3 orientation);
	void AlignOrientationWithAxis();
	void SetOrthographicSize(const float2 size);
	void LookAt(const float3 focus);
	void LookAt(const float x, const float y, const float z);

	void SetPosition(const float3 position);
	void MoveUp();
	void MoveDown();
	void MoveFoward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();

	void Center(const AABB &bounding_box);
	void Focus(const AABB &bounding_box);

	void OrbitCameraWithMouseMotion(const float2 &motion);
	void OrbitX(const float angle);
	void OrbitY(const float angle);

	void RotateCameraWithMouseMotion(const float2 &motion);
	void RotatePitch(const float angle);
	void RotateYaw(const float angle);

	void SetPerpesctiveView();
	void SetOrthographicView();

	void SetSpeedUp(const bool is_speeding_up);

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	float4x4 GetInverseClipMatrix() const;

	std::vector<float> GetFrustumVertices() const;
	
	bool ComponentCamera::IsInsideFrustum(const AABB& aabb) const;
	ComponentAABB::CollisionState CheckAABBCollision(const AABB& reference_AABB) const;

	bool ComponentCamera::IsInsideFrustum(const AABB2D& aabb) const;
	ComponentAABB::CollisionState CheckAABB2DCollision(const AABB2D& reference_AABB) const;

	void ShowComponentWindow() override;

private:
	void GenerateFrameBuffers(const float width, const float height);
	void GenerateMatrices();
	void InitCamera();

public:
	const float SPEED_UP_FACTOR = 2.f;

	const float FAR_PLANE_FACTOR = 25.f;
	const float BOUNDING_BOX_DISTANCE_FACTOR = 3.f;
	const float INITIAL_HEIGHT_FACTOR = 0.5f;
	const float CAMERA_MOVEMENT_SPEED_BOUNDING_BOX_RADIUS_FACTOR = 0.005f;
	const float CAMERA_ZOOMING_SPEED_BOUNDING_BOX_RADIUS_FACTOR = 0.0625f;
	const float CAMERA_MAXIMUN_MOVEMENT_SPEED = 1.0f;
	const float CAMERA_MINIMUN_MOVEMENT_SPEED = 0.005f;
	
	float camera_movement_speed = 0.15f;
	float camera_zooming_speed = 0.15f;
	float camera_rotation_speed = 0.000625f;

	float camera_clear_color[3] = {0.0f, 0.0f, 0.0f};

	float4x4 proj;
	float4x4 view;

private:
	Frustum camera_frustum;

	GLuint uniform_buffer = 0;
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint last_recorded_frame_texture = 0;

	float last_height = 0;
	float last_width = 0;

	float aspect_ratio = 1.f;
	float orthographic_fov_ratio = 3;
	int perpesctive_enable = 0;

	bool is_speeding_up = false;
	float speed_up = 1.f;

	bool is_focusing = false;
	float3 desired_focus_position = float3::zero;

	ClearMode camera_clear_mode = ClearMode::COLOR;
	friend class ComponentsUI;
};

#endif //_COMPONENTCAMERA_H_