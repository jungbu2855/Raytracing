#include "octree.h"
#include "definitions.h"

#include <queue>
#include <functional>

using namespace std; 

typedef Octree::OctreeNode Node;

struct less_Faceptr_x : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		float lx = (*(l->vertices[0]))[X] + (*(l->vertices[1]))[X] + (*(l->vertices[2]))[X];
		float rx = (*(r->vertices[0]))[X] + (*(r->vertices[1]))[X] + (*(r->vertices[2]))[X];
		return lx < rx;
	}
} less_Faceptr_x_obj;
struct less_Faceptr_y : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		float ly = (*(l->vertices[0]))[Y] + (*(l->vertices[1]))[Y] + (*(l->vertices[2]))[Y];
		float ry = (*(r->vertices[0]))[Y] + (*(r->vertices[1]))[Y] + (*(r->vertices[2]))[Y];
		return ly < ry;
	}
} less_Faceptr_y_obj;
struct less_Faceptr_z : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		float lz = (*(l->vertices[0]))[Z] + (*(l->vertices[1]))[Z] + (*(l->vertices[2]))[Z];
		float rz = (*(r->vertices[0]))[Z] + (*(r->vertices[1]))[Z] + (*(r->vertices[2]))[Z];
		return lz < rz;
	}
} less_Faceptr_z_obj;

// Helper function prototypes
static const Vec3f findLowest(const vector<Face *> &_fptrs);
static const Vec3f findHighest(const vector<Face *> &_fptrs);
static const Vec3f min(const Vec3f &l, const Vec3f &r);
static const Vec3f max(const Vec3f &l, const Vec3f &r);
static const Vec3f findDividingCenter(vector<Face *> facePtrs);
byte findChild(const Face &f, const Vec3f &cubeLow, const Vec3f &cubeMid, const Vec3f &cubeHigh);

Octree::Octree() {
	root = new OctreeNode();
}

Octree::~Octree() {
	if (root != nullptr)
		delete root;
}

void Octree::insertAll(Face **_faceptrs, int len) {
	vector<Face *> faceptrs;
	for (int i = 0; i < len; i++)
		faceptrs.push_back(_faceptrs[i]);
	root->push_faces(faceptrs);
}

Node::OctreeNode()
	: lowest(Vec3f(INFTY)), highest(Vec3f(-INFTY)), dividing_center(Vec3f(0.)),
	parent(nullptr), children(nullptr),	n_faceptrs(0), index(-1) {}

Node::OctreeNode(OctreeNode *_parent, byte _index)
	: lowest(Vec3f(INFTY)), highest(Vec3f(-INFTY)), dividing_center(Vec3f(0.)),
	parent(_parent), children(nullptr),	n_faceptrs(0), index(_index) {}

Node::~OctreeNode() {
	faceptrs.clear();
	if (children != nullptr) {
		for (int i = 0; i < 8; i++)
			delete children[i];
		delete children;
	}
}

bool Node::isLeaf() const {
	return children == nullptr;
}

bool Node::isRoot() const {
	return parent == nullptr;
}

bool Node::isEmpty() const {
	return n_faceptrs == 0;
}

int Node::getSize() const {
	return n_faceptrs;
}

void Node::push_faces(const vector<Face *> &_fptrs, const Vec3f &_lowest, const Vec3f &_highest) {
	// Load faces
	faceptrs = _fptrs;
	if (_lowest[X] == INFTY)
		lowest = min(lowest, findLowest(_fptrs));
	if (_highest[X] == INFTY)
		highest = max(highest, findHighest(_fptrs));
	n_faceptrs += _fptrs.size();
	
	// Terminating condition
	if (n_faceptrs < MAX_CHILDREN_PER_NODE)
		return;

	// Make children if not exists
	if (isLeaf()) {
		children = new Node *[8];
		for (int i = 0; i < 8; i++)
			children[i] = new Node(this, i);
		dividing_center = findDividingCenter(faceptrs);
	}

	// Trickle down the faces
	for (vector<Face *>::iterator it = faceptrs.begin(); it < faceptrs.begin() + faceptrs.size() / 8; ++it) {
		Face f = **it;
		
		// find the child who will have the face
		byte idx = findChild(f, lowest, dividing_center, highest);
	}
}

/* Helper functions */
static const Vec3f findLowest(const vector<Face *> &_fptrs) {
	Vec3f lowest(INFTY);
	for (int i = 0; i < _fptrs.size(); i++) {
		for (int j = 0; j < 3; j++) {
			Vec3f v = *(_fptrs[i]->vertices[j]);
			lowest = min(lowest, v);
		}
	}
	return lowest;
}

static const Vec3f findHighest(const vector<Face *> &_fptrs) {
	Vec3f highest(-INFTY);
	for (int i = 0; i < _fptrs.size(); i++) {
		for (int j = 0; j < 3; j++) {
			Vec3f v = *(_fptrs[i]->vertices[j]);
			highest = max(highest, v);
		}
	}
	return highest;
}

static const Vec3f min(const Vec3f &l, const Vec3f &r) {
	return Vec3f(
		l[X] < r[X] ? l[X] : r[X],
		l[Y] < r[Y] ? l[Y] : r[Y],
		l[Z] < r[Z] ? l[Z] : r[Z]
	);
}

static const Vec3f max(const Vec3f &l, const Vec3f &r) {
	return Vec3f(
		l[X] > r[X] ? l[X] : r[X],
		l[Y] > r[Y] ? l[Y] : r[Y],
		l[Z] > r[Z] ? l[Z] : r[Z]
	);
}

static const Vec3f findDividingCenter(vector<Face *> facePtrs) {
	Vec3f ret;
	sort(facePtrs.begin(), facePtrs.end(), less_Faceptr_x_obj);
	ret[X] = ((*facePtrs[facePtrs.size() / 2]->vertices[0])[X]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[1])[X]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[2])[X]) / 3;

	sort(facePtrs.begin(), facePtrs.end(), less_Faceptr_y_obj);
	ret[Y] = ((*facePtrs[facePtrs.size() / 2]->vertices[0])[Y]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[1])[Y]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[2])[Y]) / 3;

	sort(facePtrs.begin(), facePtrs.end(), less_Faceptr_z_obj);
	ret[Z] = ((*facePtrs[facePtrs.size() / 2]->vertices[0])[Z]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[1])[Z]
		+ (*facePtrs[facePtrs.size() / 2]->vertices[2])[Z]) / 3;

	return ret;
}

byte findChild(const Face &f, const Vec3f &cubeLow, const Vec3f &cubeMid, const Vec3f &cubeHigh) {
	byte ret = 0;
	
	// +z test
	for (int i = 0; i < 3; i++) {
		
	}
}