#pragma once

#include "vec.h"
#include "material.h"
#include "definitions.h"

#include <vector>

typedef unsigned char byte;

constexpr int MAX_CHILDREN_PER_NODE = 5;
constexpr byte MASK_X = 0b0001;
constexpr byte MASK_Y = 0b0010;
constexpr byte MASK_Z = 0b0100;

/* 인접노드 구하기 - parent-parent-(교점이 내부에 존재할 때까지)-parent-child-child-(leaf일 때까지)-child */
class Octree {
public:
	/* This cube includes boundary */
	class OctreeNode {
	private:
		vector<Face *> faceptrs;	// faces are passed by pointers for mem eff
		Vec3f lowest;				// lowX lowY lowZ coord
		Vec3f highest;				// highX highY highZ coord
		Vec3f dividing_center;		// child-dividing center
		OctreeNode *parent;			// parent pointer
		OctreeNode **children;		// 8 dynamic child pointers
		int n_faceptrs;				// number of faces in the node
		byte index;					// of which side it resides in its parent
									// 3 bits XYZ represents sign

	public:
		OctreeNode();
		OctreeNode(OctreeNode *_parent, byte _index);
		~OctreeNode();

		bool isLeaf() const;				// is this node leaf? (no children)
		bool isRoot() const;				// is this node root? (no parent)
		bool isEmpty() const;				// is this node empty?
		
		int getSize() const;		// get number of faces
		OctreeNode *left  () const;		// get left node
		OctreeNode *right () const;		// get right node
		OctreeNode *up    () const;		// get up node
		OctreeNode *down  () const;		// get down node
		OctreeNode *front () const;		// get front node
		OctreeNode *back  () const;		// get back node

		void push_faces(const vector<Face *> &_fptrs,
			const Vec3f &_lowest = Vec3f(INFTY),
			const Vec3f &_highest = Vec3f(-INFTY));
	};
private:
	OctreeNode *root;
	
public:
	Octree();
	~Octree();

	// Insert, remove
	void insertAll(Face **_faceptrs, int len);
	void remove(Face *_faceptr);

	// Booleans
	bool isEmpty() const;
	
	// Find
	const OctreeNode &find(const Face &face) const;

	// Traverse

};