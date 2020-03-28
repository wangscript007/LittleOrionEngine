#include "AnimController.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "Helper/Utils.h"
#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"

#include <math.h>
#include <algorithm>

class ComponentMeshRenderer;

void AnimController::Init()
{
	animation_time = (animation->frames/animation->frames_per_second )* 1000;
	/*
	for (size_t i = 0; i < animation->keyframes[0].channels.size(); ++i)
	{
		auto & channel = animation->keyframes[0].channels[i];
		auto it = std::find_if(skeleton->skeleton.begin(), skeleton->skeleton.end(), [channel](const Skeleton::Joint & joint)
		{
			return joint.name == channel.name;
		});
		if (it != skeleton->skeleton.end())
		{
			Skeleton::Joint joint = (*it);
			while (joint.parent_index < skeleton->skeleton.size())
			{
				joint = skeleton->skeleton[joint.parent_index];
				auto it_channel = std::find_if(animation->keyframes[0].channels.begin(), animation->keyframes[0].channels.end(), [&joint](const Animation::Channel & parent_channel)
				{
					return joint.name == parent_channel.name;
				});
				if (it_channel != animation->keyframes[0].channels.end())
				{
					channel_hierarchy_cache[i].push_back(it_channel - animation->keyframes[0].channels.begin());
				}
			}
		}

	}
	channel_global_transformation.resize(animation->keyframes[0].channels.size());*/

}


void AnimController::Play()
{ 
	current_time = 0.f;
	playing = true;
}

void AnimController::Stop()
{
	playing = false;
}

void AnimController::Update()
{
	if (!playing)
	{
		return;
	}

	current_time = current_time + App->time->delta_time;
	if (current_time >= animation_time)
	{
		if (loop) 
		{

			current_time = current_time % animation_time;
		}
		else
		{
			playing = false;
		}
	}
		//UpdateChannelsGlobalTransformation();

}

bool AnimController::GetKeyframes(int & first_keyframe_index, int & second_keyframe_index, float & interpolationLambda)
{
	float current_sample = (current_time*(animation->frames - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	bool found_keyframe = false;
	for (size_t i = 0; !found_keyframe && i < animation->keyframes.size(); i++)
	{
		if (animation->keyframes[i].frame == current_keyframe)
		{
			if (i + 1 == animation->keyframes.size())
			{
				if (this->loop)
				{
					first_keyframe_index = i;
					second_keyframe_index = (i + 1) % (int)animation->frames;
				}
				else
				{
					first_keyframe_index = second_keyframe_index = i;
				}
			}

			else
			{
				first_keyframe_index = i;
				second_keyframe_index = i + 1;
			}
			found_keyframe = true;
			break;
		}

		if (animation->keyframes[i].frame > current_keyframe)
		{
			first_keyframe_index = i - 1;
			second_keyframe_index = i;

			found_keyframe = true;
			break;
		}
	}
	if (!found_keyframe)
		return false;

	interpolationLambda = (current_sample - animation->keyframes[first_keyframe_index].frame)
		/ (animation->keyframes[second_keyframe_index].frame - animation->keyframes[first_keyframe_index].frame);

	return true;
}

bool AnimController::GetTransform(const std::string & channel_name, float3 & position, Quat & rotation, const int first_keyframe_index, const int second_keyframe_index, const float interpolationLambda)
{
	bool channel_found = false;
	size_t channel_index = 0;
	while (!channel_found && channel_index < animation->keyframes[0].channels.size())
	{
		if (animation->keyframes[0].channels[channel_index].name == channel_name)
		{
			channel_found = true;
			break;
		}
		++channel_index;
	}
	if (!channel_found)
		return false;

	float3 last_translation = animation->keyframes[first_keyframe_index].channels[channel_index].translation;
	float3 next_translation = animation->keyframes[second_keyframe_index].channels[channel_index].translation;

	Quat last_rotation = animation->keyframes[first_keyframe_index].channels[channel_index].rotation;
	Quat next_rotation = animation->keyframes[second_keyframe_index].channels[channel_index].rotation;

	position = Utils::Interpolate(last_translation, next_translation, interpolationLambda);
	rotation = Utils::Interpolate(last_rotation, next_rotation, interpolationLambda);

	return true;
}

void AnimController::UpdateChannelsGlobalTransformation()
{
	float current_sample = (current_time*(animation->frames - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1) % (int)animation->frames;

	size_t i = 0;
	for (const auto & channel : animation->keyframes[current_keyframe].channels)
	{
		Animation::Channel & next_keyframe_channel = animation->keyframes[next_keyframe].channels[i];

		float4x4 current_global_tranform = float4x4::identity;
		float4x4 next_global_tranform = float4x4::identity;

		std::vector<Animation::Channel> & channels = animation->keyframes[current_keyframe].channels;
		std::vector<Animation::Channel> & netx_keyframe_channels = animation->keyframes[next_keyframe].channels;

		for (int j = channel_hierarchy_cache[i].size() - 1; j >= 0; --j)
		{
			auto & channel_parent = channels[channel_hierarchy_cache[i][j]];
			current_global_tranform = current_global_tranform * float4x4::FromTRS(channel_parent.translation, channel_parent.rotation, float3(1.0f, 1.0f, 1.0f));

			auto & channel_parent_next_keyframe = netx_keyframe_channels[channel_hierarchy_cache[i][j]];
			next_global_tranform = next_global_tranform * float4x4::FromTRS(channel_parent_next_keyframe.translation, channel_parent_next_keyframe.rotation, float3(1.0f, 1.0f, 1.0f));
		}
		float4x4 channel_current_local_tranform = float4x4::FromTRS(channel.translation, channel.rotation, float3(1.0f, 1.0f, 1.0f));
		float4x4 channel_next_local_tranform = float4x4::FromTRS(next_keyframe_channel.translation, next_keyframe_channel.rotation, float3(1.0f, 1.0f, 1.0f));

		current_global_tranform = current_global_tranform * channel_current_local_tranform;
		next_global_tranform = next_global_tranform * channel_next_local_tranform;

		float delta = current_sample - current_keyframe;
		channel_global_transformation[i] = Utils::Interpolate(current_global_tranform, next_global_tranform, delta);
		++i;
	}

}
