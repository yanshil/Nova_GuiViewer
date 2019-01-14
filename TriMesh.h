#ifndef TRIMESH_H_
#define TRIMESH_H_

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
/**
 * 
 */
using namespace Nova;
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

    double x;
    double y;
    double radius;
    std::vector<Vertex> vertices;
    void initVertex();

    int id;

    static int sNextID;

  // TODO: 
    int segments = 64;

  private:
    int getNextId();
    
};

// TODO
class HoleList
{
  public:
    HoleList();
    ~HoleList();

    std::vector<Hole> holes;

    int size(); // Get the size of holes
    int AddHole(const Hole &hole);  // return ID
    int ModifyHolebyID(int holeID);  // return modify status
    int DeleteHolebyID(int holeID); // return delete status
    void RemoveAllHole();
    
  private:

    bool isEntryValid(const  Hole &temp);
    bool Check_Overlap(const  Hole &temp);
    double Distance(double x1_, double y1_, double x2_, double y2_);
};

class Cuboid
{
  public:
    Cuboid();
    // A cube with bottom left at (0,0,0) with depth, width, height
    Cuboid(double depth, double width, double height);
    ~Cuboid();
    // TODO: Not sure what this for...
    int n_segments = 4;
    double depth;
    double width;
    double height;
    double edge_max;
    void Update();
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

#endif  //TRIMESH_H_