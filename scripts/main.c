#include <acknext.h>

action rotator()
{
	while(true)
	{
		my->rotation = euler(45.0 * total_secs, 0, 0);
		wait();
	}
}

void main()
{
    ent_create("earth.mdl", vector(30, 0, 0), &rotator);
}
