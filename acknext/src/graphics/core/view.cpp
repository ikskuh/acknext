#include "view.hpp"
#include <algorithm>

std::vector<View*> View::all;

View::View(RENDERCALL render, void * arg, bool userCreated) :
	EngineObject<VIEW>(),
    userCreated(userCreated)
{
	api().flags = VISIBLE | FULLSCREEN;
	api().renderer = render;
	api().context = arg;
	api().layer = 0.0;
	all.emplace_back(this);
}

View::~View()
{
	std::remove(all.begin(), all.end(), this);
}

void View::draw()
{
	VIEW & view = api();
	if(view.renderer == nullptr) {
		return;
	}
	if(!(view.flags & VISIBLE)) {
		return;
	}
	assert(view_current != nullptr);
	POINT pos;
	SIZE size;
	view_to_bounds(&this->api(), &pos, &size);

	if(size.width <= 0 || size.height <= 0) {
		return;
	}

	int gly = screen_size.height - pos.y - size.height;

	glViewport(pos.x, gly, size.width, size.height);
	glScissor(pos.x, gly, size.width, size.height);

	view.renderer(view.context);
}

ACKNEXT_API_BLOCK
{
	VIEW * view_create(RENDERCALL render, void * context)
	{
		return demote(new View(render, context, true));
	}

	void view_draw(VIEW * _view)
	{
		View * view = promote<View>(_view);
		if(view) {
			view->draw();
		}
	}

	void view_remove(VIEW * _view)
	{
		View * view = promote<View>(_view);
		if(view && view->userCreated) {
			delete view;
		}
	}

	void view_to_bounds(VIEW const * view, POINT * pt, SIZE * size)
	{
		if(view == nullptr) {
			if(pt) *pt = (POINT){ 0, 0 };
			if(size) *size = screen_size;
		} else {
			if(pt) *pt = view->position;
			if(size) *size = view->size;
			if(view->flags & FULLSCREEN) {
				if(pt) *pt = (POINT){ 0, 0 };
				if(size) *size = screen_size;
			}
		}
	}
}
