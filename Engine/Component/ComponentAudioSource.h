#ifndef _COMPONENTAUDIOSOURCE_H_
#define _COMPONENTAUDIOSOURCE_H_

#include "Component.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <AK/SoundEngine/Common/AkTypes.h>
#include <unordered_map>

class GameObject;
class SoundBank;
class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource();
	ComponentAudioSource(GameObject* owner);
	~ComponentAudioSource() = default;

	void Init() override;
	void Update() override;
	void Delete() override;

	void SetSoundBank(uint32_t uuid);
	void SetVolume(float volume);
	ENGINE_API unsigned long PlayEvent(const std::string & event_to_play);
	ENGINE_API void StopEvent(const std::string & event_to_stop);
	ENGINE_API void StopEvent(unsigned long playing_id_to_stop);
	ENGINE_API void StopAll();

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

private:
	AkSoundPosition sound_position;
	AkGameObjectID gameobject_source = 0;
	std::shared_ptr<SoundBank> soundbank;
	std::unordered_map<AkUInt32,AkPlayingID> event_playing_ids;


	bool sound_3d = false;
	float volume = 1;
	friend class PanelComponent;
};

#endif //_COMPONENTAUDIOSOURCE_H_