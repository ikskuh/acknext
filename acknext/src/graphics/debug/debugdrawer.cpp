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
	vertex.weights = (VECTOR4){1,0,0,0};

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
	vertex.weights = (VECTOR4){1,0,0,0};

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
}
