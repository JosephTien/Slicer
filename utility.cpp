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

void Utility::genPiece() {
	pieces.push_back(Piece(Mesh().genCube(ld, ru)));
	int n = topo.knifes.size();
	for (int j = 0; j< n; j++) {
		std::cout << "                    " << std::endl;
		std::cout << "Apply Knife " << j + 1 << "/" << n << std::endl;
		std::vector<Piece> pieces_next = std::vector<Piece>();
		#pragma omp parallel for ordered
		for (int i = 0; i < pieces.size(); i++) {
			Piece piece = pieces[i];
			Piece piece_ = Piece(piece.mesh.slice(topo.knifes[j].center, topo.knifes[j].normal), piece.hash);
			#pragma omp ordered
			if (true) {
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
}

void Utility::initGroup() {
	for (int i = 0; i < pieces.size(); i++) {
		Group group;
		group.initAva();
		topo.calTouch(pieces[i]);
		appendPiece(group, pieces[i]);
		/**/
		//std::cout << i << " " << pieces[i].touchinfos.size() << " " << group.ava.size() << std::endl;
		//for (int j = 0; j < pieces[i].touchinfos.size(); j++) {
		//	std::cout << "		" << pieces[i].touchinfos[j].dir << " " << pieces[i].touchinfos[j].knifeid << std::endl;
		//}
		/**/
		group.id = groupidcnt++;
		groups.push_back(group);
		groupIdxMap.insert(std::pair<int, int>(group.id, i));
	}
	//***********************************
	for (int i = 0; i < groups.size(); i++) {
		for (int j = i+1; j < groups.size(); j++) {
			float force = 0;
			for (int n = 0; n < groups[i].pieces.size(); n++) {
				for (int m = 0; m < groups[j].pieces.size(); m++) {
					if (groups[i].pieces[n].isNei(groups[j].pieces[m])) {
						force++;
					}
				}
			}
			if (force > 0) {
				groupLink.push(GroupLink(groups[i].id, groups[j].id, calWorth(groups[i], groups[j]), force));//force currently no function
				groups[i].neighbor.insert(groups[j].id);
				groups[j].neighbor.insert(groups[i].id);
			}
		}
		//std::cout << "Group " << i << " has " << groups[i].pieces.size() << " pieces" << std::endl;
		//std::cout << "Group " << i << " has " << groups[i].neighbor.size() << " neighbors"  << std::endl;
	}
}

void Utility::MergeGroup(Group& group1, Group& group2) {
	Group group = group1;
	for (int i = 0; i < group2.pieces.size(); i++) {
		appendPiece(group, group2.pieces[i]);
	}
	group.id = groupidcnt++;
	group.neighbor.swap(std::set<int>());
	for (auto id : group1.neighbor)if (id != group1.id && id != group2.id) { group.neighbor.insert(id);}
	for (auto id : group2.neighbor)if (id != group1.id && id != group2.id) { group.neighbor.insert(id);}
	//for (auto id : group.neighbor) std::cout << id << std::endl;
	//*******************************
	std::vector<Group> groups_new;
	for (int i = 0; i < groups.size(); i++) {
		if (groups[i].id != group1.id && groups[i].id != group2.id) {
			groups[i].neighbor.erase(group1.id);
			groups[i].neighbor.erase(group2.id);
			groups_new.push_back(groups[i]);
			groupIdxMap[groups[i].id] = (int)(groups_new.size() - 1);
		}
	}

	groupIdxMap.erase(group1.id);
	groupIdxMap.erase(group2.id);//now group1,2 is not avaliable now
	groups.swap(groups_new);
	//*******************************
	groups.push_back(group);
	groupIdxMap.insert(std::pair<int, int>(group.id, (int)(groups.size() - 1)));
	for (auto id : group.neighbor) {
		groups[groupIdxMap[id]].neighbor.insert(group.id);
		groupLink.push(GroupLink(group.id, id, calWorth(group, groups[groupIdxMap[id]]), 0));//force not avaliable
	}
}

void Utility::appendPiece(Group& group, Piece& piece){
	group.pieces.push_back(piece);
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
	while (true) {
		GroupLink gl = groupLink.top();
		groupLink.pop();
		std::cout << "Top link energy = " << gl.worth << std::endl;
		if (groupIdxMap.count(gl.ida) == 0 || groupIdxMap.count(gl.idb) == 0)continue;
		if (gl.worth == 0)break;

		MergeGroup(groups[groupIdxMap[gl.ida]], groups[groupIdxMap[gl.idb]]);
		std::cout << "Group number left : " << groups.size() << std::endl;
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

void Utility::outputGroup() {
	for (int i = 0; i < groups.size(); i++) {
		Mesh mesh = groups[i].getMesh();
		char str1[20];
		char str2[20];
		sprintf(str1, "shape_%d", i);
		iglMachine.put(str1, mesh.vertices, mesh.indices);
		sprintf(str2, "pool\\shape_%d.obj", i);
		iglMachine.writeFile(str1, str2);
		std::cout << "Write " << str1 << std::endl;
	}
	FILE* fp = fopen("shapeinfo.txt","w");
	fprintf(fp, "%d\n", groups.size());
	for (int i = 0; i < groups.size();i++) {
		Vector3 dir = Vector3(0, 0, 0);
		if (groups[i].ava.size() > 0) {
			for (int j = 0; j < groups[i].ava.size(); j++)dir += groups[i].ava[j];
			dir /= groups[i].ava.size();
			fprintf(fp, "%f %f %f\n", dir.x, dir.y, dir.z);
		}
		else fprintf(fp, "0 0 0\n");
	}
	fclose(fp);
}

float Utility::calWorth(Group& group1, Group& group2) {
	Group group = group1;
	for (int i = 0; i < group2.pieces.size(); i++) {
		topo.purneAva(group, group2.pieces[i]);
	}
	return group.ava.size();
}
//********************************************************

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
