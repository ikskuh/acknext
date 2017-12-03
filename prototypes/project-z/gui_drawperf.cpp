#include <acknext.h>
#include <acknext/ext/ackgui.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl3w.h>

#include <stdlib.h>

#include <assert.h>

#include <list>
#include <vector>
#include <algorithm>
#include <numeric>

template<typename T>
struct perfcounter
{
	mutable std::list<T> samples;
	mutable uint maxlen;

	perfcounter() : samples(), maxlen(100)
	{

	}

	T average() const
	{
		T total = std::accumulate(
			this->samples.begin(),
			this->samples.end(),
			T());
		return total / T(this->samples.size());
	}

	void limit(uint count) const
	{
		this->maxlen = std::max(this->maxlen, count);
		while(this->samples.size() > this->maxlen)
			this->samples.pop_front();
	}

	void insert(const T & sample)
	{
		this->samples.push_back(sample);
	}
};

struct perfview
{
	ImDrawList * graphics;
	ImVec2 pos;
	int w, h;

	perfview(int h = 100) :
	    graphics(ImGui::GetWindowDrawList()),
	    pos(ImGui::GetCursorScreenPos()),
		w(ImGui::GetWindowContentRegionWidth()), h(h)
	{
		const ImVec4 bg = ImVec4(0.2f,0.2f,0.2f,1.0f);
		graphics->AddRectFilled(
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x+w, pos.y + h),
			ImColor(bg));
		ImGui::Dummy(ImVec2(w, h));
	}

	template<typename T>
	void show(perfcounter<T> const & counter, ImVec4 const & color)
	{
		T avg = counter.average();
		if(avg == 0)
			avg = 1;
		this->show(counter, color, 2 * avg);
	}

	template<typename T>
	void show(perfcounter<T> const & counter, ImVec4 const & color, T const & max)
	{
		counter.limit(uint(w));

		std::vector<ImVec2> polyline(counter.samples.size());

		std::transform(
			counter.samples.begin(), counter.samples.end(),
			polyline.begin(),
			[=](T const & val) { return ImVec2(0, float(val) / float(max)); });

		for(uint i = 0; i < polyline.size(); i++)
		{
			polyline[i].x = pos.x + i;
			polyline[i].y = pos.y + h - h * polyline[i].y;
		}

		graphics->AddPolyline(
			polyline.data(),
			polyline.size(),
			ImColor(color),
			false,
			1.0,
			true);
	}
};

static perfcounter<float> perfTimeTotal, perfTimeGpu;
static perfcounter<ulong> perfPolycount, perfDrawcalls;

static float time_step_over_time = 0;
static float gpu_time_over_time = 0;

extern "C" void gui_drawperf()
{
	time_step_over_time = 0.9 * time_step_over_time + 0.1 * time_step;
	gpu_time_over_time = 0.9 * gpu_time_over_time + 0.1 * engine_stats.gpuTime;

	int num = 0;
	ENTITY * you;
	for(you = ent_next(NULL); you; you = ent_next(you), num++);

	perfTimeTotal.insert(time_step);
	perfTimeGpu.insert(0.001 * engine_stats.gpuTime);
	perfPolycount.insert(engine_stats.polygons);
	perfDrawcalls.insert(engine_stats.drawcalls);

	if(!ImGui::Begin("Stats"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text(
		"Camera Pos: (%.2f, %.2f, %.2f)",
		camera->position.x,
		camera->position.y,
		camera->position.z);
	ImGui::Text(
		"Framerate:  %d FPS",
		int(1.0 / time_step_over_time));
	ImGui::Text(
		"Frame Time: %4.2f ms",
		1000.0 * time_step_over_time);
	ImGui::Text(
		"GPU Time:   %4.2f ms",
		gpu_time_over_time);
	ImGui::Text(
		"Entities:   %d",
		num);
	ImGui::Text(
		"Drawcalls:  %d",
		engine_stats.drawcalls);
	ImGui::Text(
		"Polygons:   %lld",
		engine_stats.polygons);

	// Primitives

	ImGui::Separator();
	ImGui::Text("Performance Graph");
	{
		perfview view(100);

		float max = 2.0 * perfTimeTotal.average();

		view.show(perfTimeTotal, ImVec4(1,1,1,1), max);
		view.show(perfTimeGpu, ImVec4(1,0,0,1), max);
	}

	ImGui::Separator();
	ImGui::Text("Polycount (Red) / Drawcall (Green) Graph");
	{
		perfview view(100);
		view.show(perfPolycount, ImVec4(1,0,0,1));
		view.show(perfDrawcalls, ImVec4(0,1,0,1));
	}

	ImGui::End();
}
