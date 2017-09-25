#include <glm/glm.hpp>
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//#include <GLUT/glut.h>

#include "externals/include/glew/glew.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>

#include <mesh.h>
#include <iglmachine.h>

#include "tiny_obj_loader.h"
#define TINYOBJLOADER_IMPLEMENTATION

using namespace std;

void loadObjByTiny(const char* filename, std::vector<float> &verts, std::vector<float> &norms, std::vector<unsigned int> &facs){
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);

    if (!err.empty()) { // `err` may contain warning message.
      std::cerr << err << std::endl;
    }
    if (!ret) {
      exit(1);
    }
    verts.clear();norms.clear();facs.clear();
    for(int i=0;i<(int)attrib.vertices.size();i++){
        verts.push_back(attrib.vertices[i]);
    }
    for(int i=0;i<(int)attrib.normals.size();i++){
        norms.push_back(attrib.normals[i]);
    }
    for (size_t i = 0; i < shapes.size(); i++) {
        size_t indexOffset = 0;
        for (size_t n = 0; n < shapes[i].mesh.num_face_vertices.size(); n++) {
            int ngon = shapes[i].mesh.num_face_vertices[n];
            for (size_t f = 0; f < (size_t)ngon; f++) {
                unsigned int v = shapes[i].mesh.indices[indexOffset + f].vertex_index;
                facs.push_back(v);
            }
            indexOffset += ngon;
        }
    }
}
void Mesh::loadObj(const char* filename, std::vector<float> &verts, std::vector<float> &norms, std::vector<unsigned int> &facs)
{
    loadObjByTiny(filename, verts, norms, facs);
    return;
    vector<glm::vec4> vertices;
    vector<glm::vec3> normals;
    vector<GLushort> elements;
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    int hasvn = 0;

    string line;
    while(getline(in, line)){
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
            //vertices[vcnt++]=v;
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            string aa, bb, cc;
            std::vector<string> strs;
            s >> aa; s >> bb; s >> cc;
            GLushort a,b,c;
            boost::split(strs, aa, boost::is_any_of("/"));
            a = atoi(strs[0].c_str());
            boost::split(strs, bb, boost::is_any_of("/"));
            b = atoi(strs[0].c_str());
            boost::split(strs, cc, boost::is_any_of("/"));
            c = atoi(strs[0].c_str());
            a--; b--; c--;
            elements.push_back(a); elements.push_back(b); elements.push_back(c);
            //elements[fcnt++]=a;elements[fcnt++]=b;elements[fcnt++]=c;
        }
        else if (line.substr(0,3) == "vn ")
        {
            hasvn = 1;
            istringstream s(line.substr(3));
            glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
            normals.push_back(v);
            //normals[ncnt++]=v;
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

    if(!hasvn){
        normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
        for (int i = 0; i < elements.size(); i+=3)
        {
            GLushort ia = elements[i];
            GLushort ib = elements[i+1];
            GLushort ic = elements[i+2];
            glm::vec3 normal = glm::normalize(glm::cross(
                                                         glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
                                                         glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
            normals[ia] += normal;
            normals[ib] += normal;
            normals[ic] += normal;
        }
        for (int i = 0; i < normals.size(); i++){
            normals[i] = glm::normalize(normals[i]);
        }
    }

    verts.clear();
    norms.clear();
    facs.clear();
    for(int i=0;i<(int)vertices.size();i++){
        verts.push_back(vertices[i].x);
        verts.push_back(vertices[i].y);
        verts.push_back(vertices[i].z);
    }
    for(int i=0;i<(int)normals.size();i++){
        norms.push_back(normals[i].x);
        norms.push_back(normals[i].y);
        norms.push_back(normals[i].z);
    }
    for(int i=0;i<(int)elements.size();i++){
        facs.push_back((unsigned int)elements[i]);
    }

}
void Mesh::loadObj_rc(QFile * qfile, std::vector<float> &verts, std::vector<float> &norms, std::vector<unsigned int> &facs)
{
    vector<glm::vec4> vertices;
    vector<glm::vec3> normals, normals_new;
    vector<GLushort> elements;
    glm::vec3 zero3(0,0,0);
    if (!qfile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr << "Cannot open resources" << endl; exit(1);
    }

    int hasvn = 0;
    int hasvn_new = 0;
    string line;
    while (!qfile->atEnd())
    {
        line = qfile->readLine().toStdString();
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
            normals_new.push_back(zero3);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            string aa, bb, cc;
            std::vector<string> strs;
            s >> aa; s >> bb; s >> cc;
            GLushort a,b,c;
            boost::split(strs, aa, boost::is_any_of("//"));
            a = atoi(strs[0].c_str());
            if(!strs[1].empty()&&hasvn){
                hasvn_new = 1;
                GLushort an = atoi(strs[1].c_str());
                normals_new[a] = normals[an];
            }
            boost::split(strs, bb, boost::is_any_of("//"));
            b = atoi(strs[0].c_str());
            if(!strs[1].empty()&&hasvn){
                hasvn_new = 1;
                GLushort bn = atoi(strs[1].c_str());
                normals_new[b] = normals[bn];
            }
            boost::split(strs, cc, boost::is_any_of("//"));
            c = atoi(strs[0].c_str());
            if(!strs[1].empty()&&hasvn){
                hasvn_new = 1;
                GLushort cn = atoi(strs[1].c_str());
                normals_new[c] = normals[cn];
            }
            a--; b--; c--;
            elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        else if (line.substr(0,3) == "vn ")
        {
            hasvn = 1;
            istringstream s(line.substr(3));
            glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
            normals.push_back(v);
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }
    if(hasvn_new && hasvn){
        normals.clear();
        normals = normals_new;
    }
    if(!hasvn){
        normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
        for (int i = 0; i < (int)elements.size(); i+=3)
        {
            GLushort ia = elements[i];
            GLushort ib = elements[i+1];
            GLushort ic = elements[i+2];
            glm::vec3 normal = glm::normalize(glm::cross(
                                                         glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
                                                         glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
            normals[ia] += normal;
            normals[ib] += normal;
            normals[ic] += normal;
        }
        for (int i = 0; i < (int)normals.size(); i++){
            normals[i] = glm::normalize(normals[i]);
        }
    }

    verts.clear();
    norms.clear();
    facs.clear();
    for(int i=0;i<(int)vertices.size();i++){
        verts.push_back(vertices[i].x);
        verts.push_back(vertices[i].y);
        verts.push_back(vertices[i].z);
    }
    for(int i=0;i<(int)normals.size();i++){
        norms.push_back(normals[i].x);
        norms.push_back(normals[i].y);
        norms.push_back(normals[i].z);
    }
    for(int i=0;i<(int)elements.size();i++){
        facs.push_back((unsigned int)elements[i]);
    }
    qfile->close();
}
void IglMachine::loadObj(const char* filename, std::vector<float> &verts, std::vector<float> &norms, std::vector<unsigned int> &facs){
    loadObjByTiny(filename, verts, norms, facs);
}
