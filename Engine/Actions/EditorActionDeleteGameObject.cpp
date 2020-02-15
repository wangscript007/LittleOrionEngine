#include "EditorActionDeleteGameObject.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionDeleteGameObject::~EditorActionDeleteGameObject()
{
	if(!game_object->IsEnabled())
		App->scene->RemoveGameObject(game_object);
}

void EditorActionDeleteGameObject::Undo()
{
	game_object->SetEnabled(true);
	parent->children.push_back(game_object);
	game_object->parent = parent;
	game_object->SetHierarchyDepth(hierarchy_depth);
}

void EditorActionDeleteGameObject::Redo()
{
	game_object->SetEnabled(false);
	game_object->parent->RemoveChild(game_object);
	game_object->SetHierarchyDepth(0);
}

