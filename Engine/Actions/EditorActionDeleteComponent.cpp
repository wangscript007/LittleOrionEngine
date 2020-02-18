#include "EditorActionDeleteComponent.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Main/Application.h"
#include "Module/ModuleScene.h"

EditorActionDeleteComponent::EditorActionDeleteComponent(Component* comp)
{
	UUID_COMP = comp->UUID;
	comp->Save(serialization_component);
	
}


EditorActionDeleteComponent::~EditorActionDeleteComponent()
{
	Component* component = App->scene->GetComponent(UUID_COMP);
	if(component != nullptr && !component->IsEnabled())
		component->Delete();
}

void EditorActionDeleteComponent::Undo()
{
	//TODO: Save which type of component it is and then do a switch creating the specific component type
	Component* component = App->scene->GetComponent(UUID_COMP);
	component->Enable();
	component->owner->components.push_back(component);

	if (component->type == Component::ComponentType::LIGHT)
	{
		App->lights->lights.push_back((ComponentLight*)component);
	}
}

void EditorActionDeleteComponent::Redo()
{
	Component* component = App->scene->GetComponent(UUID_COMP);
	component->Disable();
	auto it = std::find(component->owner->components.begin(), component->owner->components.end(), component);
	component->owner->components.erase(it);


	if (component->type == Component::ComponentType::LIGHT)
	{
		auto it = std::find(App->lights->lights.begin(), App->lights->lights.end(), (ComponentLight*)component);
		if (it != App->lights->lights.end())
		{
			App->lights->lights.erase(it);
		}
	}
}
