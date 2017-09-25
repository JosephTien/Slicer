#include "topo.h"

void Topo::read() {
	std::ifstream ifs;
	//*********************
	ifs.open("thinstruct.txt");
	ifs >> verticenum >> edgenum;
	for (int i = 0; i < verticenum; i++) {
		float x, y, z;
		ifs >> x >> y >> z;
		vertices.push_back(Vector3(x, y, z));
	}
	for (int i = 0; i < edgenum; i++) {
		float a ,b;
		ifs >> a >> b;
		edges.push_back(TopoEdge(a,b));
	}
	ifs.close();
	//*********************
	ifs.open("splitinfo.txt");
	for (int i = 0; i < edgenum; i++) {
		std::string line;
		std::getline(ifs, line);
		std::stringstream ss(line);
		float x, y, z;
		ss >> x >> y >> z;
		splitNorm.push_back(Vector3(x, y, z));
	}
	ifs.close();
	//*********************
}

void Topo::genKnife() {
	float angthre = cos(5.0f * M_PI / 180.0f);
	float disthre = 5.0f;
	//****************************************
	knifes.swap(std::vector<Plane>());
	bool * visited; visited = (bool*)malloc(sizeof(bool)*splitNorm.size()); memset(visited, false, splitNorm.size());
	for (int i = 0; i < splitNorm.size(); i++) {
		if (visited[i])continue;
		for (int j = i + 1; j < splitNorm.size(); j++) {
			Plane planei = Plane(splitNorm[i].normalize(), getEdgeCent(i));//normalized
			Plane planej = Plane(splitNorm[j].normalize(), getEdgeCent(j));//normalized
			TopoEdge edge = edges[j];
			Vector3 v1 = vertices[edge.ia];
			Vector3 v2 = vertices[edge.ib];
			//std::cout << i << " " << j << " " << abs(planei.distanceToPoint(v1)) << " " << abs(planei.distanceToPoint(v2)) << " " << (visited[j] ? "true" : "false") << std::endl;
			if (abs(planei.distanceToPoint(v1)) < disthre
				&& abs(planei.distanceToPoint(v2)) < disthre
				&& abs(planei.normal.dot(planej.normal)) > angthre)
					visited[j] = true;

			//Plane planej = Plane(splitNorm[j].normalize(), getEdgeCent(j));
			//float dot = planei.normal.dot(planej.normal);
			//if ((dot > angthre || dot < -angthre) && abs(planej.distanceToPoint(planei.center)) < disthre)visited[j] = true;
			
		}
	}
	knifeIdx.swap(std::vector<int>());
	for (int i = 0; i < splitNorm.size(); i++) {
		if (!visited[i]) {
			knifes.push_back(Plane(splitNorm[i].normalize(), getEdgeCent(i)));
			knifeIdx.push_back(i);
		}
	}
}

void Topo::calTouch(Piece & piece) {
	float thre = 5.0f;
	float angthre = cos(5.0f * M_PI / 180);
	//******************
	piece.touchinfos.swap(std::vector<TouchInfo>());
	for (int j = 0; j < edgenum; j++)
	{
		TopoEdge edge = edges[j];
		bool disapear = false;
		Vector3 va = vertices[edge.ia];
		Vector3 vb = vertices[edge.ib];
		unsigned int hashiter = piece.hash.hash;
		Vector3 dir = Vector3(0, 0, 0);
		int knifeid = -1;
		std::vector<bool> bits =  piece.hash.getBits();
		for (int i = 0; i < bits.size(); i++)
		{
			bool bit = bits[i];
			Plane knife = knifes[i];
			knife.normal.normalize();
			float da = (va - knife.center).dot(knife.normal);
			float db = (vb - knife.center).dot(knife.normal);
			if (abs(da) < thre && abs(db) < thre && abs(knife.normal.dot(splitNorm[j].normalize())) > angthre) {//thought da&sb is 0, splitnorm may not the knife
				dir = knife.normal * (bit ? 1 : -1);//normalized
				knifeid = i;
			}
			else if (abs(da) < thre && db > 0 && !bit) disapear = true;
			else if (abs(da) < thre && db < 0 && bit) disapear = true; 
			else if (abs(db) < thre && da > 0 && !bit) disapear = true;
			else if (abs(db) < thre && da < 0 && bit) disapear = true;
			else if ((da > 0 && db > 0 && !bit) || (da < 0 && db < 0 && bit)) disapear = true;
			else if (da * db < 0)
			{
				Vector3 c = (vb - va).normalize().dot((knife.center - va)) * (vb - va).normalize() + va;
				if (da < 0 && bit) va = c;
				if (da < 0 && !bit) vb = c;
				if (da > 0 && bit) vb = c;
				if (da > 0 && !bit) va = c;
			}
			if (disapear) break;
		}
		if (!disapear) {
			piece.touchinfos.push_back(TouchInfo(j, va, vb, dir, knifeid));
			//std::cout << "     " << piece.touchinfos[piece.touchinfos.size()-1].e << std::endl;
		}
	}
}

void Topo::purneAva(Group &group, Piece &piece) {
	for (int i = 0; i < piece.touchinfos.size(); i++) {
		int touchedge = piece.touchinfos[i].e;
		TopoEdge edge = edges[touchedge];
		Vector3 cent = (vertices[edge.ia] + vertices[edge.ib]) / 2;
		Vector3 norm = piece.touchinfos[i].dir;
		std::vector<Vector3> ava_new;
		for (int i = 0; i < group.ava.size(); i++) {
			if (group.ava[i].dot(norm) > cos((90 - group.plus)*M_PI / 180))ava_new.push_back(group.ava[i]);
		}
		group.ava.swap(ava_new);
	}	
}

