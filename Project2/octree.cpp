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
	if (_lowest[X] == INFTY)
		lowest = min(lowest, findLowest(_fptrs));
	if (_highest[X] == -INFTY)
		highest = max(highest, findHighest(_fptrs));
	
	// Terminating condition
	if (n_faceptrs + _fptrs.size() < MAX_CHILDREN_PER_NODE) {
		// Load faces
		for (int i = 0; i < _fptrs.size(); ++i) {
			faceptrs.push_back(_fptrs[i]);
		}
		n_faceptrs += _fptrs.size();
		return;
	}

	// Make children if not exists
	if (isLeaf()) {
		children = new Node *[8];
		for (int i = 0; i < 8; i++)
			children[i] = new Node(this, i);
		dividing_center = findDividingCenter(_fptrs);
	}

	// Trickle down the faces
	vector<Face *> fptrsForChildren[8];
	for (vector<Face *>::const_iterator it = _fptrs.begin(); it != _fptrs.end(); ++it) {
		Face f = **it;
		
		// find the child who will have the face
		byte idx = findChild(f, lowest, dividing_center, highest);
		if (idx == (byte)(-1)) {
			faceptrs.push_back(&f);
			n_faceptrs++;
			continue;
		}
		fptrsForChildren[idx].push_back(&f);
	}
	
	// Recursive calls (8)
	for (int i = 0; i < 8; i++) {
		//if (fptrsForChildren[i].size() == 0)
		//	continue;
		Vec3f low, high;
		low[X] = (i & MASK_X) ? dividing_center[X] : lowest[X];
		high[X] = (i & MASK_X) ? highest[X] : dividing_center[X];
		low[Y] = (i & MASK_Y) ? dividing_center[Y] : lowest[Y];
		high[Y] = (i & MASK_Y) ? highest[Y] : dividing_center[Y];
		low[Z] = (i & MASK_Z) ? dividing_center[Z] : lowest[Z];
		high[Z] = (i & MASK_Z) ? highest[Z] : dividing_center[Z];

		children[i]->push_faces(fptrsForChildren[i], low, high);
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

static byte findChild(const Face &f, const Vec3f &cubeLow, const Vec3f &cubeMid, const Vec3f &cubeHigh) {
	byte xxyyzz = 0b111111; // +x -x +y -y +z -z

	for (int i = 0; i < 3; i++) {
		// +z
		if ((*f.vertices[i])[Z] > cubeMid[Z] && (*f.vertices[i])[Z] < cubeHigh[Z]) {
			xxyyzz = xxyyzz & 0b111110;
		}

		// -z
		if ((*f.vertices[i])[Z] < cubeMid[Z] && (*f.vertices[i])[Z] > cubeLow[Z]) {
			xxyyzz = xxyyzz & 0b111101;
		}

		// +y
		if ((*f.vertices[i])[Y] > cubeMid[Y] && (*f.vertices[i])[Y] < cubeHigh[Y]) {
			xxyyzz = xxyyzz & 0b111011;
		}

		// -y
		if ((*f.vertices[i])[Y] < cubeMid[Y] && (*f.vertices[i])[Y] > cubeLow[Y]) {
			xxyyzz = xxyyzz & 0b110111;
		}

		// +x
		if ((*f.vertices[i])[X] > cubeMid[X] && (*f.vertices[i])[X] < cubeHigh[X]) {
			xxyyzz = xxyyzz & 0b101111;
		}

		// -x
		if ((*f.vertices[i])[X] < cubeMid[X] && (*f.vertices[i])[X] > cubeLow[X]) {
			xxyyzz = xxyyzz & 0b011111;
		}
	}

	// -1 indicates the face must be stored in parent
	if (((xxyyzz & 0b110000) == 0b000000 || (xxyyzz & 0b110000) == 0b110000) ||
		((xxyyzz & 0b001100) == 0b000000 || (xxyyzz & 0b001100) == 0b001100) ||
		((xxyyzz & 0b000011) == 0b000000 || (xxyyzz & 0b000011) == 0b000011))
		return (byte)(-1);

	return ((xxyyzz & 0b100000) >> 3) | ((xxyyzz & 0b001000) >> 2) | ((xxyyzz & 0b000010) >> 1);
}