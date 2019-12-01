#pragma once

#include "vec.h"
#include "material.h"
#include "definitions.h"

typedef unsigned char byte;

/* !!Traversal - parent-parent-(교점이 내부에 존재할 때까지)-parent-child-child-(leaf일 때까지)-child */
class Octree {
public:
	class OctreeNode {
	private:
		Face **faceptrs;
		Vec3f lowest;
		Vec3f highest;
		OctreeNode **children;
		byte index;

	public:
		bool isLeaf(const OctreeNode *node) const;
		bool isRoot(const OctreeNode *node) const;
	};
private:
	OctreeNode *root;
	
public:
	Octree();
	~Octree();

	// Insert, remove
	void insertAll(const Face **_faceptrs);
	void remove(const Face *_faceptr);

	// Booleans
	bool isEmpty() const;
	
	// Find
	const OctreeNode &find(const Face &face) const;

	// Traverse

};