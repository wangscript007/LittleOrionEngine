#include "ModuleDebugDraw.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentMeshRenderer.h"

#include "EditorUI/Helper/Billboard.h"
#include "EditorUI/Helper/Grid.h"
#include "EditorUI/Panel/PanelNavMesh.h"

#include "Main/Application.h"
#include "Module/ModuleAI.h"
#include "ModuleAnimation.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleDebug.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "SpacePartition/OLQuadTree.h"

#define DEBUG_DRAW_IMPLEMENTATION
#include "EditorUI/DebugDraw.h"     // Debug Draw API. Notice that we need the DEBUG_DRAW_IMPLEMENTATION macro here!

#include <GL/glew.h>
#include <assert.h>
#include <Brofiler/Brofiler.h>

class IDebugDrawOpenGLImplementation final : public dd::RenderInterface
{
public:

    //
    // dd::RenderInterface overrides:
    //
	void openGLDraw(const dd::DrawVertex* points, int count, bool depthEnabled, GLenum mode)
	{
		assert(points != nullptr);
		assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);
		GLuint shader_program = App->program->GetShaderProgramId("Linepoint");
		glBindVertexArray(linePointVAO);
		glUseProgram(shader_program);

		glUniformMatrix4fv(
			glGetUniformLocation(shader_program, "u_MvpMatrix"),
			1, GL_TRUE, reinterpret_cast<const float*>(&mvpMatrix)
		);

		bool already = glIsEnabled(GL_DEPTH_TEST);

		if (depthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
		glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

		// Issue the draw call:
		glDrawArrays(mode, 0, count);

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		checkGLError(__FILE__, __LINE__);

		if (already)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
    void drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled) override
    {
		openGLDraw(points,count,depthEnabled,GL_POINTS);

    }

    void drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled) override
    {
		openGLDraw(lines, count, depthEnabled, GL_LINES);

    }

    void drawGlyphList(const dd::DrawVertex* glyphs, int count, dd::GlyphTextureHandle glyphTex) override
    {
        assert(glyphs != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

		GLuint shader_program = App->program->GetShaderProgramId("Text");
        glBindVertexArray(textVAO);
        glUseProgram(shader_program);

        // These doesn't have to be reset every draw call, I'm just being lazy ;)
        glUniform1i(
            glGetUniformLocation(shader_program, "u_glyphTexture"),
            0
        );
        glUniform2f(
            glGetUniformLocation(shader_program, "u_screenDimensions"),
            static_cast<GLfloat>(width),
            static_cast<GLfloat>(height)
        );

        if (glyphTex != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
        }

        bool already_blend = glIsEnabled(GL_BLEND);

        if (!already_blend)
        {
            glEnable(GL_BLEND);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        bool already = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

        glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

        if (!already_blend)
        {
            glDisable(GL_BLEND);
        }

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        if (already)
        {
            glEnable(GL_DEPTH_TEST);
        }
    }

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void* pixels) override
    {
        assert(width > 0 && height > 0);
        assert(pixels != nullptr);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        return GLToHandle(textureId);
    }

    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override
    {
        if (glyphTex == nullptr)
        {
            return;
        }

        const GLuint textureId = handleToGL(glyphTex);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

    // These two can also be implemented to perform GL render
    // state setup/cleanup, but we don't use them in this sample.
    //void beginDraw() override { }
    //void endDraw()   override { }

    //
    // Local methods:
    //

    IDebugDrawOpenGLImplementation()
        : mvpMatrix()
        , width(0)
        , height(0)
        , linePointVAO(0)
        , linePointVBO(0)
        , textVAO(0)
        , textVBO(0)
    {
        //std::printf("\n");
        //std::printf("GL_VENDOR    : %s\n",   glGetString(GL_VENDOR));
        //std::printf("GL_RENDERER  : %s\n",   glGetString(GL_RENDERER));
        //std::printf("GL_VERSION   : %s\n",   glGetString(GL_VERSION));
        //std::printf("GLSL_VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        //std::printf("IDebugDrawOpenGLImplementation initializing ...\n");

        // This has to be enabled since the point drawing shader will use gl_PointSize.
        glEnable(GL_PROGRAM_POINT_SIZE);

        setupVertexBuffers();

        //std::printf("IDebugDrawOpenGLImplementation ready!\n\n");
    }

    ~IDebugDrawOpenGLImplementation()
    {
        glDeleteVertexArrays(1, &linePointVAO);
        glDeleteBuffers(1, &linePointVBO);

        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
    }

    void setupVertexBuffers()
    {
        //std::printf("> IDebugDrawOpenGLImplementation::setupVertexBuffers()\n");

        //
        // Lines/points vertex buffer:
        //
        {
            glGenVertexArrays(1, &linePointVAO);
            glGenBuffers(1, &linePointVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(linePointVAO);
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

            // RenderInterface will never be called with a batch larger than
            // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by 3D points and lines:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec3)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 3;

            glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // VAOs can be a pain in the neck if left enabled...
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //
        // Text rendering vertex buffer:
        //
        {
            glGenVertexArrays(1, &textVAO);
            glGenBuffers(1, &textVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);

            // NOTE: A more optimized implementation might consider combining
            // both the lines/points and text buffers to save some memory!
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by the 2D text:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec2)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(1); // in_TexCoords (vec2)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(2); // in_Color (vec4)
            glVertexAttribPointer(
                /* index     = */ 2,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // Ditto.
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    static GLuint handleToGL(dd::GlyphTextureHandle handle)
    {
        const std::size_t temp = reinterpret_cast<std::size_t>(handle);
        return static_cast<GLuint>(temp);
    }

    static dd::GlyphTextureHandle GLToHandle(const GLuint id)
    {
        const std::size_t temp = static_cast<std::size_t>(id);
        return reinterpret_cast<dd::GlyphTextureHandle>(temp);
    }

    static void checkGLError(const char* file, int line)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            //errorF("%s(%d) : GL_CORE_ERROR=0x%X - %s", file, line, err, errorToString(err));
        }
    }

    // The "model-view-projection" matrix for the scene.
    // In this demo, it consists of the camera's view and projection matrices only.
    math::float4x4 mvpMatrix;
    unsigned width, height;

private:
    GLuint linePointVAO;
    GLuint linePointVBO;

    GLuint textVAO;
    GLuint textVBO;
}; // class IDebugDrawOpenGLImplementation


IDebugDrawOpenGLImplementation* ModuleDebugDraw::dd_interface_implementation = 0; // TODO: Ask why this is needed

// Called before render is available
bool ModuleDebugDraw::Init()
{
	APP_LOG_SECTION("************ Module Debug Draw Init ************");

	dd_interface_implementation = new IDebugDrawOpenGLImplementation();
    dd::initialize(dd_interface_implementation);

	light_billboard = new Billboard(LIGHT_BILLBOARD_TEXTURE_PATH, 1.72f, 2.5f);	
	camera_billboard = new Billboard(VIDEO_BILLBOARD_TEXTURE_PATH, 2.5f, 2.5f);

	grid = new Grid();

    APP_LOG_SUCCESS("Module Debug Draw initialized correctly.")

	return true;
}

void ModuleDebugDraw::Render()
{

	BROFILER_CATEGORY("Render Debug Draws", Profiler::Color::Lavender);
	if(App->debug->show_navmesh)
	{
		App->artificial_intelligence->RenderNavMesh(*App->cameras->scene_camera);
	}

	if (App->debug->show_quadtree)
	{
		BROFILER_CATEGORY("Render QuadTree", Profiler::Color::Lavender);

		for (auto& ol_quadtree_node : App->renderer->ol_quadtree.flattened_tree)
		{
			float3 quadtree_node_min = float3(ol_quadtree_node->box.minPoint.x, 0, ol_quadtree_node->box.minPoint.y);
			float3 quadtree_node_max = float3(ol_quadtree_node->box.maxPoint.x, 0, ol_quadtree_node->box.maxPoint.y);
			dd::aabb(quadtree_node_min, quadtree_node_max, float3::one);
		}
	}

	if (App->debug->show_octtree)
	{
		for (auto& ol_octtree_node : App->renderer->ol_octtree.flattened_tree)
		{
			float3 octtree_node_min = float3(ol_octtree_node->box.minPoint.x, ol_octtree_node->box.minPoint.y, ol_octtree_node->box.minPoint.z);
			float3 octtree_node_max = float3(ol_octtree_node->box.maxPoint.x, ol_octtree_node->box.maxPoint.y, ol_octtree_node->box.maxPoint.z);
			dd::aabb(octtree_node_min, octtree_node_max, float3::one);
		}
	}

	if(App->debug->show_aabbtree)
	{
		App->renderer->DrawAABBTree();
	}

	if (App->editor->selected_game_object != nullptr)
	{
		BROFILER_CATEGORY("Render Selected GameObject DebugDraws", Profiler::Color::Lavender);

		RenderCameraFrustum();
		RenderLightGizmo();
		RenderBones();
		RenderOutline(); // This function tries to render again the selected game object. It will fail because depth buffer
	}

	if (App->debug->show_bounding_boxes)
	{
		RenderBoundingBoxes();
	}

	if (App->debug->show_global_bounding_boxes)
	{
		RenderGlobalBoundingBoxes();
	}

	if(App->debug->show_pathfind_points)
	{
		RenderPathfinding();
	}

	RenderBillboards();
	RenderCanvas();


	if (App->debug->show_grid)
	{
		float scene_camera_height = App->cameras->scene_camera->owner->transform.GetGlobalTranslation().y;
		grid->ScaleOnDistance(scene_camera_height);
		grid->Render();
	}
	RenderDebugDraws(*App->cameras->scene_camera);
}

void ModuleDebugDraw::RenderCameraFrustum() const
{
	BROFILER_CATEGORY("Render Selected GameObject Camera Frustum", Profiler::Color::Lavender);

	if (!App->debug->show_camera_frustum)
	{
		return;
	}

	Component * selected_camera_component = App->editor->selected_game_object->GetComponent(Component::ComponentType::CAMERA);
	if (selected_camera_component != nullptr) {
		ComponentCamera* selected_camera = static_cast<ComponentCamera*>(selected_camera_component);

		dd::frustum(selected_camera->GetInverseClipMatrix(), float3::one);
	}	
}

void ModuleDebugDraw::RenderLightGizmo() const	
{	
	BROFILER_CATEGORY("Render Selected GameObject Light Gizmo", Profiler::Color::Lavender);

	Component* selected_light_component = App->editor->selected_game_object->GetComponent(Component::ComponentType::LIGHT);	
	if (selected_light_component != nullptr)
  {	
		ComponentLight* selected_light = static_cast<ComponentLight*>(selected_light_component);	
		GameObject* selected_gameobject = selected_light->owner;
		ComponentTransform* selected_light_transform = &selected_gameobject->transform;
		float gizmo_radius = 2.5f;	
		switch (selected_light->light_type)	
		{	
		case ComponentLight::LightType::DIRECTIONAL_LIGHT:	
			dd::directional_light(selected_light_transform->GetGlobalTranslation(), selected_light_transform->GetRotation().ToFloat4x4(), float3(1.f, 1.f, 0.f), 5.f, gizmo_radius);	
			break;	
		case ComponentLight::LightType::SPOT_LIGHT:	
			dd::spot_light(	
				selected_light_transform->GetGlobalTranslation(), 	
				selected_light_transform->GetRotation().ToFloat4x4(),	
				float3(1.f, 1.f, 0.f),	
				selected_light->spot_light_parameters.range / 10.f,	
				tan(DegToRad(selected_light->spot_light_parameters.spot_angle/2.f)) * selected_light->spot_light_parameters.range / 10.f
			); 	
			break;	
		case ComponentLight::LightType::POINT_LIGHT:	
			dd::point_light(selected_light_transform->GetGlobalTranslation(), float3(1.f, 1.f, 0.f), selected_light->point_light_parameters.range / 10.f);
			break;	
		default:	
			break;	
		}	
	}	
}	

void ModuleDebugDraw::RenderBones() const
{
	for (auto& animation : App->animations->animations)
	{
		if (animation->IsEnabled())
		{
			GameObject* animation_game_object = animation->owner;
			RenderBone(animation_game_object, nullptr, float3(1.f, 0.f, 0.f));
		}
	}

	
}

void ModuleDebugDraw::RenderBone(const GameObject* current_bone, const GameObject* last_bone, const float3& color) const
{
	if (current_bone->name.substr(current_bone->name.length() - 2) == "IK" || current_bone->name.substr(current_bone->name.length() - 2) == "FK")
	{
		return;
	}

	if (last_bone != nullptr)
	{
		dd::line(last_bone->transform.GetGlobalTranslation(), current_bone->transform.GetGlobalTranslation(), color);
	}

	for (auto& child_bone : current_bone->children)
	{
		float3 next_color;
		if (color.x == 1.f)
			next_color = float3(0.f, 1.f, 0.f);
		if (color.y == 1.f)
			next_color = float3(0.f, 0.f, 1.f);
		if (color.z == 1.f)
			next_color = float3(1.f, 0.f, 0.f);
		RenderBone(child_bone, current_bone, next_color);
	}
}

void ModuleDebugDraw::RenderOutline() const
{
	BROFILER_CATEGORY("Render Outline", Profiler::Color::Lavender);

	GameObject* selected_game_object = App->editor->selected_game_object;
	Component* selected_object_mesh_component = selected_game_object->GetComponent(Component::ComponentType::MESH_RENDERER);

	if (selected_object_mesh_component != nullptr && selected_object_mesh_component->IsEnabled())
	{
		BROFILER_CATEGORY("Render Outline Write Stencil", Profiler::Color::Lavender);

		ComponentMeshRenderer* selected_object_mesh = static_cast<ComponentMeshRenderer*>(selected_object_mesh_component);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glStencilMask(0xFF);

		selected_object_mesh->Render();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		BROFILER_CATEGORY("Render Outline Read Stencil", Profiler::Color::Lavender);

		GLuint outline_shader_program = App->program->GetShaderProgramId("Outline");
		glUseProgram(outline_shader_program);
		float4x4 new_transformation_matrix;
		if (selected_game_object->parent != nullptr)
		{
			new_transformation_matrix = selected_game_object->parent->transform.GetGlobalModelMatrix() * selected_game_object->transform.GetModelMatrix() * float4x4::Scale(float3(1.01f));

		ComponentTransform object_transform_copy = selected_game_object->transform;
		float3 object_scale = object_transform_copy.GetScale();
		object_transform_copy.SetScale(object_scale*1.01f);
		object_transform_copy.GenerateGlobalModelMatrix();
		}
		else 
		{
			new_transformation_matrix =  selected_game_object->transform.GetGlobalModelMatrix() * float4x4::Scale(float3(1.01f));
		}

		
		ModuleRender::DrawMode last_draw_mode = App->renderer->draw_mode;
		App->renderer->SetDrawMode(ModuleRender::DrawMode::WIREFRAME);
		glLineWidth(15.f);

		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), selected_game_object->transform.GetGlobalModelMatrix().Transposed().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		selected_object_mesh->RenderModel();

		glLineWidth(1.f);
		App->renderer->SetDrawMode(last_draw_mode);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);

		glUseProgram(0);

	}
}

void ModuleDebugDraw::RenderBoundingBoxes() const
{
	BROFILER_CATEGORY("Render Bounding Boxes", Profiler::Color::Lavender);

	for (auto& mesh : App->renderer->meshes_to_render)
	{
		GameObject* mesh_game_object = mesh->owner;
		if (!mesh_game_object->aabb.IsEmpty())
		{
			dd::aabb(mesh_game_object->aabb.bounding_box.minPoint, mesh_game_object->aabb.bounding_box.maxPoint, float3::one);
		}
	}
}

void ModuleDebugDraw::RenderGlobalBoundingBoxes() const
{
	BROFILER_CATEGORY("Render Global Bounding Boxes", Profiler::Color::Lavender);

	for (auto& object : App->scene->game_objects_ownership)
	{
		dd::aabb(object->aabb.global_bounding_box.minPoint, object->aabb.global_bounding_box.maxPoint, float3::one);
	}
}

void ModuleDebugDraw::RenderBillboards() const
{
	BROFILER_CATEGORY("Render Billboards", Profiler::Color::Lavender);

	for (auto& object : App->scene->game_objects_ownership)
	{
		Component * light_component = object->GetComponent(Component::ComponentType::LIGHT);
		if (light_component != nullptr) {
			light_billboard->Render(object->transform.GetGlobalTranslation());
		}

		Component * camera_component = object->GetComponent(Component::ComponentType::CAMERA);
		if (camera_component != nullptr) {
			camera_billboard->Render(object->transform.GetGlobalTranslation());
		}
	}
}

void ModuleDebugDraw::RenderCanvas() const
{
	/*for(auto &canvas: App->ui->canvases)
	{
		dd::box(canvas->owner->transform.GetTranslation(), float3::one, App->window->GetWidth() * 0.25f, App->window->GetHeight() * 0.25f, 0.01f);
		dd::circle(canvas->owner->transform.GetTranslation(), float3(0, 0, 1), float3::one, 1.0f, 10);
	}*/
}

void ModuleDebugDraw::RenderPathfinding() const
{
	//First check if starting and ending point are null and render
	if(App->artificial_intelligence->start_initialized)
	{
		dd::point(App->artificial_intelligence->start_position, float3(0, 255, 0), 20.0f);
	}

	if (App->artificial_intelligence->end_initialized)
	{
		dd::point(App->artificial_intelligence->end_position, float3(0, 255, 255), 20.0f);
	}

	for(auto point : App->artificial_intelligence->debug_path)
	{
		dd::point(point, float3(0, 0, 255), 10.0f);
	}
}

void ModuleDebugDraw::RenderDebugDraws(const ComponentCamera& camera)
{
	BROFILER_CATEGORY("Flush Debug Draw", Profiler::Color::Lavender);

	math::float4x4 view = camera.GetViewMatrix();
	math::float4x4 proj = camera.GetProjectionMatrix();

	dd_interface_implementation->width = static_cast<unsigned int>(camera.GetWidth());
	dd_interface_implementation->height = static_cast<unsigned int>(camera.GetHeight());
	dd_interface_implementation->mvpMatrix = proj * view;

	dd::flush();

}

// Called before quitting
bool ModuleDebugDraw::CleanUp()
{
	APP_LOG_INFO("Destroying Module Debug Draw");

    dd::shutdown();

    delete dd_interface_implementation;
    dd_interface_implementation = 0;

	delete light_billboard;
	delete camera_billboard;
	delete grid;

	return true;
}