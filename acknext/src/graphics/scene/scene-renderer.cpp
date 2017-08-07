#include <engine.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ackglm.hpp"
#include "../../scene/entity.hpp"

ACKNEXT_API_BLOCK
{
	CAMERA * camera;

	COLOR sky_color = { 0.3, 0.7, 1.0, 1.0 };

	void render_scene_with_camera(CAMERA * perspective)
	{
		if(perspective == nullptr) {
			return;
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

			for(MESH & mesh : model->meshes)
			{
				opengl_setIndexBuffer(mesh.indexBuffer);
				opengl_setVertexBuffer(mesh.vertexBuffer);
				if(ent->material == nullptr) {
					opengl_setMaterial(mesh.material);
				}

				opengl_setTransform(
					&matWorld,
					&matView,
					&matProj);

				opengl_draw(GL_TRIANGLES, 0, mesh.indexBuffer->size / sizeof(INDEX));
			}
		}
	}
}
