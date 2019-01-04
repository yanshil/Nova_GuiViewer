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
    ~Hole();
    void Set(double x_, double y_, double radius_);
    double x = 0.5;
    double y = 0.5;
    double radius = 0.1;
    std::vector<Vertex> vertices;

  //private:
    int segments = 64;
};

class Cuboid
{
  public:
    Cuboid();
    ~Cuboid();
    int n_segments = 4;
    double depth = 1.0;
    double width = 1.0;
    double height = 1.0;
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

class TriMesh
{
  public:
    TriMesh();
    ~TriMesh();
    std::vector<int> triangle_list;
    std::vector<double> vertex_list;
    std::vector<std::vector<Vertex> > GenMesh(Cuboid cuboid_, std::vector<Hole> hs_);
};
