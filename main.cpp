#include "Viewer.h"

using namespace opengl_gui_viewer;

int main(int argc, char const *argv[])
{
    Viewer viewer;
    viewer.Initialize();
    viewer.Main_Loop();
    viewer.Terminate();

    return 0;
}
