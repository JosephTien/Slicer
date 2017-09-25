#ifndef TOPO_H
#define TOPO_H
#include "pch.h"
#include "geometry.h"
#include "datastructure.h"

class TopoEdge {
public:
	TopoEdge(int a, int b) {
		ia = a;
		ib = b;
	}
	int ia;
	int ib;
};

class Topo
{
public:
	Topo(){}
	std::vector<Vector3> vertices;
	std::vector<TopoEdge> edges;
	std::vector<Vector3> splitNorm;
	std::vector<Plane> knifes;
	std::vector<int> knifeIdx;
	const float radii = 5.0f;
	int verticenum;
	int edgenum;
	void genKnife();
	void read();
	Vector3 getEdgeCent(int idx) { return (vertices[edges[idx].ia] + vertices[edges[idx].ib]) / 2; }
	void calTouch(Piece & piece);
	void purneAva(Group &group, Piece &piece);
};

#endif // TOPO_H