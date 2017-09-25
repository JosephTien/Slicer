#include <cgaltool.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
//typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Surface_mesh;

namespace SMS = CGAL::Surface_mesh_simplification ;
Surface_mesh CGALTool::simplify(Surface_mesh surface_mesh){

    SMS::Count_stop_predicate<Surface_mesh> stop(1000);

  // This the actual call to the simplification algorithm.
  // The surface mesh and stop conditions are mandatory arguments.
  // The index maps are needed because the vertices and edges
  // of this surface mesh lack an "id()" field.
    int r = SMS::edge_collapse(
						surface_mesh
						,stop
						//,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
						,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
					    .halfedge_index_map  (get(CGAL::halfedge_external_index,surface_mesh))
				        .get_cost (SMS::Edge_length_cost <Surface_mesh>())
                        .get_placement(SMS::Midpoint_placement<Surface_mesh>())
                    );
    return surface_mesh;
}
int CGALTool::simplify(std::string input, std::string output)
{
  Surface_mesh surface_mesh;

  std::ifstream is(input.c_str()) ; is >> surface_mesh ;
  // This is a stop predicate (defines when the algorithm terminates).
  // In this example, the simplification stops when the number of undirected edges
  // left in the surface mesh drops below the specified number (1000)
  SMS::Count_stop_predicate<Surface_mesh> stop(1000);

  // This the actual call to the simplification algorithm.
  // The surface mesh and stop conditions are mandatory arguments.
  // The index maps are needed because the vertices and edges
  // of this surface mesh lack an "id()" field.
  int r = SMS::edge_collapse
            (surface_mesh
            ,stop
             //,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
            ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                .halfedge_index_map  (get(CGAL::halfedge_external_index  ,surface_mesh))
                .get_cost (SMS::Edge_length_cost <Surface_mesh>())
                .get_placement(SMS::Midpoint_placement<Surface_mesh>())
            );

  std::cout << "		Finished...\n		" << r << " edges removed.\n		"
            << (surface_mesh.size_of_halfedges()/2) << " final edges.\n" ;

  std::ofstream os( output.c_str() ) ; os << surface_mesh ;

  return 0 ;
}
