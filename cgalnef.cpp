#include "cgalnef.h"

CGALNef::CGALNef()
{

}

void CGALNef::genPiece(std::vector<Vector3> cs, std::vector<Vector3> ns) {
	Ns.swap(std::vector<Nef_polyhedron>());
	Ns.push_back(Cube);
	int kn = cs.size();
	for (int i = 0; i < kn; i++) {
		std::cout << "applying knife " << i << std::endl;
		Ns_.swap(std::vector<Nef_polyhedron>());
		for (int j = 0; j < Ns.size(); j++) {
			Nef_polyhedron N = Ns[j];
			Nef_polyhedron N_ = Slice(N, cs[i], ns[i]);
			if (N != Nef_polyhedron::EMPTY) Ns_.push_back(N); else printf("empty\n");
			if (N_ != Nef_polyhedron::EMPTY) Ns_.push_back(N_); else printf("empty\n");
		}
		Ns.swap(Ns_);
	}
}

Nef_Plane_3 CGALNef::getPlane(float x, float y, float z, float d) {
	//return Plane_3((int)(x*SC), (int)(y*SC), (int)(z*SC), (int)(d*SC));
	return Nef_Plane_3(x, y, z, d);
}

void CGALNef::genCube(Vector3 ld, Vector3 ru) {
	Nef_polyhedron N1(getPlane(1, 0, 0, -ru.x));
	Nef_polyhedron N2(getPlane(-1, 0, 0, ld.x));
	Nef_polyhedron N3(getPlane(0, 1, 0, -ru.y));
	Nef_polyhedron N4(getPlane(0, -1, 0, ld.y));
	Nef_polyhedron N5(getPlane(0, 0, 1, -ru.z));
	Nef_polyhedron N6(getPlane(0, 0, -1, ld.z));
	Cube = N1 * N2 * N3 * N4 * N5 * N6;
}

Nef_polyhedron CGALNef::Slice(Nef_polyhedron &N, Vector3 p, Vector3 n) {
	n.normalize();
	Vector3 n_ = -n;
	Nef_polyhedron Plane(getPlane(-n.x, -n.y, -n.z, p.dot(n)));
	Nef_polyhedron Plane_(getPlane(-n_.x, -n_.y, -n_.z, p.dot(n_)));
	Nef_polyhedron N_ = N - Plane;
	N = N  - Plane_;
	return N_;
}

std::string CGALNef::getOffStr(Nef_polyhedron N) {
	std::stringstream ss;
	ss.precision(17);
	Polyhedron P;
	N.convert_to_polyhedron(P);
	ss << P << std::endl;
	return ss.str();
}