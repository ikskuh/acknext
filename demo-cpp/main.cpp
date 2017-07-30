#include <iostream>

#include <acknext.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
        cout << "Failed to initialize engine: " << engine_lasterror(NULL) << endl;
        return 1;
    }

    while(engine_frame())
    {
        // Now: Render!
    }
    engine_close();
    return 0;
}
