// Copyright 2019 Yanshi
//#####################################################################
// Class Sim_Object.h
//######################################################################
#ifndef SIM_OBJECT_H_
#define SIM_OBJECT_H_

#include "TriMesh.h"

namespace opengl_gui_viewer
{

class Sim_Object
{
public:
  Sim_Object();
  ~Sim_Object();

  Cuboid *cube;
  HoleList *holes;
  TriMesh *trimesh;

  // Display Options
  bool option_wireframe;
  // int option_material;
  bool option_path;
  bool animation_test = false;

  // Auxiliary Functions
  bool Check_Out_Of_Boundary(Hole &h_, Cuboid &c_);
  bool Check_Validation(Cuboid &tmpc_);

private:
};

} // namespace opengl_gui_viewer

#endif // SIM_OBJECT_H_
