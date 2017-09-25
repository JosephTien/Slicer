#ifndef CGALNEF_H
#define CGALNEF_H

#include "pch.h"
#include "pch_cgal.h"

#define SC 10000

//typedef CGAL::Extended_homogeneous<CGAL::Exact_integer>  Kernel;//gmpz
typedef CGAL::Extended_cartesian<CGAL::Exact_rational> Kernel;//gmpfr
typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Nef_polyhedron::Plane_3  Nef_Plane_3;
typedef Kernel::Plane_3  Plane_3;
typedef Kernel::Aff_transformation_3  Aff_transformation_3;

class CGALNef
{
public:
    CGALNef();
	Nef_polyhedron Cube;
	std::vector<Nef_polyhedron> Ns;
	std::vector<Nef_polyhedron> Ns_;
	void genCube(Vector3 ld, Vector3 ru);
	void genPiece(std::vector<Vector3> cs, std::vector<Vector3> ns);
	Nef_polyhedron Slice(Nef_polyhedron &N, Vector3 p, Vector3 n);
	std::string getOffStr(Nef_polyhedron N);
	Nef_Plane_3 getPlane(float x, float y, float z, float d);
};

#endif // CGALNEF_H