#ifndef CGALMACHIHNE_H
#define CGALMACHIHNE_H
#include <pch.h>
#include <pch_cgal.h>
#include <cgaltool.h>
#include "cgalutility.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef CGAL::Polyhedron_3<Kernel>                          Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel>                      Nef_polyhedron;
typedef Kernel::Point_3                                     Point_3;
typedef Kernel::Vector_3                                    Vector_3;
typedef Kernel::Aff_transformation_3                        Aff_transformation_3;
typedef Polyhedron::Halfedge_handle                         Halfedge_handle;
typedef Polyhedron::HalfedgeDS                              HalfedgeDS;
typedef CGAL::Delaunay_triangulation_3<Kernel>              Delaunay;
typedef Delaunay::Vertex_handle                             Vertex_handle;
typedef Polyhedron::Vertex_iterator                         Vertex_iterator;
typedef Polyhedron::Facet_iterator                          Facet_iterator;
//typedef Polyhedron::Halfedge_around_facet_circulator      Halfedge_around_facet_circulator;
typedef Polyhedron::Halfedge_around_facet_circulator        Halfedge_facet_circulator;

typedef Nef_polyhedron::Vertex_const_handle               Vertex_const_handle;
typedef Nef_polyhedron::Halfedge_const_handle             Halfedge_const_handle;
typedef Nef_polyhedron::Halffacet_const_handle            Halffacet_const_handle;
typedef Nef_polyhedron::Volume_const_handle               Volume_const_handle;
typedef Nef_polyhedron::Object_handle                     Object_handle;

/*********************data_struct*********************/
class PolyStruct {
public:
    Polyhedron polyhedron;
    Nef_polyhedron nef_polyhedron;
    std::string name;
    std::string filename;
    int flag;//bit : [nef_polyhedron][polyhedron]
    int structstate;//bit : [normal][center]

    void setCenter(Point_3 p);
    Point_3 getCenter();
    void makeNef();
    int makeDataExist();

private:
    Point_3 center;
};

class CGALMachine
{
public:
    CGALMachine();
    void machine(std::string line);
    void machine(std::string line, std::vector<float> coords, std::vector<unsigned int> tris);
    void machine(std::string line, std::vector<float> coords, std::vector<int> tris);
    void Reset(){polyDataTail=0;cnt=0;}
    bool checkInside(std::string out, std::string in);
private:
    static const int polyDataSize=10000;
    PolyStruct polyData[polyDataSize];
    int polyDataTail=0;
    void calCenter(PolyStruct &pd);
    Polyhedron genePoly(std::vector<float> coords, std::vector<int> tris);
    Polyhedron convexHull(Polyhedron P);
    Polyhedron vectorScale(Polyhedron P, double thick);
    int searchTar(std::string str);
    int checkDataExist(int tar);
    int checkDataExist(int tar, int flag);
    int cnt=0;
    CGALTool cgaltool;
};

#endif // CGALMACHING_H
