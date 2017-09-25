#ifndef IGLMACHING_H
#define IGLMACHING_H

#include <pch.h>
#include <glm/glm.hpp>
#include <igl/read_triangle_mesh.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <igl/copyleft/cgal/CSGTree.h>
#include <Eigen/Core>
#include <igl/writeOFF.h>

class IglMesh
{
public:
    IglMesh(){}
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    std::string name = "";
    std::vector<float> vertices;
    std::vector<int> indices;
    void setName(std::string name);
    void getData(std::vector<float> &vertices, std::vector<unsigned int> &indices);
    void setData(std::vector<float> vertices, std::vector<int> indices);
    void setData(std::vector<float> vertices, std::vector<unsigned int> indices);
    void setData(Eigen::MatrixXd V, Eigen::MatrixXi F);
    void addData(Eigen::MatrixXd V, Eigen::MatrixXi F);
    void set(std::string name,std::vector<float> vertices, std::vector<int> indices);
    void set(std::string name,std::vector<float> vertices, std::vector<unsigned int> indices);
    void set(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F);
    //todo : apply matrix
};

class IglMachine
{
public:
    IglMachine(){}
    void loadObj(const char* filename, std::vector<float> &verts, std::vector<float> &norms, std::vector<unsigned int> &facs);
    const static int dataNumLimit = 100000;
	std::vector<IglMesh> mesh;
	std::map<std::string, int> nameMap;
	std::set<int> emptySet;
    igl::MeshBooleanType boolean_type;
    const static int treeNum=10;
    igl::copyleft::cgal::CSGTree M[treeNum];
	bool debugText = false;
    //*****************************************
	int searchTar(std::string tarName);
    int searchTar(std::string tarName, bool create);
	void insertEmpty();
	void insert(std::string name, int tar);
	int searchEmpty();
    void putNew(std::string name);
    void put(std::string name,std::vector<float> vertices, std::vector<unsigned int> indices);
    void put(std::string name,std::vector<float> vertices, std::vector<int> indices);
    void put(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F);
    void add(std::string name, Eigen::MatrixXd V, Eigen::MatrixXi F);
    void get(std::string name,std::vector<float> &vertices, std::vector<unsigned int> &indices);
    void readFile(std::string name, const char * filePath);
    void writeFile(std::string name, char * filePath);
    void writeFileOff(std::string name, char * filePath);
    Eigen::MatrixXd getV(std::string name);
    Eigen::MatrixXi getF(std::string name);
	void remove(std::string name);
    void command(std::string line);
    void fetchTree(std::string name, int tarTree);
    void reset();
    //todo not exist handle
    //todo ex-handle
};

#endif // IGLMACHING_H
