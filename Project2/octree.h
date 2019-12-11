#pragma once

#include "vec.h"
#include "material.h"
#include "ray.h"
#include "definitions.h"

#include <vector>

typedef unsigned char byte;

constexpr int MAX_CHILDREN_PER_NODE = 10;
constexpr byte MASK_X = 0b0001;
constexpr byte MASK_Y = 0b0010;
constexpr byte MASK_Z = 0b0100;

/* 인접노드 구하기 - parent-parent-(교점이 내부에 존재할 때까지)-parent-child-child-(leaf일 때까지)-child */
class Octree {
public:
	class OctreeNode {
	private:
		vector<Face *> faceptrs;	// faces are passed by pointers for mem eff
		Vec3f lowest;				// lowX lowY lowZ coord
		Vec3f highest;				// highX highY highZ coord
		Vec3f dividing_center;		// child-dividing center
		OctreeNode *parent;			// parent pointer
		OctreeNode **children;		// 8 dynamic child pointers
		byte index;					// of which side it resides in its parent
									// 3 bits XYZ represents sign

	public:
		OctreeNode();
		OctreeNode(const vector<Face *> &_faceptrs, OctreeNode *_parent, byte _index);
		~OctreeNode();

		bool isLeaf() const;				// is this node leaf? (no children)
		bool isRoot() const;				// is this node root? (no parent)
		bool isEmpty() const;				// is this node empty?
		
		int getSize() const;						// get number of faces
		OctreeNode *getChild(byte idx) const;		// get a child
		bool nearestIntersect(const Ray &ray,		// nearest intersection for the ray
			Face &ret_face, float &ret_r) const;
		bool penetratedBy(const Ray &ray) const;	// does the ray pass through?
	};
private:
	OctreeNode *root;
	
public:
	Octree(Face **_faceptrs, int len);
	~Octree();

	// Traverse
	void showAll(OctreeNode *ptr = nullptr) const;
	bool getNearestIntersect(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const;
};