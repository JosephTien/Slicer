#include "utility.h"

float gr() {
	return (((float)rand() / RAND_MAX) - 0.5f) * 2;
}

void Utility::genPieceGroupMesh(std::string filename){
	bool realMerge = false;
    std::ifstream ifs;
    ifs.open(filename);
    std::string line;
    Vector3 ld, ru; float x,y,z; int n,l,m;
    char str[20];
    std::stringstream ss;
    std::getline( ifs, line);ss = std::stringstream(line);
    ss >> x >> y >> z; ld = Vector3(x,y,z);
    ss >> x >> y >> z; ru = Vector3(x,y,z);
    std::cout << "left low : " << ld << std::endl;
    std::cout << "right up : " << ru << std::endl;
    //********************************************************
    std::vector<Vector3> cs,ns;
    std::getline( ifs, line);ss = std::stringstream(line);
    ss >> n >> l;
    std::cout << "knife number : " << n << std::endl;
    std::cout << "shape number : " << l << std::endl;
    for(int j=0;j<n;j++){
        std::getline( ifs, line);ss = std::stringstream(line);
        ss >> x >> y >> z;cs.push_back(Vector3(x,y,z));
        ss >> x >> y >> z;ns.push_back(Vector3(x,y,z));
        std::cout << "knife_" << j << " : " << cs[j] << " " << ns[j] <<std::endl;
    }
    //********************************************************
    std::cout << "________________________________________" << std::endl;
    iglMachine.reset();
    iglMachine.command("NEW null");
    for(int j=0;j<n;j++){
        //Mesh mesh = Mesh().genCube(cs[j]-Vector3(100000,100000,0), cs[j]+Vector3(100000,100000,100000));
        Mesh mesh = Mesh().genTri(cs[j], 100000);
        mesh.rotateTo(ns[j]); //notice the reverse case
        sprintf(str,"knife_%d", j);
        iglMachine.put(str, mesh.vertices, mesh.indices);
    }
    //********************************************************
    int cnt=0;
    for(int j=0;j<l;j++){
        std::cout << "________________________________________" << std::endl;
        std::getline( ifs, line);ss = std::stringstream(line);
        ss >> m;
        int tar = cnt++;
        std::cout << "Generate shape_" << tar << " with " << m << " pieces\n>" << std::endl;
        sprintf(str,"NEW shape_%d", tar);iglMachine.command(str);
        for(int k=0;k<m;k++){
            std::getline( ifs, line);
            Mesh mesh = Mesh().genCube(ld, ru);
            iglMachine.put("temp", mesh.vertices, mesh.indices);
            const char * chs = line.c_str();
            int len = strlen(chs);
            for(int i=0;i<len;i++){
                sprintf(str,"%c temp temp knife_%d", chs[i], i);iglMachine.command(str);
            }
            sprintf(str,"ADD shape_%d temp", tar);iglMachine.command(str);
        }
		if (realMerge) { sprintf(str, "+ shape_%d shape_%d null", tar, tar); iglMachine.command(str); }
        sprintf(str,"WRITE shape_%d", tar);iglMachine.command(str);
    }
    std::cout << "________________________________________\nDone!" << std::endl;
	//********************************************************
}

void Utility::genRandomTest(int k) {
	FILE * fp = fopen("randomTest.txt", "w");
	fprintf(fp, "-1 -1 -1 1 1 1\n");
	fprintf(fp, "%d 0\n", k);
	for (int i = 0; i < k; i++) {
		fprintf(fp, "%f %f %f %f %f %f\n", gr(), gr(), gr(), gr(), gr(), gr());
	}
	fclose(fp);
}

void Utility::voxelBfs() {
	std::vector<int> queue;
	int flag = 0;
	for (int i = 0; i < voxels.size(); i++) {
		if (voxels[i].touchid.size() > 0 && !voxels[i].exist) {
			queue.push_back(i);
			voxels[i].exist = true;
			break;
		}
	}
	int p = 0;
	Piece piece;
	std::set<int> touchs;
	while (flag < queue.size()) {
		int qn = queue.size();
		for (; flag < qn; flag++) {
			int cur = queue[flag];
			piece.voxelsi.push_back(cur);
			voxels[cur].belong = p;
			for (auto j : voxels[cur].touchid) {
				touchs.insert(j);
			}
			int ix, iy, iz;
			voxels[cur].getXYZ(ix, iy, iz);
			int nei[6] = { -1, -1, -1, -1, -1, -1 };
			if (ix - 1 >= 0)nei[0] = ((ix - 1)* ny * nz + (iy)* nz + (iz));
			if (ix + 1 < nx)nei[1] = ((ix + 1) * ny * nz + (iy)* nz + (iz));
			if (iy - 1 >= 0)nei[2] = ((ix)* ny * nz + (iy - 1)* nz + (iz));
			if (iy + 1 < ny)nei[3] = ((ix)* ny * nz + (iy + 1)* nz + (iz));
			if (iz - 1 >= 0)nei[4] = ((ix)* ny * nz + (iy)* nz + (iz - 1));
			if (iz + 1 < nz)nei[5] = ((ix)* ny * nz + (iy)* nz + (iz + 1));
			for (int i = 0; i < 6; i++) {
				if (nei[i] == -1)continue;
				int tar = nei[i];
				if(voxels[tar].exist)continue;
				bool walk = true;
				for (auto j : touchs) {
					//if (voxels[cur].hash.getBit(j * 3 + 1) && voxels[cur].hash.getBit(j * 3 + 2) != voxels[tar].hash.getBit(j * 3 + 2))walk = false;
					//if (voxels[cur].hash.getBit(j * 3 + 2) != voxels[tar].hash.getBit(j * 3 + 2))walk = false;
					if (voxels[cur].hash.getBit(j * 3 + 2) != voxels[tar].hash.getBit(j * 3 + 2))walk = false;
				}
				if (walk) {
					voxels[tar].exist = true;
					queue.push_back(tar);
				}
			}
		}
	}
	pieces.push_back(piece); 
	//********************************************
	int cnt = 0;
	for (int i = 0; i < voxels.size(); i++) {
		if (!voxels[i].exist) continue;
		int ix, iy, iz;
		voxels[i].getXYZ(ix, iy, iz);
		int nei[6] = { -1, -1, -1, -1, -1, -1 };
		if (ix - 1 >= 0)nei[0] = ((ix - 1)* ny * nz + (iy)* nz + (iz));
		if (ix + 1 < nx)nei[1] = ((ix + 1) * ny * nz + (iy)* nz + (iz));
		if (iy - 1 >= 0)nei[2] = ((ix)* ny * nz + (iy - 1)* nz + (iz));
		if (iy + 1 < ny)nei[3] = ((ix)* ny * nz + (iy + 1)* nz + (iz));
		if (iz - 1 >= 0)nei[4] = ((ix)* ny * nz + (iy)* nz + (iz - 1));
		if (iz + 1 < nz)nei[5] = ((ix)* ny * nz + (iy)* nz + (iz + 1));
		int samecnt = 0;
		for (int j = 0; j < 6; j++) {
			if (nei[j] == -1)continue;
			if (!voxels[nei[j]].exist)continue;
			if (voxels[nei[j]].belong!= voxels[i].belong)continue;
			samecnt++;
		}
		if (samecnt < 6) {
			pieces[voxels[i].belong].voxels.push_back(voxels[i].pos);
		}
	}
	std::cout << cnt << std::endl;
}

void Utility::genVoxel() {
	clock_t t1, t2; t1 = clock();

	std::vector<Capsule> caps = std::vector<Capsule>();
	for (int i = 0; i < topo.edgenum; i++) {
		topo.knifes.push_back(Plane(topo.splitNorm[i], topo.getEdgeCent(i)));
		topo.knifeIdx.push_back(i);
		caps.push_back(Capsule(topo.vertices[topo.edges[i].ia], topo.vertices[topo.edges[i].ib], topo.radii));
	}
	std::vector<bool> bits = std::vector<bool>(topo.knifes.size() * 3);
	nx = (ru.x - ld.x) / l + 1;
	ny = (ru.y - ld.y) / l + 1;
	nz = (ru.z - ld.z) / l + 1;
	voxels = std::vector<Voxel>(nx*ny*nz);
	#pragma omp parallel for
	for (int ix = 0; ix < nx; ix++) {
		for (int iy = 0; iy < ny; iy++) {
			for (int iz = 0; iz < nz; iz++) {
				Vector3 pos = ld + Vector3(l*ix, l*iy, l*iz);
				int idx = ix * ny * nz + iy * nz + iz;
				voxels[idx] = Voxel(Hash(), pos, idx);
				std::vector<bool> bits = std::vector<bool>(topo.knifes.size()*3);
				for (int i = 0; i < topo.knifes.size(); i++) {
					bool between;
					bits[i * 3] = caps[i].collide(pos, l, between);//it is L
					bits[i * 3 + 1] = between;
					bits[i * 3 + 2] = (pos - topo.knifes[i].center).dot(topo.knifes[i].normal) >= 0;
					if (bits[i * 3]) voxels[idx].touchid.insert(i);
				}
				voxels[idx].hash.assign(bits);
				voxels[idx].setXYZ(ix, iy, iz);
				voxels[idx].exist = false;
			}
		}
	}
	t2 = clock(); printf("%lf sec\n", (t2 - t1) / (double)(CLOCKS_PER_SEC)); t1 = clock();
	//**************************************
	/*
	//test
	pieces = std::vector<Piece>(topo.edgenum);
	for (int i = 0; i < voxels.size(); i++) {
		std::vector<bool> bits = voxels[i].hash.getBits();
		for (int j = 0; j < bits.size(); j+=3) {
			if (bits[j]) {
				pieces[j / 3].voxels.push_back(voxels[i].pos);
				pieces[j / 3].voxelsi.push_back(i);
				pieces[j / 3].volume++;
				break;
			}
		}
	}
	for (int i = 0; i < pieces.size(); i++) {
		std::cout << pieces[i].voxels.size() << std::endl;
	}
	*/
}

void Utility::genVoxelByKnife() {
	nx = (ru.x - ld.x) / l + 1;
	ny = (ru.y - ld.y) / l + 1;
	nz = (ru.z - ld.z) / l + 1;
	voxels = std::vector<Voxel>(nx*ny*nz);
	#pragma omp parallel for
	for (int ix = 0; ix < nx; ix++) {
		for (int iy = 0; iy < ny; iy++) {
			for (int iz = 0; iz < nz; iz++) {
				Vector3 pos = ld + Vector3(l*ix, l*iy, l*iz);
				int idx = ix * ny * nz + iy * nz + iz;
				std::vector<bool> bits = std::vector<bool>(topo.knifes.size());
				for (int i = 0; i < topo.knifes.size(); i++) {
					bits[i] = topo.knifes[i].distanceToPoint(pos) >= 0;
				}
				voxels[idx] = Voxel(Hash().assign(bits), pos, idx);
				voxels[idx].setXYZ(ix, iy, iz);
			}
		}
	}
	//************************************************************************************
	#pragma omp parallel for
	for (int i = 0; i < voxels.size(); i++) {
		int ix = i / (ny * nz);
		int iy = i / nz % ny;
		int iz = i % nz;
		int nei[6] = { -1, -1, -1, -1, -1, -1 };
		if (ix - 1 >= 0)nei[0] = ((ix - 1)* ny * nz + (iy)* nz + (iz));
		if (ix + 1 < nx)nei[1] = ((ix + 1) * ny * nz + (iy)* nz + (iz));
		if (iy - 1 >= 0)nei[2] = ((ix)* ny * nz + (iy - 1)* nz + (iz));
		if (iy + 1 < ny)nei[3] = ((ix)* ny * nz + (iy + 1)* nz + (iz));
		if (iz - 1 >= 0)nei[4] = ((ix)* ny * nz + (iy)* nz + (iz - 1));
		if (iz + 1 < nz)nei[5] = ((ix)* ny * nz + (iy)* nz + (iz + 1));
		int samecnt = 0;
		for (int j = 0; j < 6; j++) {
			if (nei[j] != -1 && voxels[nei[j]].hash == voxels[i].hash) {
				samecnt++;
			}
		}
		if (samecnt == 6)voxels[i].exist = false;
	}
}

void Utility::genPiece(std::string filename) {
	genPiece(filename, true);
}

void Utility::genPiece(std::string filename, bool output) {
	std::ifstream ifs;
	ifs.open(filename);
	std::string line;
	Vector3 ld, ru; float x, y, z; int n, l, m;
	char str[20];
	std::stringstream ss;
	std::getline(ifs, line); ss = std::stringstream(line);
	ss >> x >> y >> z; ld = Vector3(x, y, z);
	ss >> x >> y >> z; ru = Vector3(x, y, z);
	std::cout << "left low : " << ld << std::endl;
	std::cout << "right up : " << ru << std::endl;
	//********************************************************
	std::vector<Vector3> cs, ns;
	std::getline(ifs, line); ss = std::stringstream(line);
	ss >> n >> l;
	std::cout << "knife number : " << n << std::endl;
	std::cout << "shape number : " << l << std::endl;
	for (int j = 0; j<n; j++) {
		std::getline(ifs, line); ss = std::stringstream(line);
		ss >> x >> y >> z; cs.push_back(Vector3(x, y, z)); 
		ss >> x >> y >> z; ns.push_back(Vector3(x, y, z));
		topo.knifes.push_back(Plane(ns[ns.size() - 1], cs[cs.size() - 1]));
		std::cout << "knife_" << j << " : " << cs[j] << " " << ns[j] << std::endl;

	}
	//********************************************************
	std::cout << "                    " << std::endl;
	pieces.push_back(Piece(Mesh().genCube(ld, ru)));
	for (int j = 0; j< n; j++) {
		std::cout << "                    " << std::endl;
		std::cout << "Apply Knife " << j+1 << "/" << n << std::endl;
		std::vector<Piece> pieces_next = std::vector<Piece>();
		#pragma omp parallel for ordered
		for (int i = 0; i < pieces.size(); i++) {
			Piece piece = pieces[i];
			Piece piece_ = Piece(piece.mesh.slice(cs[j], ns[j]), piece.hash);
			#pragma omp ordered
			if(true){
				if (piece.mesh.vertices.size() > 0) {
					piece.hash.addHash(true);
					pieces_next.push_back(piece);
				}
				if (piece_.mesh.vertices.size() > 0) {
					piece_.hash.addHash(false);
					pieces_next.push_back(piece_);
				}
			}
		}
		pieces.swap(pieces_next);
		std::cout << "Pieces Num : " << pieces.size() << std::endl;
		std::cout << "____________________" << std::endl;
	}
	//*****************************
	/* merge test
	while (pieces.size() > 1) {
		std::vector<Piece> pieces_next;
		for (int i = 0; i < pieces.size(); i+=2) {
			if (i + 1 >= pieces.size())pieces_next.push_back(pieces[i]);
			else {
				pieces[i].mesh.addMesh(pieces[i + 1].mesh);
				pieces_next.push_back(pieces[i]);
			}
		}
		pieces.swap(pieces_next);
		std::cout << "Pieces Num : " << pieces.size() << std::endl;
	}
	*/
	//*****************************
	if (!output)return;
	for (int i = 0; i < pieces.size(); i++) {
		Mesh mesh = pieces[i].mesh;
		char str1[20];
		char str2[20];
		sprintf(str1, "shape_%d", i);
		iglMachine.put(str1, mesh.vertices, mesh.indices);
		sprintf(str2, "pool\\shape_%d.obj", i);
		iglMachine.writeFile(str1, str2);
		std::cout << "Write " << str1 << std::endl;
	}
}

void Utility::genPiece_voxel() {
	clock_t t1, t2; t1 = clock();
	//*****************************
	genVoxelByKnife();
	t2 = clock(); printf("%lf sec\n", (t2 - t1) / (double)(CLOCKS_PER_SEC)); t1 = clock();
	//************************************************************************************
	std::map<Hash, Piece> hashMap;
	for (int i = 0; i < voxels.size(); i++) {
		int ix = i / (ny * nz);
		int iy = i / nz % ny;
		int iz = i % nz;
		Vector3 pos = ld + Vector3(l*ix, l*iy, l*iz);
		if (hashMap.count(voxels[i].hash) == 0) {
			hashMap[voxels[i].hash] = Piece(Mesh(), voxels[i].hash);//insert
		}
		if (voxels[i].exist)hashMap[voxels[i].hash].voxels.push_back(pos);
		hashMap[voxels[i].hash].voxelsi.push_back(i);
		hashMap[voxels[i].hash].volume++;
	}
	std::map<Hash, Piece>::iterator it;
	for (it = hashMap.begin(); it != hashMap.end(); it++) {
		pieces.push_back(it->second);
	}
	t2 = clock(); printf("%lf sec\n", (t2 - t1) / (double)(CLOCKS_PER_SEC)); t1 = clock();
	//************************************************************************************
	#pragma omp parallel for
	for (int i = 0; i < pieces.size(); i++) {
		//voxel mode gen .voxel but no mesh
		//for (int j = 0; j < pieces[i].voxels.size(); j++) {
		//	Vector3 pos = pieces[i].voxels[j];
		//	pieces[i].mesh.addMesh(Mesh().genCube(pos - Vector3(l / 2, l / 2, l / 2), pos + Vector3(l / 2, l / 2, l / 2)));//it is L
		//}
		for (int j = 0; j < pieces[i].voxelsi.size(); j++) {
			voxels[pieces[i].voxelsi[j]].belong = i;
		}
		pieces[i].id = i;
	}
	t2 = clock(); printf("%lf sec\n", (t2 - t1) / (double)(CLOCKS_PER_SEC)); t1 = clock();
	//************************************************************************************
	//printf("l = %f, size = %d x %d x %d, cost %lf sec\n", l, nx, ny, nz, (t2 - t1) / (double)(CLOCKS_PER_SEC));
	//************************************************************************************
}

void Utility::genPiece() {
	pieces.push_back(Piece(Mesh().genCube(ld, ru)));
	int n = topo.knifes.size();
	for (int j = 0; j< n; j++) {
		std::cout << "                    " << std::endl;
		std::cout << "Apply Knife " << j + 1 << "/" << n << std::endl;
		std::vector<Piece> pieces_next = std::vector<Piece>();
		#pragma omp parallel for
		for (int i = 0; i < pieces.size(); i++) {
			Piece piece = pieces[i];
			Piece piece_ = Piece(piece.mesh.slice(topo.knifes[j].center, topo.knifes[j].normal), piece.hash);
			if (true) {
				if (piece.mesh.vertices.size() > 0) {
					piece.hash.addHash(true);
					#pragma omp critical
					pieces_next.push_back(piece);
				}
				if (piece_.mesh.vertices.size() > 0) {
					piece_.hash.addHash(false);
					#pragma omp critical
					pieces_next.push_back(piece_);
				}
			}
		}

		pieces.swap(pieces_next);
		std::cout << "Pieces Num : " << pieces.size() << std::endl;
		std::cout << "____________________" << std::endl;
	}
	for (int i = 0; i < pieces.size(); i++) {
		pieces[i].id = i;
	}
}

void Utility::genPiece_slice() {
	float fix = 0;
	std::ifstream infile("pieceinfo.txt");
	int shapenum;  infile >> shapenum;
	for(int i=0;i<shapenum;i++)
	{
		int pieceNum; infile >> pieceNum;
		std::cout << "Shape" << i << ", Piecenum : " << pieceNum << std::endl;
		Group group;
		for (int i = 0; i < pieceNum; i++) {
			Piece piece;
			Vector3 pos;
			Vector3 size;
			infile >> pos.x >> pos.y >> pos.z >> size.x >> size.y >> size.z;
			std::string line;
			std::getline(infile, line);
			//***************************
			piece.mesh = Mesh().genCube(pos - size / 2, pos + size / 2);
			//mesh.slice
			std::getline(infile, line);
			std::istringstream iss(line);
			Vector3 cent, norm;
			while (iss >> cent.x >> cent.y >> cent.z >> norm.x >> norm.y >> norm.z) {
				cent = cent + norm.normalize() * fix;
				piece.mesh.slice(cent, norm);
			}
			pieces.push_back(piece);
			group.pieces.push_back(pieces.size()-1);
		}
		group.id = groups.size();
		group.avanum = 0;
		groups.push_back(group);
		idexist.push_back(true);
	}
	//*******************************************************************************************
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX), max = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	for(auto group : groups){
		for (auto p : group.pieces) {
			for (int j = 0; j < pieces[p].mesh.vertices.size(); j += 3) {
				Vector3 vert = Vector3(pieces[p].mesh.vertices[j], pieces[p].mesh.vertices[j + 1], pieces[p].mesh.vertices[j + 2]);
				min.x = std::min(min.x, vert.x);
				min.y = std::min(min.y, vert.y);
				min.z = std::min(min.z, vert.z);
				max.x = std::max(max.x, vert.x);
				max.y = std::max(max.y, vert.y);
				max.z = std::max(max.z, vert.z);
			}
		}
	}
	//*****************************
	//shell
	Piece piece;
	piece.mesh = Mesh().genCube(min - Vector3(10, 10, 10), max + Vector3(10, 10, 10));
	Mesh mesh = Mesh().genCube(min - Vector3(2, 2, 0), max + Vector3(2, 2, 11));
	iglMachine.put("shell", piece.mesh.vertices, piece.mesh.indices);
	iglMachine.put("inner", mesh.vertices, mesh.indices);
	char str[50];
	sprintf(str, "- shell shell inner");
	iglMachine.command(str);
	iglMachine.get("shell", piece.mesh.vertices, piece.mesh.indices);
	pieces.push_back(piece);
	Group group;
	group.pieces.push_back(pieces.size() - 1);
	group.id = groups.size();
	group.avanum = 0;
	groups.push_back(group);
	idexist.push_back(true);
	//*****************************
	//floor
	Piece piece2;
	piece2.mesh = Mesh().genCube(Vector3(min.x, min.y, min.z) - Vector3(10, 10, 10), Vector3(max.x, max.y, min.z) + Vector3(10, 10, 0));
	pieces.push_back(piece2);
	Group group2;
	group2.pieces.push_back(pieces.size() - 1);
	group2.id = groups.size();
	group2.avanum = 0;
	groups.push_back(group2);
	idexist.push_back(true);
}

void Utility::csgShape() {
	Mesh sphere; sphere.load("pool/sphere.obj");
	Mesh cylinder; cylinder.load("pool/cylinder.obj");
	Mesh all;
	float r = topo.radii * 3;
	for (int i = 0; i < topo.vertices.size(); i++) {
		Mesh mesh = sphere;
		mesh.scale(Vector3(r, r, r));
		mesh.translate(topo.vertices[i]);
		all.addMesh(mesh);
		//char str1[20];
		//char str2[20];
		//sprintf(str1, "vert_%d", i);
		//iglMachine.put(str1, mesh.vertices, mesh.indices);
		//sprintf(str2, "pool\\vert_%d.obj", i);
		//iglMachine.writeFile(str1, str2);
	}
	for (int i = 0; i < topo.edges.size(); i++) {
		TopoEdge edge = topo.edges[i];
		Vector3 v1 = topo.vertices[edge.ia];
		Vector3 v2 = topo.vertices[edge.ib];
		Vector3 vec = (v2 - v1).normalize();
		Vector3 cent = (v1 + v2) / 2;
		float len = (v2 - v1).length();
		Mesh mesh = cylinder;
		mesh.scale(Vector3(r, r, len));
		mesh.rotateTo(vec);
		mesh.translate(cent);
		all.addMesh(mesh);
		//char str1[20];
		//char str2[20];
		//sprintf(str1, "edge_%d", i);
		//iglMachine.put(str1, mesh.vertices, mesh.indices);
		//sprintf(str2, "pool\\edge_%d.obj", i); 
		//iglMachine.writeFile(str1, str2);
	}
	//********************************************
	Vector3 high(0, 0, FLT_MIN);
	std::vector<bool> localhigh(topo.vertices.size(), true);
	for (int i = 0; i < topo.vertices.size(); i++) {
		if (topo.vertices[i].z > high.z) {
			high = topo.vertices[i];
		}
		for (auto v : topo.verticevertice[i]) {
			if (topo.vertices[v].z > topo.vertices[i].z)localhigh[i] = false;
		}
	}
	float range = 10;
	for (int i = 0; i < topo.vertices.size(); i++) {
		if (topo.vertices[i].z > high.z - range && localhigh[i]) {
			Mesh mesh = cylinder;
			mesh.scale(Vector3(r, r, 100));
			//mesh.rotateTo(Vector3(0,0,1));
			mesh.translate(topo.vertices[i] + Vector3(0, 0, 50));
			all.addMesh(mesh);
			std::cout << "high point : " << topo.vertices[i] << std::endl;
		}
	}
	//********************************************
	iglMachine.put("all", all.vertices, all.indices);
	iglMachine.command("+ all all null");
	std::cout << "Generate Thinstruct" << std::endl;
	//*****************************************
	for (int i = 0; i < pieces.size(); i++) {
		char str[20];
		sprintf(str, "piece_%d", i);
		iglMachine.put(str, pieces[i].mesh.vertices, pieces[i].mesh.indices);
	}

	/*
	#pragma omp parallel for
	for (int i = 0; i < pieces.size(); i++) {
		printf("CSG piece %d\n", i);
		char str[50];
		sprintf(str, "- piece_%d piece_%d all", i, i);
		iglMachine.command(str);
		//for (int j = 0; j < topo.vertices.size(); j++) {
		//	printf("CSG piece %d on %d\n", i, j);
		//	sprintf(str, "- piece_%d piece_%d vert_%d", i, i, j);
		//	iglMachine.command(std::string(str));
		//}
		//for (int j = 0; j < topo.edges.size(); j++) {
		//	sprintf(str, "- piece_%d piece_%d edge_%d", i, i, j);
		//	iglMachine.command(std::string(str));
		//}
		sprintf(str, "piece_%d", i);
		iglMachine.get(str, pieces[i].mesh.vertices, pieces[i].mesh.indices);
	}
	*/
}

void Utility::initGroup() {
	groups = std::vector<Group>(pieces.size());
	#pragma omp parallel for
	for (int i = 0; i < pieces.size(); i++) {
		Group group;
		group.initAva();
		topo.calTouch(pieces[i]);
		appendPiece(group, pieces[i]);
		group.id = i;
		groups[i] = group;
	}
	groupidcnt = pieces.size();
	groupIdxMap = std::vector<int>(groups.size());
	for (int i = 0; i < groups.size(); i++) {
		groupIdxMap[groups[i].id] = i;//groupIdxMap.insert(std::pair<int, int>(groups[i].id, i));
		idexist.push_back(true);
	}
}

void Utility::initLink() {
	for (int i = 0; i < groups.size(); i++) {
		std::vector<int> forces = std::vector<int>(groups.size());
		#pragma omp parallel for
		for (int j = i + 1; j < groups.size(); j++) {
			forces[j] = 0;
			for (int n = 0; n < groups[i].pieces.size(); n++) {
				for (int m = 0; m < groups[j].pieces.size(); m++) {
					if (pieces[groups[i].pieces[n]].isNei(pieces[groups[j].pieces[m]])) {
						forces[j]++;
					}
				}
			}
		}
		for (int j = i + 1; j < groups.size(); j++) {
			if (forces[j] > 0) {
				groups[i].neighbor.insert(groups[j].id);
				groups[j].neighbor.insert(groups[i].id);
				groupLink.push(GroupLink(groups[i].id, groups[j].id, calWorth(groups[i], groups[j]), groups[i].volume + groups[j].volume));
			}
		}
	}
}

void Utility::initLink_voxel() {
	#pragma omp parallel for
	for (int i = 0; i < voxels.size(); i++) {
		int ix = i / (ny * nz);
		int iy = i / nz % ny;
		int iz = i % nz;
		int nei[6] = { -1, -1, -1, -1, -1, -1 };
		if (ix - 1 >= 0)nei[0] = ((ix - 1)* ny * nz + (iy)* nz + (iz));
		if (ix + 1 < nx)nei[1] = ((ix + 1) * ny * nz + (iy)* nz + (iz));
		if (iy - 1 >= 0)nei[2] = ((ix)* ny * nz + (iy - 1)* nz + (iz));
		if (iy + 1 < ny)nei[3] = ((ix)* ny * nz + (iy + 1)* nz + (iz));
		if (iz - 1 >= 0)nei[4] = ((ix)* ny * nz + (iy)* nz + (iz - 1));
		if (iz + 1 < nz)nei[5] = ((ix)* ny * nz + (iy)* nz + (iz + 1));
		int samecnt = 0;
		for (int j = 0; j < 6; j++) {//assume piece id is equal to group id
			if (nei[j] != -1) {
				int a = voxels[i].belong;
				int b = voxels[nei[j]].belong;
				groups[a].neighbor.insert(groups[b].id);
				groups[b].neighbor.insert(groups[a].id);
			}
		}
	}
	for (int i = 0; i < groups.size();i++) {
		for (auto jj : groups[i].neighbor) {
			int j = groupIdxMap[jj];
			if (i >= j)continue;
			groupLink.push(GroupLink(groups[i].id, groups[j].id, calWorth(groups[i], groups[j]), groups[i].volume + groups[j].volume));
		}
	}
}

void Utility::MergeGroup(Group& group1, Group& group2) {
	Group group = group1;
	for (int i = 0; i < group2.pieces.size(); i++) {
		appendPiece(group, pieces[group2.pieces[i]]);
	}
	group.id = groupidcnt++;
	idexist.push_back(true);
	idexist[group1.id] = idexist[group2.id] = false;
	group.neighbor.swap(std::set<int>());
	for (auto id : group1.neighbor)if (id != group1.id && id != group2.id && idexist[id]) { group.neighbor.insert(id);}
	for (auto id : group2.neighbor)if (id != group1.id && id != group2.id && idexist[id]) { group.neighbor.insert(id);}
	//*******************************//use idexist to check, so no need
	//std::vector<Group> groups_new;
	//for (int i = 0; i < groups.size(); i++) {
	//	if (groups[i].id != group1.id && groups[i].id != group2.id) {
	//		//groups[i].neighbor.erase(group1.id);//用idexist加速
	//		//groups[i].neighbor.erase(group2.id);
	//		groups_new.push_back(groups[i]);
	//		groupIdxMap[groups[i].id] = (int)(groups_new.size() - 1);
	//	}
	//}
	//groups.swap(groups_new);
	//groupIdxMap.erase(group1.id);//use idexist to check
	//groupIdxMap.erase(group2.id);//now group1,2 is not avaliable now
	//*******************************
	
	groups.push_back(group);
	groupIdxMap.push_back((int)(groups.size() - 1));//groupIdxMap.insert(std::pair<int, int>(group.id, (int)(groups.size() - 1)));
	for (auto id : group.neighbor) {
		groups[groupIdxMap[id]].neighbor.insert(group.id);
		float worth = calWorth(group, groups[groupIdxMap[id]]);
		float volume = (group.volume + groups[groupIdxMap[id]].volume);
		groupLink.push(GroupLink(group.id, id, worth, volume));
	}
	
}

void Utility::appendPiece(Group& group, Piece& piece){
	group.pieces.push_back(piece.id);
	group.volume += piece.volume;
	topo.purneAva(group, piece);
}

void Utility::calBound() {
	ld = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	ru = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	for (int i = 0; i < topo.vertices.size(); i++) {
		ld.x = ld.x < topo.vertices[i].x ? ld.x : topo.vertices[i].x;
		ld.y = ld.y < topo.vertices[i].y ? ld.y : topo.vertices[i].y;
		ld.z = ld.z < topo.vertices[i].z ? ld.x : topo.vertices[i].z;
		ru.x = ru.x > topo.vertices[i].x ? ru.x : topo.vertices[i].x;
		ru.y = ru.y > topo.vertices[i].y ? ru.y : topo.vertices[i].y;
		ru.z = ru.z > topo.vertices[i].z ? ru.x : topo.vertices[i].z;
	}
	float radii = topo.radii;
	ld -= Vector3(radii * 2, radii * 2, radii * 2);
	ru += Vector3(radii * 2, radii * 2, radii * 2);
}

void Utility::optimize() {
	int cnt = groups.size();
	while (true) {
		GroupLink gl = groupLink.top();
		groupLink.pop();
		//std::cout << "Top link energy = " << gl.worth << std::endl;
		if (!idexist[gl.ida]|| !idexist[gl.idb])continue; //if (groupIdxMap.count(gl.ida) == 0 || groupIdxMap.count(gl.idb) == 0)continue;
		if (gl.worth == 0)break;
		MergeGroup(groups[groupIdxMap[gl.ida]], groups[groupIdxMap[gl.idb]]);
		cnt--;
		std::cout << "Group number left : " << cnt << std::endl;
		if (cnt <= 1)break;
	}
}

void Utility::outputPiece() {
	for (int i = 0; i < pieces.size(); i++) {
		Mesh mesh = pieces[i].mesh;
		char str1[20];
		char str2[20];
		sprintf(str1, "piece_%d", i);
		iglMachine.put(str1, mesh.vertices, mesh.indices);
		sprintf(str2, "pool\\piece_%d.obj", i);
		iglMachine.writeFile(str1, str2);
		std::cout << "Write " << str1 << std::endl;
	}
}

void Utility::outputKnife() {
	FILE* fp = fopen("knifeinfo.txt", "w");
	for (int i = 0; i < topo.knifeIdx.size(); i++) {
		fprintf(fp, "%d\n", topo.knifeIdx[i]);
	}
	fclose(fp);
}

void Utility::outputPiece_voxel() {
	int cnt = 0;
	for (int i = 0; i < pieces.size(); i++) {
		char filename[20]; sprintf(filename, "pool\\shape_%d.voxel", cnt);
		FILE* fp = fopen(filename, "w");
		std::vector<Vector3> poss;
		for (int j = 0; j < pieces[i].voxels.size(); j++) {
			poss.push_back(pieces[i].voxels[j]);
		}
		fprintf(fp, "%d %f\n", poss.size(), l);
		for (auto pos : poss) {
			fprintf(fp, "%f %f %f\n", pos.x, pos.y, pos.z);
		}
		fclose(fp);
		std::cout << "Write " << "shape_" << cnt << ".voxel" << std::endl;
		cnt++;
	}
	FILE* fp = fopen("shapeinfo.txt", "w");
	fprintf(fp, "%d\n", cnt);
	for (int i = 0; i < pieces.size(); i++) {
		fprintf(fp, "0 0 0\n");
	}
	fclose(fp);
}

void Utility::outputGroup_voxel() {
	int cnt = 0;
	for (int i = 0; i < groups.size(); i++) {
		if (!idexist[groups[i].id])continue;
		char filename[20]; sprintf(filename, "pool\\shape_%d.voxel", cnt);
		FILE* fp = fopen(filename, "w");
		std::vector<Vector3> poss;
		for (int j = 0; j < groups[i].pieces.size();j++) {
			for (int k = 0; k < pieces[groups[i].pieces[j]].voxels.size(); k++) {
				poss.push_back(pieces[groups[i].pieces[j]].voxels[k]);
			}
		}
		fprintf(fp, "%d %f\n",poss.size(), l);
		for (auto pos : poss) {
			fprintf(fp, "%f %f %f\n", pos.x, pos.y, pos.z);
		}
		fclose(fp);
		std::cout << "Write " << "shape_" << cnt << ".voxel" << std::endl;
		cnt++;
	}
	FILE* fp = fopen("shapeinfo.txt", "w");
	fprintf(fp, "%d\n", cnt);
	for (int i = 0; i < groups.size(); i++) {
		if (!idexist[groups[i].id])continue;
		Vector3 dir = Vector3(0, 0, 0);
		if (groups[i].avanum > 0) {
			for (int j = 0; j < groups[i].avanum; j++)dir += groups[i].ava[j];
			dir /= groups[i].avanum;
			fprintf(fp, "%f %f %f\n", dir.x, dir.y, dir.z);
		}
		else fprintf(fp, "0 0 0\n");
	}
	fclose(fp);
}

void Utility::outputGroup() {
	int cnt = 0;
	//std::map<int, int> cntMap;
	std::vector<std::pair<int, int>>cntMap;
	for (int i = 0; i < groups.size(); i++) {
		if (!idexist[groups[i].id])continue;
		else cntMap.push_back(std::pair<int, int>(i, cnt++));
	}
	char str[50];
	char str1[20];
	char str2[20];
	for (auto ii : cntMap) {
		int i = ii.first;
		int j = ii.second;
		if (!idexist[groups[i].id])continue;
		Mesh mesh = groups[i].getMesh(pieces);//need ref
		if (mesh.vertices.size() == 0)continue;
		//mesh.merge();
		sprintf(str1, "shape_%d", j);
		iglMachine.put(str1, mesh.vertices, mesh.indices);
	}
	//sprintf(str, "NEW null");iglMachine.command(str);

	#pragma omp parallel for
	for(int ii=0;ii<cntMap.size();ii++){
		int i = cntMap[ii].first;
		int j = cntMap[ii].second;
		Mesh mesh = groups[i].getMesh(pieces);//need ref
		//mesh.scale(Vector3(1.001f, 1.001f, 1.001f));
		if (mesh.vertices.size() == 0)continue;
		/**/
		char str[50];
		char str1[20];
		char str2[20];
		sprintf(str1, "shape_%d", j);
		sprintf(str2, "pool\\shape_%d.obj", j);
		/**/
		sprintf(str, "+ shape_%d shape_%d null", j, j);
		iglMachine.command(str);
		/**/
		//iglMachine.get(str1, mesh.vertices, mesh.indices);
		//mesh.shrink(2);
		//iglMachine.put(str1, mesh.vertices, mesh.indices);
		/**/
		sprintf(str, "- shape_%d shape_%d all", j, j);
		iglMachine.command(str);
		iglMachine.writeFile(str1, str2);
		printf("Write %s\n", str1);
		//std::cout << "Write " << str1 << std::endl;
	}
	FILE* fp = fopen("shapeinfo.txt","w");
	fprintf(fp, "%d\n", cnt);
	for (int i = 0; i < groups.size();i++) {
		if (!idexist[groups[i].id])continue;
		Vector3 dir = Vector3(0, 0, 0);
		if (groups[i].avanum > 0) {
			for (int j = 0; j < groups[i].avanum; j++)dir += groups[i].ava[j];
			dir /= groups[i].avanum;
			fprintf(fp, "%f %f %f\n", dir.x, dir.y, dir.z);
		}
		else fprintf(fp, "0 0 0\n");
	}
	fclose(fp);
}

float Utility::calWorth(Group& group1, Group& group2) {
	//return 1;
	Group group = group1;
	for (int i = 0; i < group2.pieces.size(); i++) {
		topo.purneAva(group, pieces[group2.pieces[i]]);
	}
	return group.avanum;
}
//********************************************************

//旋轉至最少knife
//旋轉改最大值

/* BKUP : csg method
void Utility::genPiece(std::string filename) {
std::ifstream ifs;
ifs.open(filename);
std::string line;
Vector3 ld, ru; float x, y, z; int n, l, m;
char str[20];
std::stringstream ss;
std::getline(ifs, line); ss = std::stringstream(line);
ss >> x >> y >> z; ld = Vector3(x, y, z);
ss >> x >> y >> z; ru = Vector3(x, y, z);
std::cout << "left low : " << ld << std::endl;
std::cout << "right up : " << ru << std::endl;
//********************************************************
std::vector<Vector3> cs, ns;
std::getline(ifs, line); ss = std::stringstream(line);
ss >> n >> l;
std::cout << "knife number : " << n << std::endl;
std::cout << "shape number : " << l << std::endl;
for (int j = 0; j<n; j++) {
std::getline(ifs, line); ss = std::stringstream(line);
ss >> x >> y >> z; cs.push_back(Vector3(x, y, z));
ss >> x >> y >> z; ns.push_back(Vector3(x, y, z));
std::cout << "knife_" << j << " : " << cs[j] << " " << ns[j] << std::endl;
}
//********************************************************
std::cout << "________________________________________" << std::endl;
cgalnef.genCube(ld, ru);
std::cout << "Cube generated..." << std::endl;
cgalnef.genPiece(cs, ns);
std::cout << "Pieces generated..." << std::endl;
for (int i = 0; i < cgalnef.Ns.size(); i++) {
Mesh mesh;
cgaltool.readFromOFFStream(mesh.vertices, mesh.indices, cgalnef.getOffStr(cgalnef.Ns[i]));
char str1[20], str2[20];
sprintf(str1, "shape_%d", i);
iglMachine.put(str1, mesh.vertices, mesh.indices);
sprintf(str2, "pool\\shape_%d.obj", i);
iglMachine.writeFile(str1, str2);
std::cout << "Pieces " << i << " Writed!" << std::endl;
}
}
*/

/* BKUP : my slicing step
 *
if(chs[i]=='*')mesh.slice(cs[i], ns[i]);
if(chs[i]=='-')mesh.slice(cs[i], ns[i]*-1);
fillHole(mesh);
*/

/* BKUP : dividing slicing
 *
std::vector<Mesh> meshes;
std::vector<Mesh> meshes_next;
meshes.push_back(Mesh().genCube(ld, ru));
for(int j=0;j<n;j++){
ss = std::stringstream(line);
ss >> x >> y >> z;
Vector3 c(x,y,z);
ss >> x >> y >> z;
Vector3 n(x,y,z);
for(int i=0;i<meshes.size();i++){
Mesh mesh = meshes[i];
Mesh mesh_ = mesh.slice(c, n);
fillHole(mesh);fillHole(mesh_);
if(mesh.indices.size()>0)meshes_next.push_back(mesh);
if(mesh_.indices.size()>0)meshes_next.push_back(mesh_);
}
meshes.swap(meshes_next);
meshes_next.swap(std::vector<Mesh>());
}
int cnt=0;
for(int i=0;i<meshes.size();i++){
Mesh mesh = meshes[i];
char str1[20];
char str2[20];
sprintf(str1, "shape_%d", i);
iglMachine.put(str1, mesh.vertices, mesh.indices);
sprintf(str2, "pool\\shape_%d.obj", i);
iglMachine.writeFile(str1,str2);
}
*/
