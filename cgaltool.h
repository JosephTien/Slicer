#ifndef CGALTOOL_H
#define CGALTOOL_H
#include "pch.h"
#include "pch_cgal.h"
#include "cgalutility.h"
//#include <iostream>
//#include <sstream>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
typedef CGAL::Polyhedron_3<CGAL::Exact_predicates_exact_constructions_kernel> Surface_Mesh;

class CGALTool{
public:
      CGALTool(){}
      void fillHole(const char* filename);
      void fillHole(std::vector<float> &coords, std::vector<unsigned int> &tris);
      std::string fillHoleAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris);
      Surface_Mesh simplify(Surface_Mesh surface_mesh);
      int simplify(std::string input, std::string output);
      void readFromOFFStream(std::vector<float> &coords, std::vector<unsigned int> &tris, std::string &str);
	  std::string convexHullAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris);
	  std::string simplifyAndGetStr(std::vector<float> &coords, std::vector<unsigned int> &tris);
};
#endif // CGALTOOL_H
