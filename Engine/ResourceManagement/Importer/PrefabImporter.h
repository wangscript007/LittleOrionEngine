#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"

class ComponentMeshRenderer;
class GameObject;
class Config;
class ModelMetafile;

class PrefabImporter : public Importer
{
public:
	PrefabImporter() : Importer(ResourceType::PREFAB) {};
	~PrefabImporter() = default;

	FileData ExtractData(Path& file_path, const Metafile& metafile) const override;
	FileData ExtractFromModel(const Config& model_config, const ModelMetafile& metafile) const;
	FileData ExtractFromGameObject(GameObject* gameobject, bool overwritable = true) const;

private:
	static void ExtractGameObjectFromNode(
		std::unique_ptr<GameObject>& parent_node, 
		const Config& node_config, 
		std::vector<std::unique_ptr<GameObject>>& game_objects,
		std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
		std::vector<uint32_t>& loaded_skeletons,
		const ModelMetafile& metafile
	);
	static void ExtractMeshComponent(
		uint32_t mesh_uuid, uint32_t material_uuid, uint32_t skeleton_uuid,
		std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
		GameObject* node_game_object
	);
	static void ExtractAnimationComponent(GameObject* node_game_object, const Config& node_config);
};

#endif