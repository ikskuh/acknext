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
#include <map>
#include <unordered_map>

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
static BUFFER * instaBuf = nullptr;

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
};

enum class HalfSpace
{
	Negative = -1,
	OnPlane = 0,
	Positive = 1,
};

struct Plane
{
	Plane() : xyz(0,0,0), w(0) { }

	Plane(float x, float y, float z, float w) :
	    xyz(x, y, z),
	    w(w)
	{
		this->normalize();
	}

	glm::vec3 xyz;
	float w;

	void normalize()
	{
		float mag = glm::length(xyz);
		this->xyz /= mag;
		this->w   /= mag;
	}

	float distance(glm::vec3 const & pt) const
	{
		return this->xyz.x * pt.x + this->xyz.y * pt.y + this->xyz.z * pt.z + this->w;
	}

	float distance(VECTOR const & pt) const
	{
		return this->xyz.x * pt.x + this->xyz.y * pt.y + this->xyz.z * pt.z + this->w;
	}

	HalfSpace classify(glm::vec3 const & pt) const
	{
		float d = this->distance(pt);
		if (d < 0)
			return HalfSpace::Negative;
		if (d > 0)
			return HalfSpace::Positive;
		return HalfSpace::OnPlane;
	}
};

struct Frustrum
{
	Plane planes[6];

	Frustrum(MATRIX const & modelView)
	{
		float m11 = modelView.fields[0][0];
		float m12 = modelView.fields[1][0];
		float m13 = modelView.fields[2][0];
		float m14 = modelView.fields[3][0];

		float m21 = modelView.fields[0][1];
		float m22 = modelView.fields[1][1];
		float m23 = modelView.fields[2][1];
		float m24 = modelView.fields[3][1];

		float m31 = modelView.fields[0][2];
		float m32 = modelView.fields[1][2];
		float m33 = modelView.fields[2][2];
		float m34 = modelView.fields[3][2];

		float m41 = modelView.fields[0][3];
		float m42 = modelView.fields[1][3];
		float m43 = modelView.fields[2][3];
		float m44 = modelView.fields[3][3];

		/*left*/   planes[0] = Plane(m41 + m11, m42 + m12, m43 + m13, m44 + m14);
		/*right*/  planes[1] = Plane(m41 - m11, m42 - m12, m43 - m13, m44 - m14);
		/*bottom*/ planes[2] = Plane(m41 + m21, m42 + m22, m43 + m23, m44 + m24);
		/*top*/    planes[3] = Plane(m41 - m21, m42 - m22, m43 - m23, m44 - m24);
		/*near*/   planes[4] = Plane(m41 + m31, m42 + m32, m43 + m33, m44 + m34);
		/*far*/    planes[5] = Plane(m41 - m31, m42 - m32, m43 - m33, m44 - m34);

		for(int i = 0; i < 6; i++)
			planes[i].normalize();
	}
};

static bool cull(Frustrum const & frustrum, VECTOR const & position, float radius)
{
	using namespace glm;

	var threshold = -radius;

	for(int i = 0; i < 6; i++)
	{
		float dist = frustrum.planes[i].distance(position);
		if(dist < threshold)
			return true;
	}
	return false;
}

struct Drawgroup
{
	MATERIAL const * mtl = nullptr;
	MESH const * mesh = nullptr;
	MODEL const * model = nullptr;
	bool doublesided = false;
};

static bool operator ==(Drawgroup const & lhs, Drawgroup const & rhs)
{
	return lhs.mtl == rhs.mtl
		&& lhs.model == rhs.model
		&& lhs.mesh == rhs.mesh
		&& lhs.doublesided == rhs.doublesided;
}

class DrawgroupHash
{
public:
    size_t operator()(const Drawgroup &group) const
    {
        size_t h1 = reinterpret_cast<size_t>(group.mtl);
		size_t h2 = reinterpret_cast<size_t>(group.model);
        size_t h3 = reinterpret_cast<size_t>(group.mesh);
		size_t h4 = std::hash<bool>()(group.doublesided);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

struct Instance
{
	// NOTE: MUST BE FIRST MEMBER OF
	// STRUCT, OTHERWISE THE VERTEX ARRAY LAYOUT
	// WILL MESS UP!
	MATRIX transform;
	ENTITY const * ent = nullptr;
} __attribute__((packed));

extern GLuint vao;

static void render_scene(CAMERA * perspective, MATERIAL * mtlOverride);

static SHADER * create_ppshader(char const * pixelop)
{
	SHADER * ppshader = shader_create();

	shader_addFileSource(ppshader, VERTEXSHADER, "/builtin/shaders/postprocess.vert");
	shader_addFileSource(ppshader, FRAGMENTSHADER, "/builtin/shaders/gamma.glsl");
	shader_addFileSource(ppshader, FRAGMENTSHADER, pixelop);

	shader_link(ppshader);

	return ppshader;
}

ACKNEXT_API_BLOCK
{
	CAMERA * camera;

	COLOR sky_color = { 0.3, 0.7, 1.0, 1.0 };

	var pp_exposure = 1.0;
	PPSTAGES pp_stages = PP_BLOOM | PP_SSAO | PP_REINHARD;

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
		GLint drawFboId = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);

		if(drawFboId != 0)
			engine_log("Current FB: %d", drawFboId);


		static FRAMEBUFFER * stageScene = nullptr;
		static FRAMEBUFFER * stageSSAOApply = nullptr;
		static FRAMEBUFFER * stageSSAOBlur = nullptr;
		static FRAMEBUFFER * stageSSAOCombine = nullptr;
		static FRAMEBUFFER * stageBloom0 = nullptr;
		static FRAMEBUFFER * stageBloom1 = nullptr;
		static FRAMEBUFFER * stageBloomCombine = nullptr;
		static FRAMEBUFFER * stageHDR = nullptr;

		SIZE targetSize;
		view_to_bounds(view_current, nullptr, &targetSize);

		SIZE halfSize = targetSize;
		halfSize.width /= 2;
		halfSize.height /= 2;

		MATRIX matView, matProj;
		camera_to_matrix(perspective, &matView, &matProj, view_current);

		if(stageScene == nullptr)
		{
			stageScene = framebuf_create();
			stageScene->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGBA32F); // color
			stageScene->targets[1] = bmap_create(GL_TEXTURE_2D, GL_RGBA32F); // position
			stageScene->targets[2] = bmap_create(GL_TEXTURE_2D, GL_RGBA8);   // normal
			stageScene->targets[3] = bmap_create(GL_TEXTURE_2D, GL_RGBA8);   // attributes (roughness, metallic, ???)
			stageScene->depthBuffer = bmap_create(GL_TEXTURE_2D, GL_DEPTH24_STENCIL8);
		}

		if(stageSSAOApply == nullptr)
		{
			stageSSAOApply = framebuf_create();
			stageSSAOApply->targets[0] = bmap_create(GL_TEXTURE_2D, GL_R8);
		}

		if(stageSSAOBlur == nullptr)
		{
			stageSSAOBlur = framebuf_create();
			stageSSAOBlur->targets[0] = bmap_create(GL_TEXTURE_2D, GL_R8);
		}

		if(stageSSAOCombine == nullptr)
		{
			stageSSAOCombine = framebuf_create();
			stageSSAOCombine->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGB16F);
		}

		if(stageBloom0 == nullptr)
		{
			stageBloom0 = framebuf_create();
			stageBloom0->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGB16F);
		}

		if(stageBloom1 == nullptr)
		{
			stageBloom1 = framebuf_create();
			stageBloom1->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGB16F);
		}

		if(stageBloomCombine == nullptr)
		{
			stageBloomCombine = framebuf_create();
			stageBloomCombine->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGB16F);
			framebuf_resize(stageBloomCombine, targetSize);
		}

		if(stageHDR == nullptr)
		{
			stageHDR = framebuf_create();
			stageHDR->targets[0] = bmap_create(GL_TEXTURE_2D, GL_RGB16F);
			framebuf_resize(stageHDR, targetSize);
		}

		static SHADER * tonemapLinear = nullptr;
		static SHADER * tonemapReinhard = nullptr;
		static SHADER * bloomblur = nullptr;
		static SHADER * bloomcomb = nullptr;
		static SHADER * ssao = nullptr;
		static SHADER * ssaoCombine = nullptr;
		static SHADER * fxaa = nullptr;

		if(!tonemapLinear)   tonemapLinear   = create_ppshader("/builtin/shaders/pp/hdr/linear.frag");
		if(!tonemapReinhard) tonemapReinhard = create_ppshader("/builtin/shaders/pp/hdr/reinhard.frag");
		if(!bloomblur)       bloomblur       = create_ppshader("/builtin/shaders/pp/bloom/blur.frag");
		if(!bloomcomb)       bloomcomb       = create_ppshader("/builtin/shaders/pp/bloom/combine.frag");
		if(!ssao)            ssao            = create_ppshader("/builtin/shaders/pp/ssao/apply.frag");
		if(!ssaoCombine)     ssaoCombine     = create_ppshader("/builtin/shaders/pp/ssao/combine.frag");
		if(!fxaa)            fxaa            = create_ppshader("/builtin/shaders/pp/fxaa.frag");

		{ // 1: render scnee
			framebuf_resize(stageScene, targetSize);
			opengl_setFrameBuffer(stageScene);

			render_scene(perspective, nullptr);
		}

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		BITMAP * currentOutput = stageScene->targets[0];

		if(pp_stages & PP_SSAO)
		{
			{
				framebuf_resize(stageSSAOApply, halfSize);
				opengl_setFrameBuffer(stageSSAOApply);

				opengl_setShader(ssao);

				currentShader->matView = matView;
				currentShader->matProj = matProj;
				currentShader->texInput = bmap_to_linear(stageScene->targets[0]);
				currentShader->texPosition = bmap_to_linear(stageScene->targets[1]);
				currentShader->texNormal = bmap_to_linear(stageScene->targets[2]);

				opengl_drawFullscreenQuad();
			}

			{
				framebuf_resize(stageSSAOBlur, halfSize);
				opengl_setFrameBuffer(stageSSAOBlur);

				opengl_setShader(bloomblur);
				currentShader->texInput = bmap_to_linear(stageSSAOApply->targets[0]);
				currentShader->fCutoff = 0.0;
				currentShader->vecBlurScale = (VECTOR2) { 0.5, 0.5 };

				opengl_drawFullscreenQuad();
			}

			{
				framebuf_resize(stageSSAOCombine, targetSize);
				opengl_setFrameBuffer(stageSSAOCombine);

				opengl_setShader(ssaoCombine);
				currentShader->texInput     = bmap_to_linear(stageScene->targets[0]);
				currentShader->texOcclusion = bmap_to_linear(stageSSAOBlur->targets[0]);

				opengl_drawFullscreenQuad();
			}

			currentOutput = stageSSAOCombine->targets[0];
		}

		if(pp_stages & PP_BLOOM)
		{
			{ // 2: render bloom image (half size)
				framebuf_resize(stageBloom0, halfSize);
				opengl_setFrameBuffer(stageBloom0);

				opengl_setShader(bloomblur);
				currentShader->texInput = bmap_to_linear(currentOutput);
				currentShader->fCutoff = 1.0;
				currentShader->vecBlurScale = (VECTOR2) { 1.0, 0.5 };

				opengl_drawFullscreenQuad();
			}

			{ // 3: render bloom image (quart size)
				SIZE halfSize = targetSize;
				halfSize.width /= 4;
				halfSize.height /= 4;
				framebuf_resize(stageBloom1, halfSize);
				opengl_setFrameBuffer(stageBloom1);

				opengl_setShader(bloomblur);
				currentShader->texInput = bmap_to_linear(stageBloom0->targets[0]);
				currentShader->fCutoff = 0.0;
				currentShader->vecBlurScale = (VECTOR2) { 1.0, 0.5 };

				opengl_drawFullscreenQuad();
			}

			{ // 3: combine bloom image with source image
				framebuf_resize(stageBloomCombine, targetSize);
				opengl_setFrameBuffer(stageBloomCombine);

				opengl_setShader(bloomcomb);
				currentShader->texInput = currentOutput;
				currentShader->texBloom = bmap_to_linear(stageBloom1->targets[0]);

				opengl_drawFullscreenQuad();
			}
			currentOutput = stageBloomCombine->targets[0];
		}

		{ // 4:
			framebuf_resize(stageHDR, targetSize);
			opengl_setFrameBuffer(stageHDR);

			if(pp_stages & PP_REINHARD) {
				opengl_setShader(tonemapReinhard);
			} else {
				opengl_setShader(tonemapLinear);
			}

			currentShader->texInput = currentOutput;
			currentShader->fExposure = pp_exposure;

			opengl_drawFullscreenQuad();
		}

		{
			opengl_setFrameBuffer(nullptr);
			if(drawFboId != 0)
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId);

			opengl_setShader(fxaa);

			currentShader->texInput = stageHDR->targets[0];

			opengl_drawFullscreenQuad();
		}
	}
}

static void render_scene(CAMERA * perspective, MATERIAL * mtlOverride)
{
	glBindVertexArray(vao);

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

	if(!instaBuf)
	{
		instaBuf = buffer_create(VERTEXBUFFER);
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

	MATRIX matView, matProj;
	camera_to_matrix(perspective, &matView, &matProj, view_current);

	std::vector<Drawcall> drawcalls;


	MATRIX matViewProj;
	glm_to_ack(&matViewProj,
		  ack_to_glm(matProj)
		* ack_to_glm(matView));

	Frustrum clipFrustrum(matViewProj);

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

		var dist = vec_dist(&camera->position, &ent->position);
		uint lod;
		for(lod = 15; lod_distances[lod] > dist && lod > 0; lod--);

		if(lod > ent->model->minimumLOD)
			continue;

		Model * model = promote<Model>(ent->model);
		for(int i = 0; i < model->api().meshCount; i++)
		{
			Drawcall call;

			if(mtlOverride != nullptr)
			{
				call.material = mtlOverride;
			}
			else
			{
				if(ent->material == nullptr)
					call.material = model->api().materials[i];
				else
					call.material = ent->material;
			}

			call.matWorld = matWorld;
			call.model = demote(model);
			call.mesh = model->api().meshes[i];
			call.ent = ent;
			call.renderDoubleSided = !!(call.mesh->lodMask & DOUBLESIDED);

			float radius = 0;
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.minimum.x));
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.minimum.y));
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.minimum.z));
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.maximum.x));
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.maximum.y));
			radius = glm::max(radius, glm::abs(call.mesh->boundingBox.maximum.z));

			// Only allow rendering of meshes when the
			// LOD is enabled in the MESH
			if(call.mesh->lodMask & (1<<lod))
			{
				// And only render it, when the
				// mesh is actually visible
				if(!cull(clipFrustrum, call.ent->position, radius))
				{
					drawcalls.push_back(call);
				}
			}
		}
	}

	{
		std::unordered_map<Drawgroup, std::vector<Instance>, DrawgroupHash> groups;
		for(auto & call : drawcalls)
		{
			Drawgroup group;
			group.mtl = call.material;
			group.mesh = call.mesh;
			group.model = call.model;
			group.doublesided = call.renderDoubleSided;

			Instance instance;
			instance.ent = call.ent;
			instance.transform = call.matWorld;

			groups[group].push_back(instance);
		}

//			engine_log("start rendering");
		for(auto & entry : groups)
		{
			Drawgroup const & params = entry.first;
			std::vector<Instance> const & instances = entry.second;

			// Setup:
			{
				opengl_setMaterial(params.mtl);

				currentShader->matView = matView;
				currentShader->matProj = matProj;

				currentShader->vecViewPos = perspective->position;
				static const COLOR fog = {152/255.0,179/255.0,166/255.0,0.0003};
				currentShader->vecFogColor = fog;
				currentShader->fArc = tan(0.5 * DEG_TO_RAD * perspective->arc);

				setupLights();
				setupBones();
			}

			shader_setUniforms(&currentShader->api(), params.model, false);
			shader_setUniforms(&currentShader->api(), params.mesh, false);

			// Animated models have a slight problem:
			// They can't be instanced!
			bool useInstancing =
				   (currentShader->api().flags & USE_INSTANCING)
				&& ((params.mesh->lodMask & ANIMATED) == 0);

//				engine_log("Render %5d of (mtl=%p model=%p mesh=%p dsr=%d)%s",
//					int(instances.size()),
//					params.mtl,
//					params.model,
//					params.mesh,
//					params.doublesided,
//					(useInstancing) ? " instanced" : "");

			if(params.doublesided)
				glDisable(GL_CULL_FACE);
			else
				glEnable(GL_CULL_FACE);

			if(useInstancing == false)
			{
				for(Instance const & inst : instances)
				{
					MATRIX animatedBones[ACKNEXT_MAX_BONES];
					for(int i = 0; i < ACKNEXT_MAX_BONES; i++)
					{
						MATRIX & transform = animatedBones[i];
						mat_id(&transform);
						mat_translate(&transform, &inst.ent->pose[i].position);
						mat_rotate(&transform, &inst.ent->pose[i].rotation);
						mat_scale(&transform, &inst.ent->pose[i].scale);
					}

					MATRIX transforms[ACKNEXT_MAX_BONES];
					transforms[0] = animatedBones[0];
					for(int i = 1; i < inst.ent->model->boneCount; i++)
					{
						BONE * bone = &inst.ent->model->bones[i];
						mat_mul(&transforms[i], &animatedBones[i], &transforms[bone->parent]);
					}

					MATRIX * boneTrafos = (MATRIX*)buffer_map(bonesBuf, READWRITE);
					for(int i = 0; i < inst.ent->model->boneCount; i++)
					{
						BONE * bone = &inst.ent->model->bones[i];
						mat_mul(&boneTrafos[i], &bone->bindToBoneTransform, &transforms[i]);
					}
					buffer_unmap(bonesBuf);

					currentShader->useInstancing = false;
					currentShader->useBones = true;
					currentShader->matWorld = inst.transform;
					opengl_drawMesh(params.mesh);
				}
			}
			else
			{
				MATRIX transforms[ACKNEXT_MAX_BONES];
				transforms[0] = params.model->bones[0].transform;
				for(int i = 1; i < params.model->boneCount; i++)
				{
					BONE const * bone = &params.model->bones[i];
					mat_mul(&transforms[i], &bone->transform, &transforms[bone->parent]);
				}

				MATRIX * boneTrafos = (MATRIX*)buffer_map(bonesBuf, READWRITE);
				for(int i = 0; i < params.model->boneCount; i++)
				{
					BONE const * bone = &params.model->bones[i];
					mat_mul(&boneTrafos[i], &bone->bindToBoneTransform, &transforms[i]);
				}
				buffer_unmap(bonesBuf);

				currentShader->useInstancing = true;
				currentShader->useBones = false;

				// TODO: Implement instance buffer cycling
				{
					size_t size = instances.size() * sizeof(Instance);
					if(size <= instaBuf->size) {
						buffer_update(
							instaBuf,
							0,
							size,
							instances.data());
					} else {
						buffer_set(
							instaBuf,
							size,
							instances.data());
					}
				}

				glVertexArrayVertexBuffer(
					vao,
					12,
					instaBuf->object,
					0,
					sizeof(Instance));
				glVertexArrayBindingDivisor(
					vao,
					12,
					1);

				currentShader->matWorld = MATRIX { 0 };

				int count;
				GLenum type = opengl_setMesh(params.mesh, &count);

				opengl_draw(
					type,
					0,
					count,
					instances.size());
			}
		}
	}

	DebugDrawer::render(matView, matProj);
}
