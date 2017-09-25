#include "cgalmachine.h"
#include "cgaltool.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef CGAL::Polyhedron_3<Kernel>                          Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel>                      Nef_polyhedron;
typedef Kernel::Point_3                                     Point_3;
typedef Kernel::Vector_3                                    Vector_3;
typedef Kernel::Aff_transformation_3                        Aff_transformation_3;
typedef Polyhedron::Halfedge_handle                         Halfedge_handle;
typedef Polyhedron::HalfedgeDS                              HalfedgeDS;
typedef CGAL::Delaunay_triangulation_3<Kernel>              Delaunay;
typedef Delaunay::Vertex_handle                             Vertex_handle;
typedef Polyhedron::Vertex_iterator                         Vertex_iterator;
typedef Polyhedron::Facet_iterator                          Facet_iterator;
//typedef Polyhedron::Halfedge_around_facet_circulator      Halfedge_around_facet_circulator;
typedef Polyhedron::Halfedge_around_facet_circulator        Halfedge_facet_circulator;

typedef Nef_polyhedron::Vertex_const_handle               Vertex_const_handle;
typedef Nef_polyhedron::Halfedge_const_handle             Halfedge_const_handle;
typedef Nef_polyhedron::Halffacet_const_handle            Halffacet_const_handle;
typedef Nef_polyhedron::Volume_const_handle               Volume_const_handle;
typedef Nef_polyhedron::Object_handle                     Object_handle;

void PolyStruct::setCenter(Point_3 p){
    center = p;
}
Point_3 PolyStruct::getCenter(){//better method of cgal?
    if(structstate==1)return center;
    Vector_3 vec( 0.0, 0.0, 0.0);
    std::size_t order = 0;
    for ( Vertex_iterator v = polyhedron.vertices_begin(); v != polyhedron.vertices_end(); ++v){
        vec = vec + ( v->point() - CGAL::ORIGIN);
        ++ order;
    }
    center =  CGAL::ORIGIN + (vec / static_cast<double>(order));
    structstate=1;
    return center;
}
void PolyStruct::makeNef(){
    std::string offstr;
    std::stringstream buffer;
    buffer << polyhedron;
    CGAL::OFF_to_nef_3(buffer, nef_polyhedron);
    flag=3;
    flag|=4;// non-mani P
}
int PolyStruct::makeDataExist(){ // exception ? illegal P to N ?
    if(flag == 0){// bit : 00
        std::cout << "      Error : No Data in \"" << name <<"\""<< std::endl;
        return 0;
    }
    else if(flag == 2){// bit : 10
        if(!nef_polyhedron.is_simple()){
            std::cout << "      Error : Not simple to convert \"" << name <<"\""<< std::endl;
            return 0;
        }
        nef_polyhedron.convert_to_polyhedron(polyhedron);
        flag = 3;/// bit : 11
        return 1;
    }
    else if(flag == 1){// bit : 01
        if(polyhedron.is_closed()){
            if(flag|4){
                makeNef();
            }else{
                Nef_polyhedron _nef_polyhedron(polyhedron);
                nef_polyhedron = _nef_polyhedron;
            }
            flag = 3;/// bit : 11
            return 1;
        }else {
            std::cout << "      Error : Not closed : \"" << name <<"\""<< std::endl;
            return 0;
        }

    }
    return 1;
}

CGALMachine::CGALMachine()
{

}
int CGALMachine::searchTar(std::string str){
    for(int i = 0 ; i < polyDataTail ; i++){
        if(polyData[i].name == str)return i;
    }
    std::cout << "      Error : Not Found \"" << str <<"\""<< std::endl;
    return -1;
}

int CGALMachine::checkDataExist(int tar){ // exception ? illegal P to N ?
    return polyData[tar].makeDataExist();
}
int CGALMachine::checkDataExist(int tar, int flag){
    if(polyData[tar].flag==flag)return 1;
    return polyData[tar].makeDataExist();
}
Polyhedron CGALMachine::genePoly(std::vector<float> coords, std::vector<int> tris){
    Polyhedron P;
    Polyhedron_builder<HalfedgeDS> builder( coords, tris );
    P.delegate(builder);
    return P;
}

Polyhedron CGALMachine::convexHull(Polyhedron P){
    std::list<Point_3> points;
    for ( Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v)
        points.push_back(v->point());
    Delaunay T;
    T.insert(points.begin(), points.end());
    std::list<Vertex_handle> vertices;
    T.incident_vertices(T.infinite_vertex(), std::back_inserter(vertices));
    Polyhedron chull;
    CGAL::convex_hull_3_to_face_graph(T,chull);
    return chull;
}
Polyhedron CGALMachine::vectorScale(Polyhedron P, double thick){
    if(thick==0)return P;
    //fetch vertices and initialize vertices' normals
    std::vector<Point_3> vertices;
    std::vector<Vector_3> vnormals;
    std::vector<int> vnnum;
    for ( Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v){
        vertices.push_back(v->point());
        vnormals.push_back(Vector_3(0,0,0));
        vnnum.push_back(0);
    }

    std::vector< Vector_3 > normals;
    std::vector< int > facets;
    for ( Facet_iterator f = P.facets_begin(); f != P.facets_end(); ++f)
    {
        //get facet normal
        Polyhedron::Facet::Halfedge_handle h = f->halfedge();
        Vector_3 normal = CGAL::cross_product(
                h->next()->vertex()->point() - h->vertex()->point(),
                h->next()->next()->vertex()->point() - h->next()->vertex()->point());
        normals.push_back(normal);//not must

        //get facet index and add
        std::vector<int> facet;
        Halfedge_facet_circulator j = f->facet_begin();
        CGAL_assertion( CGAL::circulator_size(j) >= 3);
        do {
            int idx = std::distance(P.vertices_begin(), j->vertex());
            Kernel::FT dis = Kernel::FT(sqrt(CGAL::to_double(normal.squared_length())));
            vnormals[idx] = vnormals[idx] + (normal) / dis;//current method is to equalize the movement of each facet
            vnnum[idx]++;
            facets.push_back(idx);
        } while ( ++j != f->facet_begin());
        //std::cout << normal.x() << " " << normal.y() << " " << normal.z() << "\n";
    }
    for (int i=0 ; i<(int)vnnum.size(); i++){
        vertices[i] = vertices[i] + (vnormals[i] / vnnum[i] * thick);
    }
    std::vector<float> coords;
    for(int i = 0;i < (int)vertices.size();i++){
        coords.push_back( (float)CGAL::to_double(vertices[i].x()) );//hx?
        coords.push_back( (float)CGAL::to_double(vertices[i].y()) );//hy?
        coords.push_back( (float)CGAL::to_double(vertices[i].z()) );//hz?
    };

    Polyhedron _P;
    Polyhedron_builder<HalfedgeDS> builder( coords, facets );
    _P.delegate(builder);
    return _P;

}
void CGALMachine::machine(std::string line, std::vector<float> coords, std::vector<unsigned int> tris){
    std::vector<int> tris_;
    for(int i=0;i<tris.size();i++){
        tris_.push_back((int)tris[i]);
    }
    machine(line, coords, tris_);
}

void CGALMachine::machine(std::string line, std::vector<float> coords, std::vector<int> tris){
    std::string instruction;
    char instruction_c;
    std::stringstream scriptIfs(line);
    scriptIfs >> instruction;
    instruction_c = instruction.c_str()[0];
    std::cout << "instruction" << cnt++ << "..." << std::endl;

    if(instruction == "LOAD"){
        if(polyDataTail>=polyDataSize){
            std::cout << "      Error : No more space!!!"<< std::endl;
            return;
        }
        scriptIfs >> polyData[polyDataTail].name;
        polyData[polyDataTail].polyhedron = genePoly(coords, tris);
        polyData[polyDataTail].flag = 1;
        checkDataExist(polyDataTail, 1);//use to convert P to N, assume legal obj
        std::cout << "      Load \""<<  polyData[polyDataTail].filename.c_str()
                                    <<"\" to \""<< polyData[polyDataTail].name << "\""<< std::endl;
        polyDataTail++;
    }else if(instruction == "LOADNEF"){
        if(polyDataTail>=polyDataSize){
            std::cout << "      Error : No more space!!!"<< std::endl;
            return;
        }
        scriptIfs >> polyData[polyDataTail].name;
        polyData[polyDataTail].polyhedron = genePoly(coords, tris);
        polyData[polyDataTail].flag = 1;
        polyData[polyDataTail].makeNef();
        std::cout << "      Load \""<<  polyData[polyDataTail].filename.c_str()
                                    <<"\" to \""<< polyData[polyDataTail].name << "\" (directly gen nef)"<< std::endl;
        polyDataTail++;
    }
}
void CGALMachine::machine(std::string line){
    std::string name[3];
    int tar[3];
    float val;
    std::string instruction;
    char instruction_c;
    std::stringstream scriptIfs(line);
    scriptIfs >> instruction;
    instruction_c = instruction.c_str()[0];
//    if(instruction == "HALT") {
//        std::cout << "\nFinishing!!"<< std::endl;
//        return;
//    }
//    if(instruction_c == '#' || instruction_c == '\n'){
//        scriptIfs.getline(trash, 256);
//        return;
//    }
    std::cout << "instruction" << cnt++ << "..." << std::endl;

    if(instruction == "SMPLF"){
        scriptIfs >> name[0] >> name[1];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        if(tar[0] < 0 )return;
        if(tar[1] < 0 )return;
        if(!checkDataExist(tar[1], 1))return;
        polyData[tar[0]].polyhedron = cgaltool.simplify(polyData[tar[1]].polyhedron);
        polyData[tar[0]].flag = 1;
        std::cout << "      Simplify \"" << name[1] << "\" -> \"" << name[0] <<"\""<< std::endl;
    }else if(instruction == "NEW"){
        if(polyDataTail>=polyDataSize){
            std::cout << "      Error : No more space!!!"<< std::endl;
            return;
        }
        scriptIfs >> polyData[polyDataTail].name;
        //scriptIfs >> polyData[polyDataTail].filename;
        polyData[polyDataTail].flag = 0; // bit : 00
        std::cout << "      New : \"" << polyData[polyDataTail].name <<"\""<< std::endl;
        polyDataTail++;
    }else if(instruction == "CH"){//Convex Hull
        scriptIfs >> name[0] >> name[1];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        if(tar[0] < 0 )return;
        if(tar[1] < 0 )return;
        if(!checkDataExist(tar[1], 1))return;
        polyData[tar[0]].polyhedron = convexHull(polyData[tar[1]].polyhedron);
        polyData[tar[0]].flag = 1;
        std::cout << "      Convex Hull \"" << name[1] << "\" -> \"" << name[0] <<"\""<< std::endl;
    }else if(instruction == "Align"){//Convex Hull
        scriptIfs >> name[0] >> name[1] >> name[2];
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        tar[2] = searchTar(name[2]);
        if(tar[0] < 0 )return;
        if(tar[1] < 0 )return;
        if(tar[2] < 0 )return;
        if(!checkDataExist(tar[1], 1))return;
        if(!checkDataExist(tar[2], 1))return;
        Aff_transformation_3 transl(CGAL::TRANSLATION, polyData[tar[2]].getCenter() - polyData[tar[1]].getCenter());
        polyData[tar[0]].polyhedron = polyData[tar[1]].polyhedron;
        std::transform( polyData[tar[0]].polyhedron.points_begin(), polyData[tar[0]].polyhedron.points_end(),
                        polyData[tar[0]].polyhedron.points_begin(), transl);
        polyData[tar[0]].flag = 1;
        std::cout << "      Align Center : \"" << name[1] << "\" -> \"" << name[2] << "\" => \"" << name[0] <<"\""<< std::endl;
    }else if(instruction == "VS"){//scale depend on vector
        scriptIfs >> name[0] >> name[1] >> val;
        tar[0] = searchTar(name[0]);
        tar[1] = searchTar(name[1]);
        if(tar[0] < 0 )return;
        if(tar[1] < 0 )return;
        if(!checkDataExist(tar[1], 1))return;
        polyData[tar[0]].polyhedron = vectorScale(polyData[tar[1]].polyhedron, val);
        polyData[tar[0]].flag = 1;
        std::cout << "      Scale " << val << " depend on vector : \"" << name[1] << "\" -> \"" << name[0] <<"\""<< std::endl;
    }else if(instruction == "TRANS"){//scale depend on vector
        float tx, ty, tz;
        scriptIfs >> name[0] >> tx >> ty >> tz;
        tar[0] = searchTar(name[0]);
        if(tar[0] < 0 )return;
        if(!checkDataExist(tar[0], 2))return;
        Aff_transformation_3 transl(CGAL::TRANSLATION, Vector_3(tx, ty, tz));
        polyData[tar[0]].nef_polyhedron.transform(transl);
        polyData[tar[0]].flag = 2;
        std::cout << "      Trans " << "\"" << name[0] <<"\""<< " : " << tx << " " << ty << " " << tz <<std::endl;
    }else if(instruction == "SCALE"){//scale depend on vector
        float tx, ty, tz;
        scriptIfs >> name[0] >> tx >> ty >> tz;
        tar[0] = searchTar(name[0]);
        if(tar[0] < 0 )return;
        if(!checkDataExist(tar[0], 2))return;
        Aff_transformation_3 scale(tx,0,0,
             0,ty,0,
             0,0,tz,
             1);
        polyData[tar[0]].nef_polyhedron.transform(scale);
        polyData[tar[0]].flag = 2;
        std::cout << "      Scale " << "\"" << name[0] <<"\""<< " : " << tx << " " << ty << " " << tz <<std::endl;
    }else if(instruction == "WRITE"){
        scriptIfs >> name[0];
        tar[0] = searchTar(name[0]);
        if(tar[0] < 0 )return;
        scriptIfs >> polyData[tar[0]].filename ;
        if(!checkDataExist(tar[0], 1))return;
        std::ofstream ofs;
        ofs.open(polyData[tar[0]].filename);
        CGAL::print_polyhedron_wavefront(ofs, polyData[tar[0]].polyhedron);
        std::cout << "      Write \"" << name[0]
                                      <<"\" to \""<< polyData[tar[0]].filename << "\""<< std::endl;
    }else if(instruction_c == '+' || instruction_c == '-' || instruction_c == '*' || instruction_c == '^'){
            scriptIfs >> name[0] >> name[1] >> name[2];
            int tar[3],valid = 1;
            for(int i=0;i<3;i++){
                tar[i] = searchTar(name[i]);
                if(tar[i] < 0 )valid = 0;
                if(i > 0)checkDataExist(tar[i], 2);
            }
            if(!(polyData[tar[1]].flag & 2))valid = 0;
            if(!(polyData[tar[2]].flag & 2))valid = 0;
            if(!valid)return;
            //havn't handle illegal case
            if(instruction_c == '+'){
                polyData[tar[0]].nef_polyhedron = polyData[tar[1]].nef_polyhedron + polyData[tar[2]].nef_polyhedron;
            }else if(instruction_c == '-'){
                polyData[tar[0]].nef_polyhedron = polyData[tar[1]].nef_polyhedron - polyData[tar[2]].nef_polyhedron;
            }else if(instruction_c == '*'){
                polyData[tar[0]].nef_polyhedron = polyData[tar[1]].nef_polyhedron * polyData[tar[2]].nef_polyhedron;
            }else if(instruction_c == '^'){
                polyData[tar[0]].nef_polyhedron = polyData[tar[1]].nef_polyhedron ^ polyData[tar[2]].nef_polyhedron;
            }
            polyData[tar[0]].flag = 2;//bit : 10;
            std::cout << "      "<<name[0] << " = " << name[1] << " " << instruction_c << " " << name[2] << std::endl;
    }


}

bool CGALMachine::checkInside(std::string out, std::string in){
    std::string name[2];
    name[0] = out;
    name[1] = in;
    int tar[2],valid = 1;

    tar[0] = searchTar(name[0]);
    if(tar[0] < 0 )valid = 0;
    checkDataExist(tar[0], 2);

    tar[1] = searchTar(name[1]);
    if(tar[1] < 0 )valid = 0;
    checkDataExist(tar[1], 2);

    if(!(polyData[tar[0]].flag & 1))valid = 0;
    if(!(polyData[tar[1]].flag & 2))valid = 0;
    if(!valid)return false;

    return (polyData[tar[1]].nef_polyhedron-polyData[tar[0]].nef_polyhedron).is_empty();
}

//void backup{
//    Nef_polyhedron N = polyData[tar[0]].nef_polyhedron;
//    Vertex_const_handle v;
//    Halfedge_const_handle e;
//    Halffacet_const_handle f;
//    Volume_const_handle c;
//    Object_handle o = N.locate(Point_3(0,0,0));

//    if(CGAL::assign(c,o))
//        std::cout << "Locating volume" << std::endl;
//    else if(CGAL::assign(e,o))
//        std::cout << "Locating edge" << std::endl;
//    else if(CGAL::assign(f,o))
//        std::cout << "Locating facet" << std::endl;
//    else if(CGAL::assign(v,o))
//        std::cout << "Locating vertex" << std::endl;

//    bool isFine = true;
//    Polyhedron P = polyData[tar[1]].polyhedron;
//    Nef_polyhedron N = polyData[tar[0]].nef_polyhedron;
//    for ( Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v){
//        Object_handle o = N.locate(v->point());
//        Volume_const_handle c;
//        std::cout << v->point().x() << std::endl;
//        std::cout << CGAL::assign(c,o) << std::endl;
//        if(!CGAL::assign(c,o)){
//            isFine = false;
//            break;
//        }
//    }
//}

//continue and break : change to return
//same name
