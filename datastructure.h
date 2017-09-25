#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "pch.h"
#include "mesh.h"

class GroupLink {
public:
	GroupLink(int ida, int idb, float worth, float force) {
		if (ida > idb) {
			this->ida = idb;
			this->idb = ida;
		}
		else {
			this->ida = ida;
			this->idb = idb;
		}
		this->worth = worth;
		this->force = force;
	}
	int ida, idb;
	float worth;
	float force;
	bool operator < (const GroupLink& tar) const
	{
		return worth < tar.worth;
	}
	bool operator == (const GroupLink& tar) const
	{
		return worth == tar.worth;
	}
	bool operator > (const GroupLink& tar) const
	{
		return worth > tar.worth;
	}
};

class TouchInfo{
public:
	TouchInfo() {}
	TouchInfo(int e, Vector3 va, Vector3 vb, Vector3 dir, int knifeid) {
		this->e = e;
		this->va = va;
		this->vb = vb;
		this->dir = dir;
		this->knifeid = knifeid;
	}
	int e, knifeid;
	Vector3 va, vb, dir;
};

class Hash {
public:
	int hash = 0;
	int hashlen = 0;
	Hash() {
		hashlen = 0;
		hash = 0;
	}
	Hash(int hash, int hashlen) {
		this->hashlen = hashlen;
		this->hash = hash;
	}
	void addHash(bool positive) {
		if (positive) {
			int mask = 0x1 << hashlen;
			hash = hash | mask;
		}
		hashlen++;
	}
	int difNum(Hash tar) {
		unsigned int hashiter = hash ^ tar.hash;
		int cnt = 0;
		for (int i = 0; i < hashlen; i++, hashiter /= 2) {
			if (hashiter % 2 == 0x1)cnt++;
		}
		return cnt;
	}
	std::vector<bool> getBits() {
		std::vector<bool> bits;
		unsigned int hashiter = hash;
		for (int i = 0; i < hashlen; i++, hashiter /= 2) {
			if (hashiter % 2 == 0x1)bits.push_back(true);
			else bits.push_back(false);
		}
		return bits;
	}
	/*
	Hash() {
	hashlen = 0;
	hashes = std::vector<unsigned int>();
	}
	Hash(std::vector<unsigned int> hashes, int hashlen) {
	this->hashes = hashes;
	this->hashlen = hashlen;
	}
	int hashlen = 0;
	std::vector<unsigned int> hashes;
	void addHash(bool positive) {
	int hashpos = hashlen % 32;
	int hashgroup = hashlen / 32;
	if (hashpos == 0) {
	hashes.push_back(0);
	}
	if (positive) {
	int mask = 0x1 << hashpos;
	hashes[hashgroup] = hashes[hashgroup] | mask;
	}
	hashlen++;
	}
	*/
};

class Piece
{
public:
	Piece() {};
	Piece(Mesh mesh) {
		this->mesh = mesh;
	};
	Piece(Mesh mesh, Hash hash) {
		this->mesh = mesh;
		this->hash = hash;
	};
	Mesh mesh;
	Hash hash;
	std::vector<TouchInfo> touchinfos;
	bool exist = false;
	bool isNei(Piece tar) {
		return hash.difNum(tar.hash) == 1;
	}
};

class Group
{
public:
	Group() {};
	std::vector<Piece> pieces;
	std::vector<Vector3> ava;
	void initAva();
	const float plus = 15.0f;
	int id;
	std::set<int> neighbor;//id
	Mesh getMesh();
};

#endif // DATASTRUCTURE_H