#pragma once
#include <vector>
#define REAL double
//#define ANSI_DECLARATORS
#include <iostream>
#include "triangle.h"

#undef REAL
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <nova/Geometry/Topology_Based_Geometry/Triangulated_Surface.h>

using namespace Nova;
/**
 * 
 */
class Vertex
{
  public:
    Vertex();
    ~Vertex();
    Vertex(double x_, double y_);
    Vertex(double x_,double y_,double z_);
    int     id;
    double  x;
    double  y;
    double  z = 0.0;
    int boundary_marker = 1;
};


class Hole
{
  public:
    Hole();
    Hole(double x, double y, double radius);
    ~Hole();
    // TODO: Change to Modify()?
    void Set(double x_, double y_, double radius_);
    double x;
    double y;
    double radius;
    std::vector<Vertex> vertices;
    void initVertex();

  //private:
    int segments = 64;
};

// TODO
class HoleList
{
  public:
    HoleList();
    ~HoleList();

    std::vector<Hole> holes;

    int AddHole(Hole &hole);  // return ID
    int ModifyHolebyID(int holeID);  // return modify status
    int DeleteHolebyID(int holeID); // return delete status

};

class Cuboid
{
  public:
    Cuboid();
    // A cube with bottom left at (0,0,0) with depth, width, height
    Cuboid(double depth, double width, double height, int n_segments);
    ~Cuboid();
    int n_segments;
    double depth;
    double width;
    double height;
    double edge_max;
};

class Segment
{
  public:
    Segment();
    ~Segment();
    int id;
    int v1;
    int v2;
};

/**
 * Generate Triangular Mesh
 */
class TriMesh
{
  public:
    TriMesh();
    ~TriMesh();
    Array<Vector<double,3> >  vertex_list;
    Array<Vector<int,3> >     triangle_list;
    Triangulated_Surface<double> triangulated_surface;
    void GenMesh(Cuboid cuboid_, std::vector<Hole> hs_);
};
