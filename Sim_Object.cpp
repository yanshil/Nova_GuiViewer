#include "Sim_Object.h"

namespace opengl_gui_viewer
{

Sim_Object::Sim_Object()
  :cube(nullptr), holes(nullptr), trimesh(nullptr)
{}

Sim_Object::~Sim_Object()
{
  if(cube != nullptr)
    delete cube;
  if(holes != nullptr)
    delete holes;
  if(trimesh != nullptr)
    delete trimesh;
}

bool Sim_Object::Check_Out_Of_Boundary(Hole h_, Cuboid c_)
{

    if ((h_.x - h_.radius) > 0 &&
        (h_.x + h_.radius) < c_.depth &&
        (h_.y - h_.radius) > 0 &&
        (h_.y + h_.radius) < c_.width)
        return false;
    else
    {
        std::cout << "Out of boundary!" << std::endl;
        return true;
    }
}
}   // opengl_gui_viewer