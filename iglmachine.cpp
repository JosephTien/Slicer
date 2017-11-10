#include<iglmachine.h>
/*
 * Note : will always get if the name is wrong
 * Note : won't write file if the name is wrong
 */

void IglMesh::setName(std::string name){
    this->name = name;
}
void IglMesh::getData(std::vector<float> &vertices, std::vector<unsigned int> &indices){
    this->vertices.clear();
    this->indices.clear();
    for(int i=0;i<V.rows();i++){
        this->vertices.push_back(V(i,0));
        this->vertices.push_back(V(i,1));
        this->vertices.push_back(V(i,2));
    }
    for(int i=0;i<F.rows();i++){
        this->indices.push_back(F(i,0));
        this->indices.push_back(F(i,1));
        this->indices.push_back(F(i,2));
    }
    vertices = this->vertices;
    indices.clear();
    for(int i=0;i<this->indices.size();i++){
        indices.push_back(this->indices[i]);
    }
}
void IglMesh::setData(std::vector<float> vertices, std::vector<int> indices){
    this->vertices = vertices;
    this->indices = indices;
    V.resize(vertices.size()/3,3);
    F.resize(indices.size()/3,3);
    for(int i=0;i<V.rows();i++){
        V(i,0) = vertices[i*3];
        V(i,1) = vertices[i*3+1];
        V(i,2) = vertices[i*3+2];
    }
    for(int i=0;i<F.rows();i++){
        F(i,0) = indices[i*3];
        F(i,1) = indices[i*3+1];
        F(i,2) = indices[i*3+2];
    }
}
void IglMesh::setData(std::vector<float> vertices, std::vector<unsigned int> indices){
    this->indices.clear();
    for(int i=0;i<(int)indices.size();i++){
        this->indices.push_back((int)indices[i]);
    }
    setData(vertices, this->indices);
}

void IglMesh::setData(Eigen::MatrixXd V, Eigen::MatrixXi F){
    this->V = V;
    this->F = F;
    std::vector<double> vertices_;
    vertices_.resize(V.rows()*V.cols(),0);
    Eigen::MatrixXd::Map(vertices_.data(), 3, V.rows()) = V.transpose();
    vertices.clear();
    for(int i=0;i<(int)vertices_.size();i++){
        vertices.push_back(vertices_[i]);
    }
    indices.resize(F.rows()*F.cols(),0);
    Eigen::MatrixXi::Map(indices.data(), 3, F.rows()) = F.transpose();
}
void IglMesh::addData(Eigen::MatrixXd V, Eigen::MatrixXi F){
        int oldVnum = this->V.rows();
        int oldFnum = this->F.rows();
        this->V.conservativeResize(oldVnum+V.rows(),3);
        this->F.conservativeResize(oldFnum+F.rows(),3);
        for(int i=0;i<V.rows();i++){
            this->V(i+oldVnum,0) = V(i,0);
            this->V(i+oldVnum,1) = V(i,1);
            this->V(i+oldVnum,2) = V(i,2);
        }
        for(int i=0;i<F.rows();i++){
            this->F(i+oldFnum,0) = F(i,0)+oldVnum;
            this->F(i+oldFnum,1) = F(i,1)+oldVnum;
            this->F(i+oldFnum,2) = F(i,2)+oldVnum;
        }
    }
void IglMesh::set(std::string name,std::vector<float> vertices, std::vector<int> indices){
    setName(name);
    setData(vertices, indices);
}
void IglMesh::set(std::string name,std::vector<float> vertices, std::vector<unsigned int> indices){
    setName(name);
    setData(vertices, indices);
}
void IglMesh::set(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F){
    setName(name);
    setData(V,F);
}
//*****************************************************************************************
int IglMachine::searchTar(std::string tarName) {//force creat
	return searchTar(tarName, true);
}
int IglMachine::searchTar(std::string tarName, bool create){
	/*
    for(int i=0;i<mesh.size();i++){
        if(tarName == mesh[i].name){
            return i;
        }
    }
	*/
	std::map<std::string, int>::iterator iter = nameMap.find(tarName);
	if (iter != nameMap.end()) {
		return iter->second;
	}
	if (create) {
		int tar = searchEmpty();
		mesh[tar].setName(tarName);
		insert(tarName, tar);
		return tar;
	}
    return -1;
}
void IglMachine::insertEmpty() {
	mesh.push_back(IglMesh());
	nameMap.insert(std::pair<std::string, int>("", mesh.size() - 1));
	emptySet.insert(mesh.size() - 1);
}
void IglMachine::insert(std::string name, int tar) {
	nameMap.insert(std::pair<std::string, int>(name, tar));
	emptySet.erase(tar);
}
int IglMachine::searchEmpty() {

	/*
	for (int i = 0; i<mesh.size(); i++) {
		if (mesh[i].name == "") {
			std::cout << "A quick room!";
			return i;
		}
	}
	*/
	int tar = -1;
	for (auto i : emptySet) {
		tar = i;
		break;
	}
	if (tar >= 0) {
		emptySet.erase(tar);
		return tar;
	}
	insertEmpty();
	if (mesh.size() == dataNumLimit)std::cout << "No more room for iglmachine...";
	return mesh.size() - 1;
}
void IglMachine::putNew(std::string name){
    int tar = searchTar(name);
    if(tar == -1){
		tar = searchEmpty();
        mesh[tar].setName(name);
		insert(name, tar);
    }else{
        std::cout << "Warning : \"" << name << "\" already exist" << std::endl;
    }
}
void IglMachine::put(std::string name,std::vector<float> vertices, std::vector<unsigned int> indices){
    int tar = searchTar(name);
    if(tar == -1){
		tar = searchEmpty();
		mesh[tar].set(name, vertices, indices);
		insert(name, tar);
    }else{
        mesh[tar].setData(vertices, indices);
    }
}
void IglMachine::put(std::string name,std::vector<float> vertices, std::vector<int> indices){
    int tar = searchTar(name);
    if(tar == -1){
		tar = searchEmpty();
		mesh[tar].set(name, vertices, indices);
		insert(name, tar);
    }else{
        mesh[tar].setData(vertices, indices);
    }
}
void IglMachine::put(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F){
    int tar = searchTar(name);
    if(tar == -1){
		tar = searchEmpty();
		mesh[tar].set(name, V, F);
		insert(name, tar);
    }else{
        mesh[tar].setData(V, F);
    }
}
void IglMachine::add(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F){
        int tar = searchTar(name);
        if(tar == -1){
			tar = searchEmpty();
			mesh[tar].set(name, V, F);
			insert(name, tar);
        }else{
            mesh[tar].addData(V, F);
        }
    }
void IglMachine::get(std::string name,std::vector<float> &vertices, std::vector<unsigned int> &indices){
    int tar = searchTar(name);
    if(tar != -1){
        mesh[tar].getData(vertices, indices);
    }
}

void IglMachine::readFile(std::string name, const char * filePath){
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(filePath,V,F);
    put(name, V, F);
}
void IglMachine::writeFile(std::string name, char * filePath){
    int tar = searchTar(name, false);
    if(tar>=0){
        igl::writeOBJ(filePath,mesh[tar].V,mesh[tar].F);
    }else{

    }
}
void IglMachine::writeFileOff(std::string name, char * filePath){
    int tar = searchTar(name, false);
    if(tar>=0){
        igl::writeOFF(filePath,mesh[tar].V,mesh[tar].F);
    }else{

    }
}

Eigen::MatrixXd IglMachine::getV(std::string name){
    int tar = searchTar(name);
    if(tar == -1){
        return mesh[dataNumLimit-1].V;//return empty
    }else{
        return mesh[tar].V;
    }
}
Eigen::MatrixXi IglMachine::getF(std::string name){
    int tar = searchTar(name);
    if(tar == -1){
        return mesh[dataNumLimit-1].F;//return empty
    }else{
        return mesh[tar].F;
    }
}

void IglMachine::remove(std::string name) {
	int tar = searchTar(name, false);
	if (tar != -1) {
		mesh[tar].setName("");
	}
}

void IglMachine::command(std::string line){

    std::string name[3];
    int tar[3];
    //float val[3];
    std::string instruction;
    std::stringstream scriptIfs(line);
    scriptIfs >> instruction;
    if(instruction == "NEW" || instruction == "new"){
        scriptIfs >> name[0];
        putNew(name[0]);
        if(debugText)std::cout<<"New " << name[0] <<std::endl;
    }else if(instruction == "LOAD" || instruction == "load"){
        scriptIfs >> name[0];
        std::vector<float> verts;
        std::vector<float> norms;
        std::vector<unsigned int> facs;
        char path[100];sprintf(path,"pool\\%s.obj",name[0].c_str());
        loadObj(path, verts, norms, facs);
        put(name[0], verts, facs);
		if (debugText)std::cout<<"LOAD " << name[0] <<std::endl;
    }else if(instruction == "COPY" || instruction == "copy"){
        scriptIfs >> name[0] >> name[1];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        put(name[0],mesh[tar[1]].V,mesh[tar[1]].F);
		if (debugText)std::cout<<"COPY " << name[0] << " " << name[1] <<std::endl;
    }else if(instruction == "WRITE" || instruction == "write"){
        scriptIfs >> name[0];
        if(searchTar(name[0])==-1){
            std::cout << "Warning : \"" << name << "\" not exist" << std::endl;
            return;
        }
        char path[100];sprintf(path,"pool\\%s.obj",name[0].c_str());
        writeFile(name[0] ,path);
		if (debugText)std::cout<<"WRITE " << name[0] <<std::endl;
    }
    else if(instruction == "+"){
        boolean_type = igl::MESH_BOOLEAN_TYPE_UNION;
        scriptIfs >> name[0] >> name[1] >> name[2];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        tar[2] = searchTar(name[2]);
        igl::copyleft::cgal::mesh_boolean(mesh[tar[1]].V,mesh[tar[1]].F,mesh[tar[2]].V,mesh[tar[2]].F,boolean_type,mesh[tar[0]].V,mesh[tar[0]].F);
		if (debugText)std::cout<<"CSG Union " << name[0] << " " << name[1] << " " << name[2] <<std::endl;
    }else if(instruction == "-"){
        boolean_type = igl::MESH_BOOLEAN_TYPE_MINUS;
        scriptIfs >> name[0] >> name[1] >> name[2];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        tar[2] = searchTar(name[2]);
        igl::copyleft::cgal::mesh_boolean(mesh[tar[1]].V,mesh[tar[1]].F,mesh[tar[2]].V,mesh[tar[2]].F,boolean_type,mesh[tar[0]].V,mesh[tar[0]].F);
		if (debugText)std::cout<<"CSG Minus " << name[0] << " " << name[1] << " " << name[2] <<std::endl;
    }else if(instruction == "*"){
        boolean_type = igl::MESH_BOOLEAN_TYPE_INTERSECT;
        scriptIfs >> name[0] >> name[1] >> name[2];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        tar[2] = searchTar(name[2]);
        igl::copyleft::cgal::mesh_boolean(mesh[tar[1]].V,mesh[tar[1]].F,mesh[tar[2]].V,mesh[tar[2]].F,boolean_type,mesh[tar[0]].V,mesh[tar[0]].F);
		if (debugText)std::cout<<"CSG Intersec " << name[0] << " " << name[1] << " " << name[2] <<std::endl;
    }
    else if(instruction == "ADD" || instruction == "add"){
        boolean_type = igl::MESH_BOOLEAN_TYPE_UNION;
        scriptIfs >> name[0] >> name[1];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        add(name[0],mesh[tar[1]].V,mesh[tar[1]].F);
		if (debugText)std::cout<<"CSG QuickUnion " << name[0] << " " << name[1] << " " << name[2] <<std::endl;
    }
    else if(instruction == "SCALE"){
        float x,y,z;
        scriptIfs >> name[0]>> name[1] >> x >> y >> z;
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        std::vector<float> vertices_new = mesh[tar[1]].vertices;
        for(int i=0;i<(int)vertices_new.size();i+=3){
            vertices_new[i]*=x;
            vertices_new[i+1]*=y;
            vertices_new[i+2]*=z;
        }
        put(name[0],vertices_new, mesh[tar[1]].indices);
		if (debugText)std::cout<<"SCALE " << name[0] << " "  << name[1] <<std::endl;
    }else if(instruction == "TRANS"){
        float x,y,z;
        scriptIfs >> name[0]>> name[1] >> x >> y >> z;
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        std::vector<float> vertices_new = mesh[tar[1]].vertices;
        for(int i=0;i<(int)vertices_new.size();i+=3){
            vertices_new[i]+=x;
            vertices_new[i+1]+=y;
            vertices_new[i+2]+=z;
        }
        put(name[0],vertices_new, mesh[tar[1]].indices);
		if (debugText)std::cout<<"TRANS " << name[0] << " "  << name[1] <<std::endl;
    }
}
void IglMachine::fetchTree(std::string name, int tarTree){
    Eigen::MatrixXi MF = M[tarTree].F();
    Eigen::MatrixXd MV = M[tarTree].cast_V<Eigen::MatrixXd>();
    put(name, MV, MF);
}
void IglMachine::reset(){
	mesh.swap(std::vector<IglMesh>());
	nameMap.swap(std::map<std::string, int>());
	emptySet.swap(std::set<int>());
    for(int i=0;i<treeNum;i++){
        M[i] = {};
    }
}
