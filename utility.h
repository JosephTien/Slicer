#ifndef UTILITY_H
#define UTILITY_H

#include "pch.h"
#include "iglmachine.h"
#include "cgalnef.h"
#include "topo.h"

class Utility
{
public:
    Utility(){}
	CGALNef cgalnef;
	IglMachine iglMachine;
	Topo topo;
	std::vector<Voxel> voxels;
	std::vector<Piece> pieces;
	std::vector<Group> groups;
	std::priority_queue<GroupLink> groupLink;
	std::vector<int> groupIdxMap;//std::map<int, int> groupIdxMap;
	std::vector<bool> idexist;
	int nx, ny, nz;
	int groupidcnt = 0;
	Vector3 ld, ru;
	const float l = 2.0f;
	/*special case mode*/
	bool caseplane = false;
	int injnum = 10;
	/**/
	void genRandomTest(int k);
    void genPieceGroupMesh(std::string filename);
	void calBound();
	void genVoxel();
	void genVoxelByKnife();
	void genPiece_voxel();
	void genPiece();
	void genPiece(std::string filename);
	void genPiece(std::string filename, bool output);
	void genPiece_slice();
	void csgShape();
	void moldMold(int shapenum);
	void appendPiece(Group& group, Piece& piece);
	void MergeGroup(Group& group1, Group& group2);
	void initGroup();
	void initLink();
	void initLink_voxel();
	void optimize();
	void voxelBfs();
	void outputPiece();
	void outputPiece_voxel();
	void outputGroup();
	void outputGroup_voxel();
	void outputKnife();
	float calWorth(Group& group1, Group& group2);
};

#endif // UTILITY_H
