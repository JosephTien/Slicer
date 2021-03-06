#include "pch.h"
#include "utility.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
    Utility utility;
	srand((unsigned)time(NULL));
    if(argc>1){
		if (strcmp(argv[1], "slice") == 0) {
			utility.genPiece_slice();
			utility.topo.read();
			utility.csgShape();
			utility.outputGroup();
		}else if (strcmp(argv[1], "mold") == 0) {
			int shapenum;
			sscanf(argv[2], "%d", &shapenum);
			utility.moldMold(shapenum);
		}
		else if (strcmp(argv[1], "tune")==0) {
			utility.topo.read();
			utility.topo.geneOpt();
			//utility.topo.atomOpt();
			//utility.topo.beeOpt();
			utility.topo.outputRotateArg();
		}
		else if (strcmp(argv[1], "less") == 0) {
			utility.topo.read();
			utility.topo.geneLess();
			//utility.topo.optMode = 1;	
			//utility.topo.geneOpt(0, 180, 0.01);
			//utility.topo.atomOpt();
			//utility.topo.beeOpt();
			utility.topo.outputRotateArg();
		}
		else if (strcmp(argv[1], "bfs") == 0) {
			utility.topo.read();
			utility.calBound();
			utility.genVoxel();
			utility.voxelBfs();
			utility.outputPiece_voxel();
			utility.outputKnife();
			std::cout << "Done" << std::endl;
		}
		else if (strcmp(argv[1], "voxel") == 0) {
			utility.topo.read();
			utility.calBound();
			utility.topo.genKnife();
			utility.outputKnife();
			utility.genPiece_voxel();
			std::cout << "Piece Generated!" << std::endl;
			utility.initGroup();
			std::cout << "Group Initialized!" << std::endl;
			utility.initLink_voxel();
			std::cout << "Link Initialized!" << std::endl;
			utility.optimize();
			std::cout << "Optimized!" << std::endl;
			utility.outputGroup_voxel();
		}
		else {
			//utility.genPieceGroupMesh(argv[1]);
			utility.genPiece(argv[1]);
		}
	}
	else {
		//utility.genRandomTest(20);
		//utility.genPiece("randomTest.txt", true);
		utility.topo.read();
		utility.topo.genKnife();
		utility.outputKnife();
		utility.calBound();
		utility.genPiece();
		//utility.genPiece_voxel();
		//utility.outputPiece();
		std::cout << "Piece Generated!" << std::endl;
		utility.initGroup();
		std::cout << "Group Initialized!" << std::endl;
		utility.initLink();
		std::cout << "Link Initialized!" << std::endl;
		utility.optimize();
		std::cout << "Optimized!" << std::endl;
		utility.outputGroup();
		//utility.outputGroup_voxel();
	}
	char c = scanf("%c", &c);
}

//*******************************************************************************
/*BKUP*/
/*
typedef CGAL::Exact_integer  NT;
typedef CGAL::Homogeneous<NT>  Kernel;
typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Kernel::Vector_3  Vector_3;
typedef Kernel::Aff_transformation_3  Aff_transformation_3;
typedef Kernel::RT RT;

void printMat(Matrix4 mat4){
    printf("\n");
    printf("%.2f %.2f %.2f %.2f \n",mat4[0],mat4[1],mat4[2],mat4[3]);
    printf("%.2f %.2f %.2f %.2f \n",mat4[4],mat4[5],mat4[6],mat4[7]);
    printf("%.2f %.2f %.2f %.2f \n",mat4[8],mat4[9],mat4[10],mat4[11]);
    printf("%.2f %.2f %.2f %.2f \n\n",mat4[12],mat4[13],mat4[14],mat4[15]);
}
Nef_polyhedron transform(Nef_polyhedron N, Matrix4 mat4){
    printMat(mat4);
    N.transform(Aff_transformation_3(mat4[0],mat4[1],mat4[2],mat4[3],
                                    mat4[4],mat4[5],mat4[6],mat4[7],
                                    mat4[8],mat4[9],mat4[10],mat4[11],
                                    1));
    return N;
}

float roundFlt(float in){
    float rndVal = 10000;
    if(in>0)return ((float)((int)(in*rndVal+0.5f)))/rndVal;
    else return ((float)((int)(in*rndVal-0.5f)))/rndVal;
}
Matrix4 roundMat(Matrix4 mat4){
    for(int i=0;i<16;i++){
        mat4[i] = roundFlt(mat4[i]);
    }
    return mat4;
}
Nef_polyhedron transform(Nef_polyhedron N,Vector3 s,Vector3 t,Vector3 u, Vector3 f){
    u = u.normalize();
    f = u.normalize();
    float radtodeg = 180.0 / 3.14159265;
    Matrix4 mat4;
    mat4.identity();
    mat4.scale(s.x, s.y, s.z);
    mat4.translate(t.x, t.y, t.z);
    //*******************************
    Matrix4 rot1, rot2;
    rot1.identity() ;rot2.identity();
    Vector3 up = Vector3(0,0,1);
    Vector3 forw = Vector3(0,1,0);
    //*******************************
    if(abs(up.dot(u)) < 0.9999){
        rot1.rotate(acos(up.dot(u)) * radtodeg, up.cross(u).normalize());
        up = rot1 * up;
        forw = rot1 * forw;
    }else if(abs(up.dot(u)) < -0.9999){
        std::cout << "180deg error" << std::endl;
    }
    //*******************************
    if(abs(forw.dot(f)) < 0.9999){
        rot2.rotate(acos(forw.dot(f)) * radtodeg, forw.cross(f).normalize());//becaruful of the 180 deg
        forw = rot2 * forw;
    }else if(forw.dot(f) < -0.9999){
        std::cout << "180deg error" << std::endl;
    }
    //*******************************
    mat4.identity();
    mat4.scale(s.x, s.y, s.z);
    mat4.translate(t.x, t.y, t.z);

    Matrix4 form = rot2 * rot1 * mat4;
    form = roundMat(form);
    N = transform(N, form);
    return N;
}

int test(){
    std::ifstream in_sphere("sphere.off");
    std::ifstream in_cylinder("cylinder.off");
    std::ofstream out("out.off");
    Polyhedron P_sphere, P_cylinder, P_final;
    in_sphere >> P_sphere;
    in_cylinder >> P_cylinder;
    Nef_polyhedron N_sphere(P_sphere);
    Nef_polyhedron N_cylinder(P_sphere);
    Nef_polyhedron N_final, N_new;
    //*************************************************************
    std::cout << "start...\n" << std::endl;
    N_new = N_final = N_sphere;
    for(int i=0;i<10;i++){
        N_new = transform(N_new, Vector3(1,1,1), Vector3(50,0,0), Vector3(0,0,1), Vector3(0,1,0));
        N_final += N_new;
        std::cout << "cal " << i << "\n" << std::endl;
    }
    //*************************************************************
    if(N_final.is_simple()) {
        N_final.convert_to_polyhedron(P_final);
        out << P_final;
    }
    else {
        out << N_final;
    }
    std::cout << "done!\n" << std::endl;
}
*/
