#include <acknext.h>

void main()
{
    engine_log("Hello from script!");
	while(true)
	{
		// Do some fancy screen coloring.
		for(screen_color.blue = 0; screen_color.blue < 255; screen_color.blue ++)
		{
			wait();
		}
	}
}
