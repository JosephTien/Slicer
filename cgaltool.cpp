#include "cgaltool.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef CGAL::Polyhedron_3<Kernel>                          Polyhedron;
typedef Kernel::Point_3                                     Point_3;
typedef CGAL::Delaunay_triangulation_3<Kernel>              Delaunay;
typedef Delaunay::Vertex_handle                             Vertex_handle;
typedef Polyhedron::Vertex_iterator                         Vertex_iterator;
typedef Polyhedron::HalfedgeDS								HalfedgeDS;

std::string CGALTool::convexHullAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris) {
	Polyhedron_builder<HalfedgeDS> builder(coords, tris);
	Polyhedron P;
	P.delegate(builder);
	//*********************************
	std::list<Point_3> points;
	for (Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v)
		points.push_back(v->point());
	Delaunay T;
	T.insert(points.begin(), points.end());
	std::list<Vertex_handle> vertices;
	T.incident_vertices(T.infinite_vertex(), std::back_inserter(vertices));
	Polyhedron chull;
	CGAL::convex_hull_3_to_face_graph(T, chull);
	//*********************************
	std::stringstream ss;
	ss.precision(17);
	ss << chull << std::endl;
	return ss.str();
}

std::string CGALTool::simplifyAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris) {
	Polyhedron_builder<HalfedgeDS> builder(coords, tris);
	Polyhedron P;
	P.delegate(builder);
	//*********************************
	std::stringstream ss;
	ss.precision(17);
	ss << simplify(P) << std::endl;
	return ss.str();
}