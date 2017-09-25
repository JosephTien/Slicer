#define CGAL_EIGEN3_ENABLED
#include "CGALTool.h"

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Polyhedron_3.h>
//#include <CGAL/IO/Polyhedron_iostream.h>
//#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>

#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <boost/foreach.hpp>
#include <CGAL/Polyhedron_items_with_id_3.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>     Polyhedron;
typedef Polyhedron::Halfedge_handle    Halfedge_handle;
typedef Polyhedron::Facet_handle       Facet_handle;
typedef Polyhedron::Vertex_handle      Vertex_handle;
typedef Polyhedron::HalfedgeDS         HalfedgeDS;

void CGALTool::fillHole(const char* filename){
    Polyhedron poly;
    std::ifstream input(filename);
    if ( !input || !(input >> poly) || poly.empty() ) {
      std::cerr << "Not a valid off file." << std::endl;
      return;
    }
    // Incrementally fill the holes
    unsigned int nb_holes = 0;
    BOOST_FOREACH(Halfedge_handle h, halfedges(poly))
    {
        if(h->is_border())
        {
            std::vector<Facet_handle>  patch_facets;
            std::vector<Vertex_handle> patch_vertices;

            CGAL::Polygon_mesh_processing::triangulate_and_refine_hole(
                poly,
                h,
                std::back_inserter(patch_facets),
                std::back_inserter(patch_vertices),
                CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, poly)).
                geom_traits(Kernel()));
            ++nb_holes;
        }

    }
    std::ofstream out(filename);
    out.precision(17);
    out << poly << std::endl;
}
void CGALTool::readFromOFFStream(std::vector<float> &coords, std::vector<unsigned int> &tris, std::string &str){
    std::stringstream ss(str);
    coords.clear();
    tris.clear();
    std::string c;
    ss >> c;
    int vn,tn,cn;
    ss >> vn >> tn >> cn;
    for(int i=0;i<vn;i++){
        float x,y,z;
        ss >> x >> y >> z;
        coords.push_back(x);coords.push_back(y);coords.push_back(z);
    }
    for(int i=0;i<tn;i++){
        int n,x,y,z;
        ss >> n >> x >> y >> z;
        tris.push_back(x);tris.push_back(y);tris.push_back(z);
        //std::cout << tris[i*3] << " " << tris[i*3+1] << " " << tris[i*3+2] <<std::endl;
        //std::cout << n << " " << x << " " << y << " " << z << std::endl;
    }
}

std::string CGALTool::fillHoleAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris){
    Polyhedron poly;
    Polyhedron_builder<HalfedgeDS> builder( coords, tris );
    poly.delegate(builder);
    // Incrementally fill the holes
    unsigned int nb_holes = 0;
    BOOST_FOREACH(Halfedge_handle h, halfedges(poly))
    {
        if(h->is_border())
        {
            
            std::vector<Facet_handle>  patch_facets;
            std::vector<Vertex_handle> patch_vertices;
            //CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(
            CGAL::Polygon_mesh_processing::triangulate_and_refine_hole(
                poly,
                h,
                std::back_inserter(patch_facets),
                std::back_inserter(patch_vertices),
                CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, poly)).geom_traits(Kernel())
                );
            
			/*
			std::vector<Facet_handle>  patch_facets;
            CGAL::Polygon_mesh_processing::triangulate_hole(
                poly,
                h,
                std::back_inserter(patch_facets),
                CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, poly)).geom_traits(Kernel())
                );
			*/
        }

    }
    std::stringstream ss;
    ss.precision(17);
    ss << poly << std::endl;
    return ss.str();
}

void CGALTool::fillHole(std::vector<float> &coords, std::vector<unsigned int> &tris){
    Polyhedron poly;
    Polyhedron_builder<HalfedgeDS> builder( coords, tris );
    poly.delegate(builder);
    // Incrementally fill the holes
    unsigned int nb_holes = 0;
    BOOST_FOREACH(Halfedge_handle h, halfedges(poly))
    {
        if(h->is_border())
        {
            std::vector<Facet_handle>  patch_facets;
            std::vector<Vertex_handle> patch_vertices;

            //CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(
            CGAL::Polygon_mesh_processing::triangulate_and_refine_hole(
                poly,
                h,
                std::back_inserter(patch_facets),
                std::back_inserter(patch_vertices),
                CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, poly)).
                geom_traits(Kernel()));
        }

    }

    /*
    coords.clear();tris.clear();
    int i=0;
    for(Polyhedron::Vertex_iterator vi = poly.vertices_begin(); vi!=poly.vertices_end();vi++){
        coords.push_back(vi->point().x());
        coords.push_back(vi->point().y());
        coords.push_back(vi->point().z());
        vi->id() = i++;
    }
    for(Polyhedron::Face_iterator fi = poly.facets_begin(); fi!=poly.facets_end();fi++){

    }
    for(Polyhedron::Face_iterator fi = poly.facets_begin(); fi!=poly.facets_end();fi++){
        Polyhedron::Halfedge_around_facet_circulator circ = fi->facet_begin();
        do {
            tris.push_back(circ->vertex()->id());
            //printf("%d ",circ->vertex()->id());
        } while ( ++circ != fi->facet_begin());
        //std::cout << std::endl;
    }
    */

//    std::ofstream ofs;
//    ofs.open("temp.obj");
//    CGAL::print_polyhedron_wavefront(ofs, poly);

    std::ofstream out("temp.off");
    out.precision(17);
    out << poly << std::endl;

    std::stringstream ss;
    ss.precision(17);
    ss << poly << std::endl;
    std::string str = ss.str();
    readFromOFFStream(coords, tris, str);

}

