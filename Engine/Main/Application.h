#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include "Module/Module.h"

#include <vector>
#include <memory>

class ModuleAI;
class ModuleAnimation;
class ModuleAudio;
class ModuleEffects;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleProgram;
class ModuleTexture;
class ModuleEditor;
class ModuleCamera;
class ModuleTime;
class ModuleScene;
class ModuleFileSystem;
class ModuleDebug;
class ModuleDebugDraw;
class ModuleUI;
class ModuleLight;
class ModuleResourceManager;
class ModuleScriptManager;
class ModuleSpacePartitioning;
class ModuleActions;
class ModulePhysics;

class EngineLog;

class TimerUs;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleAnimation* animations = nullptr;
	ModuleEffects* effects = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleProgram* program = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* cameras = nullptr;
	ModuleTime* time = nullptr;
	ModuleScene* scene = nullptr;
	ModuleFileSystem* filesystem = nullptr;
	ModuleDebug* debug = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;
	ModuleUI* ui = nullptr;
	ModuleLight* lights = nullptr;
	ModuleResourceManager* resources = nullptr;
	ModuleScriptManager* scripts = nullptr;
	ModuleSpacePartitioning* space_partitioning = nullptr;
	ModuleActions* actions = nullptr;
	ModuleAI* artificial_intelligence = nullptr;
	ModulePhysics* physics = nullptr;
	ModuleAudio* audio = nullptr;

	std::unique_ptr<EngineLog> engine_log = nullptr;

private:

	std::vector<Module*> modules;

};

extern Application* App;

#endif // _APPLICATION_H_