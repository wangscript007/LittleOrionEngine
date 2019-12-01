#include "ModuleCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Component/ComponentCamera.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>

#include <algorithm>

bool ModuleCamera::Init()
{
	APP_LOG_SECTION("************ Module Camera Init ************");
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	
	scene_camera = CreateComponentCamera();

	speed_up = 1;
	is_orbiting = false;

	return true;
}

// Called every draw update
update_status ModuleCamera::Update()
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	delete scene_camera;
	return true;
}

ComponentCamera* ModuleCamera::CreateComponentCamera()
{

	ComponentCamera * new_camera = new ComponentCamera();
	cameras.push_back(new_camera);
	return new_camera;
}

void ModuleCamera::RemoveComponentCamera(ComponentCamera* camera_to_remove)
{

	auto it = std::remove_if(cameras.begin(), cameras.end(), [camera_to_remove](auto const & camera)
	{
		return camera == camera_to_remove;
	});
	cameras.erase(it);
}

void ModuleCamera::SetOrbit(const bool is_orbiting)
{
	this->is_orbiting = is_orbiting;
}

bool ModuleCamera::IsOrbiting() const
{
	return is_orbiting;
}

void ModuleCamera::SetMovement(const bool movement_enabled)
{
	this->movement_enabled = movement_enabled;
}

bool ModuleCamera::IsMovementEnabled() const
{
	return movement_enabled;
}

void ModuleCamera::ShowGameWindow() 
{
	if (ImGui::Begin(ICON_FA_TH " Scene"))
	{
		game_window_is_hovered = ImGui::IsWindowHovered();

		float imgui_window_width = ImGui::GetWindowWidth();
		float imgui_window_height = ImGui::GetWindowHeight();
		scene_camera->RecordFrame(imgui_window_width, imgui_window_height);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)scene_camera->GetLastRecordedFrame(),
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(
				ImGui::GetCursorScreenPos().x + imgui_window_width,
				ImGui::GetCursorScreenPos().y + imgui_window_height
			),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}
	if (App->cameras->IsMovementEnabled() && game_window_is_hovered) // CHANGES CURSOR IF SCENE CAMERA MOVEMENT IS ENABLED
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}
	ImGui::End();
}
void ModuleCamera::ShowCameraOptions()
{
	scene_camera->ShowComponentWindow();
}