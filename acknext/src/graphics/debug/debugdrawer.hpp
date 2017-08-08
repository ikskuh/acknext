#ifndef DEBUGDRAWER_HPP
#define DEBUGDRAWER_HPP

#include <engine.hpp>
#include <vector>

class DebugDrawer
{
private:
	static std::vector<VERTEX> points;
	static std::vector<VERTEX> lines;

	static MATERIAL * material;
	static BUFFER * vertexBuffer;

public:
	DebugDrawer() = delete;

	static void initialize();

	static void reset();

	static void render(MATRIX const & matView, MATRIX const & matProj);

	static void shutdown();

public:
	static void drawLine(VECTOR const & from, VECTOR const & to, COLOR const & color);

	static void drawPoint(VECTOR const & pt, COLOR const & color);
};

#endif // DEBUGDRAWER_HPP
