#ifndef PCH_CGAL_H
#define PCH_CGAL_H
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/IO/Nef_polyhedron_iostream_3.h>
#include <CGAL/IO/print_wavefront.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/convex_hull_3_to_face_graph.h>
#include <CGAL/OFF_to_nef_3.h>
#include <CGAL/algorithm.h>

// for cgal nef
#include <CGAL/Exact_integer.h>
#include <CGAL/Extended_homogeneous.h>

// for cgal utility
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Modifier_base.h> //not in origin code?

// for remesher
#include <CGAL/Simple_cartesian.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>

#endif // PCH_CGAL_H
