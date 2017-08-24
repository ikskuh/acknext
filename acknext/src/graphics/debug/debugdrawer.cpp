#include "debugdrawer.hpp"

std::vector<VERTEX> DebugDrawer::lines;
std::vector<VERTEX> DebugDrawer::points;

BUFFER * DebugDrawer::vertexBuffer;
MATERIAL * DebugDrawer::material;

void DebugDrawer::initialize()
{
	vertexBuffer = buffer_create(VERTEXBUFFER);
	material = mtl_create();
}

void DebugDrawer::render(MATRIX const & matView, MATRIX const & matProj)
{
	opengl_setVertexBuffer(vertexBuffer);
	opengl_setIndexBuffer(nullptr);
	opengl_setMaterial(material);

	opengl_setTransform(
		mat_id(nullptr),
		&matView,
		&matProj);

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	if(points.size() > 0)
	{
		buffer_set(
			vertexBuffer,
			sizeof(VERTEX) * points.size(),
			points.data());

		glPointSize(5.0f);
		glDrawArrays(
			GL_POINTS,
			0,
			points.size());
	}

	if(lines.size() > 0)
	{
		buffer_set(
			vertexBuffer,
			sizeof(VERTEX) * lines.size(),
			lines.data());

		glDrawArrays(
			GL_LINES,
			0,
			lines.size());
	}
}

void DebugDrawer::reset()
{
	lines.clear();
	points.clear();
}

void DebugDrawer::shutdown()
{
	mtl_remove(material);
	buffer_remove(vertexBuffer);
}

void DebugDrawer::drawLine(VECTOR const & from, VECTOR const & to, COLOR const & color)
{
	VERTEX vertex;
	vertex.color = color;
	vertex.normal = (VECTOR){0,0,0};
	vertex.tangent = (VECTOR){0,0,0};
	vertex.texcoord0 = (UVCOORD){0.5,0.5};
	vertex.texcoord1 = (UVCOORD){0.5,0.5};
	vertex.bones = (UBYTE4){0,0,0,0};
	vertex.boneWeights = (UBYTE4){255,0,0,0};

	vertex.position = from;
	lines.emplace_back(vertex);

	vertex.position = to;
	lines.emplace_back(vertex);
}

void DebugDrawer::drawPoint(VECTOR const & pt, COLOR const & color)
{
	VERTEX vertex;
	vertex.color = color;
	vertex.normal = (VECTOR){0,0,0};
	vertex.tangent = (VECTOR){0,0,0};
	vertex.texcoord0 = (UVCOORD){0.5,0.5};
	vertex.texcoord1 = (UVCOORD){0.5,0.5};
	vertex.bones = (UBYTE4){0,0,0,0};
	vertex.boneWeights = (UBYTE4){255,0,0,0};

	vertex.position = pt;
	points.emplace_back(vertex);

}

ACKNEXT_API_BLOCK
{
	void draw_line3d(VECTOR const * p1, VECTOR const * p2, COLOR const * color)
	{
		DebugDrawer::drawLine(
			*p1,
			*p2,
			*color);
	}

	void draw_point3d(VECTOR const * pos, COLOR const * color)
	{
		DebugDrawer::drawPoint(
			*pos,
			*color);
	}

	void draw_sphere3d(VECTOR const * center, var radius, COLOR const * color)
	{
		VECTOR rotx = { radius, 0, 0 };
		VECTOR roty = { 0, radius, 0 };
		VECTOR rotz = { 0, radius, 0 };
		VECTOR prex = rotx;
		VECTOR prey = roty;
		VECTOR prez = rotz;
		for(var i = 0; i < 360; i += 10)
		{
			vec_rotate(&rotx, euler(10, 0, 0));
			draw_line3d(
				vec_add(vec_clone(center), &prex),
				vec_add(vec_clone(center), &rotx),
				color);
			prex = rotx;

			vec_rotate(&roty, euler(0, 10, 0));
			draw_line3d(
				vec_add(vec_clone(center), &prey),
				vec_add(vec_clone(center), &roty),
				color);
			prey = roty;

			vec_rotate(&rotz, euler(0, 0, 10));
			draw_line3d(
				vec_add(vec_clone(center), &prez),
				vec_add(vec_clone(center), &rotz),
				color);
			prez = rotz;
		}
	}

	void draw_aabb3d(VECTOR const * min, VECTOR const * max, COLOR const * color)
	{
		draw_line3d(
			vector(min->x, min->y, min->z),
			vector(max->x, min->y, min->z),
			color);
		draw_line3d(
			vector(min->x, min->y, min->z),
			vector(min->x, max->y, min->z),
			color);
		draw_line3d(
			vector(min->x, min->y, min->z),
			vector(min->x, min->y, max->z),
			color);

		draw_line3d(
			vector(max->x, min->y, min->z),
			vector(max->x, max->y, min->z),
			color);
		draw_line3d(
			vector(max->x, min->y, min->z),
			vector(max->x, min->y, max->z),
			color);

		draw_line3d(
			vector(min->x, min->y, max->z),
			vector(min->x, max->y, max->z),
			color);
		draw_line3d(
			vector(min->x, min->y, max->z),
			vector(max->x, min->y, max->z),
			color);
		draw_line3d(
			vector(max->x, min->y, max->z),
			vector(max->x, max->y, max->z),
			color);


		draw_line3d(
			vector(min->x, max->y, min->z),
			vector(min->x, max->y, max->z),
			color);
		draw_line3d(
			vector(max->x, max->y, min->z),
			vector(max->x, max->y, max->z),
			color);
		draw_line3d(
			vector(min->x, max->y, min->z),
			vector(max->x, max->y, min->z),
			color);
		draw_line3d(
			vector(min->x, max->y, max->z),
			vector(max->x, max->y, max->z),
			color);
	}



	ACKFUN void draw_box3d(
		VECTOR const * _center,
		VECTOR const * _extends,
		QUATERNION const * rotation,
		COLOR const * color)
	{
		VECTOR center = *_center;
		QUATERNION rot = *rotation;
		VECTOR extends = *_extends;
		vec_scale(&extends, 0.5);


		int deltas[] =
		{
			0,1,0,2,2,3,1,3,4,6,4,5,5,7,6,7,0,4,1,5,3,7,2,6
		};

		for(int i = 0; i < 12; i++)
		{
			int ifrom = deltas[2*i+0];
			int   ito = deltas[2*i+1];

			VECTOR from;
			from.x = (2 * !!(ifrom & 1) - 1) * extends.x;
			from.y = (2 * !!(ifrom & 2) - 1) * extends.y;
			from.z = (2 * !!(ifrom & 4) - 1) * extends.z;

			VECTOR to;
			to.x = (2 * !!(ito & 1) - 1) * extends.x;
			to.y = (2 * !!(ito & 2) - 1) * extends.y;
			to.z = (2 * !!(ito & 4) - 1) * extends.z;

			vec_rotate(&from, &rot);
			vec_rotate(&to, &rot);
			vec_add(&from, &center);
			vec_add(&to, &center);


			draw_line3d(
				&from,
				&to,
				color);
		}
	}
}
