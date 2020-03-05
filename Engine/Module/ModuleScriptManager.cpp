#include "ModuleScriptManager.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Component/ComponentScript.h"
#include "Script/Script.h"

#include <iostream>
#include <fstream>
#include <string>


bool ModuleScriptManager::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	//TODO: Load all the .dll
	gameplay_dll = LoadLibrary("GamePlaySystem.dll");
	//TODO: fill / load the component script vector.
	LoadScriptList();
	//InitResourceScript();
	return true;
}

update_status ModuleScriptManager::Update()
{
	if (!scripts.empty()) {
		for (auto &component_script : scripts) {
			component_script->Update();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	FreeLibrary(gameplay_dll);
	return true;
}

void ModuleScriptManager::InitResourceScript() 
{
	if (gameplay_dll != nullptr)
	{
		for (auto &component_script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (component_script->name + "DLL").c_str());
			if (script_func != nullptr)
			{
				component_script->script = script_func();
				component_script->script->AddReferences(component_script->owner, App);
			}
		}
	}
}

Script* ModuleScriptManager::CreateResourceScript( const std::string& script_name, GameObject* owner) 
{
	if (gameplay_dll != nullptr)
	{
		CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (script_name+"DLL").c_str());
		if (script_func != nullptr)
		{
			Script* script = script_func();
			script->AddReferences(owner, App);
			return script;
		}
		return nullptr;
	}
	return nullptr;
}

ComponentScript* ModuleScriptManager::CreateComponentScript()
{
	ComponentScript* new_script = new ComponentScript();
	scripts.push_back(new_script);
	return new_script;
}

void ModuleScriptManager::RemoveComponentScript(ComponentScript * script_to_remove)
{
	auto it = std::find(scripts.begin(), scripts.end(), script_to_remove);
	if (it != scripts.end())
	{
		delete *it;
		scripts.erase(it);
	}
}
void ModuleScriptManager::LoadScriptList() 
{
	/*std::ifstream file_scripts(SCRIPT_LIST_PATH);
	if (!file_scripts)
		return;
	scripts_list.clear();
	std::string script;
	while (std::getline(file_scripts, script)) 
	{
		if (script.size() > 0) 
		{
			scripts_list.push_back(script);
		}
	}*/if(scripts_list.size()>0)
		scripts_list.clear();
	size_t readed_bytes;
	char* scripts_file_data = App->filesystem->Load(SCRIPT_LIST_PATH, readed_bytes);
	if (scripts_file_data != nullptr)
	{
		std::string serialized_scripts_string = scripts_file_data;
		free(scripts_file_data);

		Config scripts_config(serialized_scripts_string);

		std::vector<Config> scripts_list_configs;
		scripts_config.GetChildrenConfig("Scripts", scripts_list_configs);
		for (unsigned int i = 0; i < scripts_list_configs.size(); ++i)
		{
			std::string script;
			scripts_list_configs[i].GetString("Script", script,"");
			scripts_list.push_back(script);
		}
	}

}

