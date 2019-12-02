#pragma once

#include "vec.h"
#include "material.h"
#include "definitions.h"

#include <vector>

typedef unsigned char byte;

/* 인접노드 구하기 - parent-parent-(교점이 내부에 존재할 때까지)-parent-child-child-(leaf일 때까지)-child */
class Octree {
public:
	class OctreeNode {
	private:
		vector<Face *> faceptrs;	// faces are passed by pointers for mem eff
		Vec3f lowest;				// lowX lowY lowZ coord
		Vec3f highest;				// highX highY highZ coord
		OctreeNode *parent;			// parent pointer
		OctreeNode **children;		// 8 dynamic child pointers
		byte index;					// of which side it resides in its parent

	public:
		bool isLeaf(const OctreeNode *node) const;	// is this node leaf? (no children)
		bool isRoot(const OctreeNode *node) const;	// is this node root? (no parent)

		OctreeNode *left  () const;		// get left node
		OctreeNode *right () const;		// get right node
		OctreeNode *up    () const;		// get up node
		OctreeNode *down  () const;		// get down node
		OctreeNode *front () const;		// get front node
		OctreeNode *back  () const;		// get back node
	};
private:
	OctreeNode *root;
	
public:
	Octree();
	~Octree();

	// Insert, remove
	void insertAll(const Face **_faceptrs, int len);
	void remove(const Face *_faceptr);

	// Booleans
	bool isEmpty() const;
	
	// Find
	const OctreeNode &find(const Face &face) const;

	// Traverse

};