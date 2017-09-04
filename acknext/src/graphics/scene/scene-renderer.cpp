#include <engine.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ackglm.hpp"
#include "../../scene/entity.hpp"
#include "../opengl/shader.hpp"

#include "../debug/debugdrawer.hpp"

#define LIGHT_LIMIT 16

extern Shader * defaultShader;

Shader * FB(Shader * sh)
{
	if(sh) return sh;
	return defaultShader;
}

struct LIGHTDATA
{
	__attribute__((aligned(4))) int type;
	__attribute__((aligned(4))) float intensity;
	__attribute__((aligned(4))) float arc;
	__attribute__((aligned(16))) VECTOR position;
	__attribute__((aligned(16))) VECTOR direction;
	__attribute__((aligned(16))) COLOR color;
};

static BUFFER * ubo = nullptr;

ACKNEXT_API_BLOCK
{
	CAMERA * camera;

	COLOR sky_color = { 0.3, 0.7, 1.0, 1.0 };

	void render_scene_with_camera(CAMERA * perspective)
	{
		if(perspective == nullptr) {
			return;
		}

		if(!ubo)
		{
			ubo = buffer_create(UNIFORMBUFFER);
			buffer_set(ubo, sizeof(LIGHTDATA) * LIGHT_LIMIT, nullptr);
		}

		// glEnable(GL_CULL_FACE);
		// glCullFace(GL_BACK);

		if(opengl_wireFrame) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(sky_color.red, sky_color.green, sky_color.blue, sky_color.alpha);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: add current_view for rendering steps
		MATRIX matView, matProj;
		camera_to_matrix(perspective, &matView, &matProj, NULL);

		for(ENTITY * ent = ent_next(nullptr); ent != nullptr; ent = ent_next(ent))
		{
			// Entity * entity = promote<Entity>(ent);
			if(ent->model == nullptr) {
				continue;
			}
			// TODO: Filter entity by mask bits

			MATRIX matWorld;
			glm_to_ack(&matWorld,
				glm::translate(glm::mat4(), ack_to_glm(ent->position)) *
				glm::mat4_cast(ack_to_glm(ent->rotation)) *
				glm::scale(glm::mat4(), ack_to_glm(ent->scale)));

			if(ent->material != nullptr) {
				opengl_setMaterial(ent->material);
			}

			Model * model = promote<Model>(ent->model);

			for(uint i = 0; i < model->api().meshCount; i++)
			{
				MESH const * mesh = model->api().meshes[i];
				MATERIAL const * mtl = model->api().materials[i];

				if(ent->material == nullptr) {
					opengl_setMaterial(mtl);
				}

				opengl_setTransform(
					&matWorld,
					&matView,
					&matProj);

				// opengl_setLights() {
				int lcount = 0;
				LIGHTDATA * lights = (LIGHTDATA*)glMapNamedBuffer(
				            ubo->object,
							GL_WRITE_ONLY);
				for(LIGHT * l = light_next(nullptr); l != nullptr; l = light_next(l))
				{
					lights[lcount].type = l->type;
					lights[lcount].intensity = l->intensity;
					lights[lcount].arc = DEG_TO_RAD * l->arc;
					lights[lcount].position = l->position;
					lights[lcount].direction = l->direction;
					lights[lcount].color = l->color;

					vec_normalize(&lights[lcount].direction, 1.0);
					lcount += 1;
					if(lcount >= LIGHT_LIMIT) {
						break;
					}
				}
				glUnmapNamedBuffer(ubo->object);

				GLuint binding_point_index = 2;
				GLint block_index = glGetUniformBlockIndex(
					defaultShader->api().object,
					"LightBlock");
				glBindBufferBase(
					GL_UNIFORM_BUFFER,
					binding_point_index,
					ubo->object);
				glUniformBlockBinding(
					defaultShader->api().object,
					block_index,
					binding_point_index);
				defaultShader->iLightCount = lcount;

				defaultShader->vecViewPos = perspective->position;

				// }
				// void opengl_setLights();

				opengl_drawMesh(mesh);
			}
		}
		DebugDrawer::render(matView, matProj);
	}
}
