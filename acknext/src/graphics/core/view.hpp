#ifndef VIEW_HPP
#define VIEW_HPP

#include <engine.hpp>
#include <vector>

class View : public EngineObject<VIEW>
{
public:
	static std::vector<View*> all;
public:
	const bool userCreated;
public:
	View(RENDERCALL render, void * arg, bool userCreated);
	NOCOPY(View);
	~View();

	void draw();
};

#endif // VIEW_HPP
