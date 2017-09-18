#include <engine.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ackglm.hpp"
#include "../../scene/entity.hpp"
#include "../opengl/shader.hpp"

#include "../debug/debugdrawer.hpp"

#include <vector>
#include <algorithm>

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
static BUFFER * bonesBuf = nullptr;

extern Shader * currentShader;

static void setupLights()
{
	GLint block_index = glGetUniformBlockIndex(
		currentShader->api().object,
		"LightBlock");
	if(block_index >= 0) // only when lights are required
	{
		int lcount = 0;
		LIGHTDATA * lights = (LIGHTDATA*)glMapNamedBuffer(
					ubo->object,
					GL_WRITE_ONLY);
		for(LIGHT * l = light_next(nullptr); l != nullptr; l = light_next(l))
		{
			lights[lcount].type = l->type;
			lights[lcount].intensity = l->intensity;
			lights[lcount].arc = cos(0.5 * DEG_TO_RAD * l->arc); // arc is full arc, but cos() is half-arc
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
		glBindBufferBase(
			GL_UNIFORM_BUFFER,
			binding_point_index,
			ubo->object);
		glUniformBlockBinding(
			currentShader->api().object,
			block_index,
			binding_point_index);
		currentShader->iLightCount = lcount;
	} else {
		currentShader->iLightCount = 0;
	}
}

static void setupBones()
{
	GLint block_index = glGetUniformBlockIndex(
		currentShader->api().object,
		"BoneBlock");
	if(block_index >= 0)
	{
		GLuint binding_point_index = 4;
		glBindBufferBase(
			GL_UNIFORM_BUFFER,
			binding_point_index,
			bonesBuf->object);
		glUniformBlockBinding(
			currentShader->api().object,
			block_index,
			binding_point_index);
	}
}

struct Drawcall
{
	ENTITY const * ent = nullptr;
	MODEL const * model = nullptr;
	MESH const * mesh = nullptr;
	MATERIAL const * material = nullptr;
	MATRIX matWorld;
	bool renderDoubleSided;

	Drawcall() = default;
	Drawcall(Drawcall const &) = default;
	Drawcall(Drawcall &&) = default;
	~Drawcall() = default;

	Drawcall & operator =(Drawcall const &) = default;

	void exec()
	{
		opengl_drawMesh(mesh);
	}

	bool operator < (Drawcall const& other) const
	{
#define I(ptr) reinterpret_cast<uintptr_t>(ptr)
		if(I(material) < I(other.material))
			return true;
		if(I(mesh) < I(other.mesh))
			return true;
		return false;
#undef I
	}
};

static bool cull(MATRIX const & frustrum, AABB const & aabb)
{
	if((aabb.minimum.x > aabb.maximum.x) || (aabb.minimum.y > aabb.maximum.y) || (aabb.minimum.z > aabb.maximum.z))
	{
		return false;
	}

	VECTOR4 points[8] = {
	    { aabb.minimum.x, aabb.minimum.y, aabb.minimum.z, 1 },
	    { aabb.minimum.x, aabb.minimum.y, aabb.maximum.z, 1 },
	    { aabb.minimum.x, aabb.maximum.y, aabb.minimum.z, 1 },
	    { aabb.minimum.x, aabb.maximum.y, aabb.maximum.z, 1 },
	    { aabb.maximum.x, aabb.minimum.y, aabb.minimum.z, 1 },
	    { aabb.maximum.x, aabb.minimum.y, aabb.maximum.z, 1 },
	    { aabb.maximum.x, aabb.maximum.y, aabb.minimum.z, 1 },
	    { aabb.maximum.x, aabb.maximum.y, aabb.maximum.z, 1 },
	};
	for(int i = 0; i < 8; i++)
	{
		vec4_transform(&points[i], &frustrum);
		points[i].x /= points[i].w;
		points[i].y /= points[i].w;
		points[i].z /= points[i].w;
	}

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.x < -1.0);
			}))
		return true;

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.x > 1.0);
			}))
		return true;

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.y < -1.0);
			}))
		return true;

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.y > 1.0);
			}))
		return true;

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.z < -1.0);
			}))
		return true;

	if(std::all_of(&points[0], &points[8], [](VECTOR4 const & pt) -> bool {
				return (pt.z > 1.0);
			}))
		return true;

	return false;
}

ACKNEXT_API_BLOCK
{
	CAMERA * camera;

	COLOR sky_color = { 0.3, 0.7, 1.0, 1.0 };

	var lod_distances[16] =
	{
		0.5,
	    1.0,
	    5.0,
	    10.0,
	    15.0,
	    25.0,
	    50.0,
	    75.0,
	    100.0,
	    250.0,
	    500.0,
	    1000.0,
	    2500.0,
	    4000.0,
	    10000.0,
		20000.0,
	};

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

		if(!bonesBuf)
		{
			bonesBuf = buffer_create(UNIFORMBUFFER);
			buffer_set(bonesBuf, sizeof(MATRIX) * ACKNEXT_MAX_BONES, NULL);
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		if(opengl_wireFrame) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(sky_color.red, sky_color.green, sky_color.blue, sky_color.alpha);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_BLEND);

		// TODO: add current_view for rendering steps
		MATRIX matView, matProj;
		camera_to_matrix(perspective, &matView, &matProj, NULL);

		std::vector<Drawcall> drawcalls;

		for(ENTITY * ent = ent_next(nullptr); ent != nullptr; ent = ent_next(ent))
		{
			// Entity * entity = promote<Entity>(ent);
			if(ent->model == nullptr)
				continue;
			if(!(ent->flags & VISIBLE))
				continue;
			// TODO: Filter entity by mask bits

			MATRIX matWorld;
			glm_to_ack(&matWorld,
				glm::translate(glm::mat4(), ack_to_glm(ent->position)) *
				glm::mat4_cast(ack_to_glm(ent->rotation)) *
				glm::scale(glm::mat4(), ack_to_glm(ent->scale)));

			MATRIX matWorldViewProj;
			glm_to_ack(&matWorldViewProj,
				  ack_to_glm(matProj)
				* ack_to_glm(matView)
				* ack_to_glm(matWorld));

			var dist = vec_dist(&camera->position, &ent->position);
			uint lod;
			for(lod = 15; lod_distances[lod] > dist && lod > 0; lod--);

			if(lod >= 16)
				continue;

			if(lod > ent->model->minimumLOD)
				continue;

			Model * model = promote<Model>(ent->model);
			for(int i = 0; i < model->api().meshCount; i++)
			{
				Drawcall call;

				if(ent->material == nullptr) {
					call.material = model->api().materials[i];
				} else {
					call.material = ent->material;
				}

				call.matWorld = matWorld;
				call.model = demote(model);
				call.mesh = model->api().meshes[i];
				call.ent = ent;
				call.renderDoubleSided = !!(call.mesh->lodMask & DOUBLESIDED);

				// Only allow rendering of meshes when the
				// LOD is enabled in the MESH
				if(call.mesh->lodMask & (1<<lod)) {
					// And only render it, when the
					// mesh is actually visible
					if(!cull(matWorldViewProj, call.mesh->boundingBox)) {
						drawcalls.push_back(call);
					}
				}
			}
		}

		// engine_log("num drawcalls: %d", drawcalls.size());

		// std::sort(drawcalls.begin(), drawcalls.end());


		engine_stats.drawcalls += drawcalls.size();

		{
			MATERIAL const * mtl = nullptr;
			MESH const * mesh = nullptr;
			MODEL const * model = nullptr;
			ENTITY const * ent = nullptr;
			bool doublesided = false;
			for(auto & call : drawcalls)
			{
				if(call.material != mtl) {
					mtl = call.material;
					opengl_setMaterial(mtl);

					currentShader->matView = matView;
					currentShader->matProj = matProj;

					currentShader->vecViewPos = perspective->position;
					static const COLOR fog = {152/255.0,179/255.0,166/255.0,0.0003};
					currentShader->vecFogColor = fog;
					currentShader->fArc = tan(0.5 * DEG_TO_RAD * perspective->arc);

					setupLights();
					setupBones();

					// Bind everything
					shader_setUniforms(&currentShader->api(), call.ent, false);
					shader_setUniforms(&currentShader->api(), call.model, false);
					shader_setUniforms(&currentShader->api(), call.mesh, false);
				}

				if(call.ent != ent) {
					ent = call.ent;

					shader_setUniforms(&currentShader->api(), ent, false);

					MATRIX transforms[ACKNEXT_MAX_BONES];
					transforms[0] = ent->model->bones[0].transform;
					for(int i = 1; i < ent->model->boneCount; i++)
					{
						BONE * bone = &ent->model->bones[i];
						mat_mul(&transforms[i], &bone->transform, &transforms[bone->parent]);
					}

					MATRIX * boneTrafos = (MATRIX*)buffer_map(bonesBuf, READWRITE);
					for(int i = 0; i < ent->model->boneCount; i++)
					{
						BONE * bone = &ent->model->bones[i];
						mat_mul(&boneTrafos[i], &bone->bindToBoneTransform, &transforms[i]);
					}
					buffer_unmap(bonesBuf);
				}

				if(call.model != model) {
					model = call.model;
					shader_setUniforms(&currentShader->api(), model, false);
				}

				if(call.mesh != mesh) {
					mesh = call.mesh;
					shader_setUniforms(&currentShader->api(), mesh, false);
				}

				currentShader->matWorld = call.matWorld;

				if(doublesided != call.renderDoubleSided) {
					doublesided = call.renderDoubleSided;
					if(doublesided)
						glDisable(GL_CULL_FACE);
					else
						glEnable(GL_CULL_FACE);
				}

				call.exec();
			}
		}

		DebugDrawer::render(matView, matProj);
	}
}
