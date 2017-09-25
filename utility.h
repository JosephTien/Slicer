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
	std::vector<Piece> pieces;
	std::vector<Group> groups;
	std::priority_queue<GroupLink> groupLink;
	std::map<int, int> groupIdxMap;
	int groupidcnt = 0;
	Vector3 ld, ru;
	void genRandomTest(int k);
    void genPieceGroupMesh(std::string filename);
	void calBound();
	void genPiece();
	void genPiece(std::string filename);
	void genPiece(std::string filename, bool output);
	void appendPiece(Group& group, Piece& piece);
	void MergeGroup(Group& group1, Group& group2);
	void initGroup();//also init groupLink
	void optimize();
	void outputPiece();
	void outputGroup();
	void outputKnife();
	float calWorth(Group& group1, Group& group2);
};

#endif // UTILITY_H
