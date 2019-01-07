#include"TriMesh.h"
Vertex::Vertex()
{

}

Vertex::~Vertex()
{
    
}

Vertex::Vertex(double x_, double y_)
{
        x = x_;
        y = y_;
}
Vertex::Vertex(double x_,double y_,double z_)
{
        x = x_;
        y = y_;
        z = z_;
}

int Hole::sNextID = 0;

Hole::Hole()
{
    // TODO:
    this->x = 0.5;
    this->y = 0.5;
    this->radius = 0.1;
    this->id = getNextId();

    initVertex();
}

void Hole::initVertex()
{
    this->vertices.clear();
    
    Vertex vertex;
    double theta;
    // TODO: for all 3.14159, Define Math.PI for performance?
    double dtheta = 2.0 * 3.1415926 / segments;
    for (int i = 0; i < segments; i++)
    {
        theta = dtheta * i;
        vertex.x = x + radius * cos(theta);
        vertex.y = y + radius * sin(theta);
        this->vertices.push_back(vertex);
    }
}

// TODO: Segments?
Hole::Hole(double x, double y, double radius)
    :x(x), y(y), radius(radius)
{
    initVertex();
    this->id = getNextId();
    std::cout << "id = " << id << std::endl;
}

Hole::~Hole()
{

}
int Hole::getNextId()
{
    return ++this->sNextID;
}

void Hole::Set(double x_, double y_, double radius_)
{

    this->x = x_;
    this->y = y_;
    this->radius = radius_;

    initVertex();
}

HoleList::HoleList()
{
}

HoleList::~HoleList() {}

void HoleList::RemoveAllHole()
{
    holes.clear();
}

int HoleList::AddHole(Hole &temp) 
{
    int id = temp.id;

    if(!isEntryValid(temp))
        return 0;   // Unvalid

    holes.push_back(temp);
    return id;
}

int HoleList::ModifyHolebyID(int holeID)
{
    // TODO
    return 0;
}

int HoleList::DeleteHolebyID(int holeID)
{
    // TODO
    return 0;
}

int HoleList::size()
{
    return holes.size();
}

bool HoleList::isEntryValid(Hole &temp)
{
    bool correct_entry;
    correct_entry=!Check_Overlap(temp);
    return correct_entry;
}

bool HoleList::Check_Overlap(Hole &temp)
{
    for (int i = 0; i < holes.size(); i++)
    {
        std::cout << "We are here" << std::endl;
        std::cout << "r1:" << holes[i].radius << ", r2:" << temp.radius << ", d:" << Distance(holes[i].x, holes[i].y, temp.x, temp.y) << std::endl;
        if (Distance(holes[i].x, holes[i].y, temp.x, temp.y) <= (holes[i].radius + temp.radius))
        {
            std::cout << "r1:" << holes[i].radius << ", r2:" << temp.radius << ", d:" << Distance(holes[i].x, holes[i].y, temp.x, temp.y) << std::endl;
            std::cout << "Overlapped with #" << i + 1 << "!" << std::endl;
            return true;
        }
    }
    return false;
}

double HoleList::Distance(double x1_, double y1_, double x2_, double y2_)
{

    return sqrt((x1_ - x2_) * (x1_ - x2_) + (y1_ - y2_) * (y1_ - y2_));
}


Cuboid::Cuboid()
{
    // TODO:Default constructor with d, w, h = 1,1,1
    this->n_segments = 4;
    this->depth = 1.0;
    this->width = 1.0;
    this->height = 1.0;

    // this->edge_max = depth>(width>height?width:height)?depth:(width>height?width:height);
    this->edge_max = std::max(depth, std::max(width, height));
}

Cuboid::Cuboid(double depth, double width, double height, int n_segments)
    :depth(depth), width(width), height(height), n_segments(n_segments)
{
    // this->edge_max = depth>(width>height?width:height)?depth:(width>height?width:height);
    this->edge_max = std::max(depth, std::max(width, height));
}

Cuboid::~Cuboid()
{

}
    

Segment::Segment()
{

}
Segment::~Segment()
{

}
    
TriMesh::TriMesh()
{

}

TriMesh::~TriMesh()
{

}

std::vector<std::vector<Vertex> > TriMesh::GenMesh(Cuboid cuboid_, std::vector<Hole> hs_)
{
    FILE *stream;
    char filename[32] = "output.obj";
    stream = fopen(filename, "w+");

    struct triangulateio in, mid, out, vorout;
    std::vector<Vertex> hole_vertices_;
    std::vector<Vertex> rect_vertices_;
    hole_vertices_.clear();
    rect_vertices_.clear();

    rect_vertices_.push_back(Vertex(0,0));
    rect_vertices_.push_back(Vertex(cuboid_.depth,0));
    rect_vertices_.push_back(Vertex(cuboid_.depth,cuboid_.width));
    rect_vertices_.push_back(Vertex(0,cuboid_.width));
    



        




    for (int i = 0; i < hs_.size(); i++)
    {
        for (int j = 0; j < hs_[i].vertices.size(); j++)
        {
                hole_vertices_.push_back(hs_[i].vertices[j]);
        }
    }
    int numberofvertices = rect_vertices_.size() + hole_vertices_.size();
    in.numberofpoints = numberofvertices;
    in.numberofpointattributes = 1;
    in.pointattributelist=(double *)malloc(in.numberofpoints*sizeof(double));
    in.pointlist = (double *)malloc(in.numberofpoints * 2 * sizeof(double));
    in.pointmarkerlist = (int *)NULL;
    // add boundary vertices
    for (int i = 0; i < rect_vertices_.size(); i++)
    {
        in.pointlist[2 * i] = rect_vertices_[i].x;
        in.pointlist[2 * i + 1] = rect_vertices_[i].y;
    }

    // add hole vertices
    for (int i = 0; i < hole_vertices_.size(); i++)
    {
        in.pointlist[2*rect_vertices_.size()+2 * i] = hole_vertices_[i].x;
        in.pointlist[2*rect_vertices_.size()+2 * i + 1] = hole_vertices_[i].y;

    }

        


    in.numberofsegments = in.numberofpoints;
    in.segmentlist = (int *)malloc(in.numberofsegments * 2 * sizeof(int));
    in.segmentmarkerlist = (int *)NULL;

    // set boundary segments
    for (int i = 0; i < rect_vertices_.size() - 1; i++)
    {
        in.segmentlist[2 * i] = i;
        in.segmentlist[2 * i + 1] = i + 1;
    }

    in.segmentlist[2 * rect_vertices_.size() - 2] = rect_vertices_.size() - 1;
    in.segmentlist[2 * rect_vertices_.size() - 1] = 0;

    // set hole segments
    for(int i=0;i<hs_.size();i++)
    {
        for (int j = 0; j<hs_[i].vertices.size()-1; j++)
        {
            in.segmentlist[2 * rect_vertices_.size()+2*i*hs_[i].vertices.size()+2*j] = rect_vertices_.size()+i*hs_[i].vertices.size()+j;
            in.segmentlist[2 * rect_vertices_.size()+2*i*hs_[i].vertices.size()+2*j + 1] = rect_vertices_.size()+i*hs_[i].vertices.size()+j+1;
        }
        in.segmentlist[2 * rect_vertices_.size()+2*i*hs_[i].vertices.size()+2*(hs_[i].vertices.size()-1)]=rect_vertices_.size()+i*hs_[i].vertices.size()+(hs_[i].vertices.size()-1);
        in.segmentlist[2 * rect_vertices_.size()+2*i*hs_[i].vertices.size()+2*(hs_[i].vertices.size()-1)+1]=rect_vertices_.size()+i*hs_[i].vertices.size();
    }

    in.numberofholes = hs_.size();
    in.holelist = (double *)malloc(in.numberofholes * 2 * sizeof(double));

    for (int i = 0; i < in.numberofholes; i++)
    {
        in.holelist[2 * i] = hs_[i].x;
        in.holelist[2 * i + 1] = hs_[i].y;
    }

    in.numberofregions = 0;
    in.regionlist = (double *)NULL;




    mid.pointlist = (double *)NULL; /* Not needed if -N switch used. */
                                        /* Not needed if -N switch used or number of point attributes is zero: */
    mid.pointattributelist = (double *)NULL;
    mid.pointmarkerlist = (int *)NULL; /* Not needed if -N or -B switch used. */
    mid.trianglelist = (int *)NULL;    /* Not needed if -E switch used. */
                                           /* Not needed if -E switch used or number of triangle attributes is zero: */
    mid.triangleattributelist = (double *)NULL;
    mid.neighborlist = (int *)NULL; /* Needed only if -n switch used. */
                                        /* Needed only if segments anumTrianglesre output (-p or -c) and -P not used: */
    mid.segmentlist = (int *)NULL;   //numTriangles
        /* Needed only if segments are output (-p or -c) and -P and numTriangles-B not used: */
    mid.segmentmarkerlist = (int *)NULL;//numTriangles
    mid.edgelist = (int *)NULL;       /* Needed only if -e switch used. */
    mid.edgemarkerlist = (int *)NULL; /* Needed if -e used and -B not used. */

    vorout.pointlist = (double *)NULL; /* Needed only if -v switch used. */
                                           /* Needed only if -v switch used and number of attributes is not zero: */
    vorout.pointattributelist = (double *)NULL;
    vorout.edgelist = (int *)NULL;    /* Needed only if -v switch used. */
    vorout.normlist = (double *)NULL; /* Needed only if -v switch used. */

    triangulate("pzevngqY", &in, &mid, &vorout);

        
    // for(int i = 0; i < mid.numberofpoints; i++)
    // {
    //     std::cout<<mid.pointmarkerlist[i]<<std::endl;
    // }
        
    
    for(int i = 0; i < mid.numberofpoints; i++)
    {
        vertex_list.push_back(mid.pointlist[2*i]);
        vertex_list.push_back(mid.pointlist[2*i+1]);
        vertex_list.push_back(0.0);
    }

    for(int i = 0; i < mid.numberofpoints; i++)
    {
        vertex_list.push_back(mid.pointlist[2*i]);
        vertex_list.push_back(mid.pointlist[2*i+1]);
        vertex_list.push_back(cuboid_.height);
    }

    // bottom
    for(int i = 0; i < mid.numberoftriangles; i++)
    {
        triangle_list.push_back(mid.trianglelist[3*i]);
        triangle_list.push_back(mid.trianglelist[3*i+1]);
        triangle_list.push_back(mid.trianglelist[3*i+2]);
    }
    
    // top
    for(int i = 0; i < mid.numberoftriangles; i++)
    {
        triangle_list.push_back(mid.trianglelist[3*i]+mid.numberofpoints);
        triangle_list.push_back(mid.trianglelist[3*i+1]+mid.numberofpoints);
        triangle_list.push_back(mid.trianglelist[3*i+2]+mid.numberofpoints);
    }



    
    for(int i = 0; i < 3; i++)
    {
        triangle_list.push_back(i);
        triangle_list.push_back(i+mid.numberofpoints);
        triangle_list.push_back(i+1);

        triangle_list.push_back(i+mid.numberofpoints);
        triangle_list.push_back(i+mid.numberofpoints+1);
        triangle_list.push_back(i+1);
    }
    triangle_list.push_back(3);
    triangle_list.push_back(3+mid.numberofpoints);
    triangle_list.push_back(0);
 
    triangle_list.push_back(3+mid.numberofpoints);
    triangle_list.push_back(0+mid.numberofpoints);
    triangle_list.push_back(0);
    
    
    for(int i = 0; i < hs_.size(); i++)
    {
        // start:4+hs_[i].segments*i
        // end:   
        
        for(int j = 0; j < hs_[i].segments-1; j++)
        {
        triangle_list.push_back(4+hs_[i].segments*i+j);
        triangle_list.push_back(4+hs_[i].segments*i+j+mid.numberofpoints);
        triangle_list.push_back(4+hs_[i].segments*i+j+1);

        triangle_list.push_back(4+hs_[i].segments*i+j+mid.numberofpoints);
        triangle_list.push_back(4+hs_[i].segments*i+j+mid.numberofpoints+1);
        triangle_list.push_back(4+hs_[i].segments*i+j+1);
        }
        
        triangle_list.push_back(4+hs_[i].segments*i+hs_[i].segments-1);
        triangle_list.push_back(4+hs_[i].segments*i+hs_[i].segments-1+mid.numberofpoints);
        triangle_list.push_back(4+hs_[i].segments*i);

        triangle_list.push_back(4+hs_[i].segments*i+hs_[i].segments-1+mid.numberofpoints);
        triangle_list.push_back(4+hs_[i].segments*i+mid.numberofpoints);
        triangle_list.push_back(4+hs_[i].segments*i);
    }
    



    for(int i=0;i<vertex_list.size()/3;i++)
    fprintf(stream,"v %f %f %f\n",vertex_list[3*i],vertex_list[3*i+1],vertex_list[3*i+2]);
    for(int i=0;i<triangle_list.size()/3;i++)
    fprintf(stream,"f %d %d %d\n",triangle_list[3*i]+1,triangle_list[3*i+1]+1,triangle_list[3*i+2]+1);

	fclose(stream);

    delete in.pointlist;
    delete in.pointmarkerlist;
    delete in.segmentlist;
    delete in.segmentmarkerlist;

    delete mid.pointlist;
    delete mid.segmentlist;
    delete mid.trianglelist;
    delete mid.pointmarkerlist;
    delete mid.segmentmarkerlist;
    delete mid.edgelist;
    delete mid.edgemarkerlist;
    delete mid.triangleattributelist;
    delete mid.holelist;
        //delete mid.pointattributelist;

    delete vorout.pointlist;
    delete vorout.pointmarkerlist;
    delete vorout.edgelist;
    delete vorout.pointattributelist;
    delete vorout.edgemarkerlist;
    return (std::vector<std::vector<Vertex> >)NULL;
}

