#include "octree.h"
#include "definitions.h"

#include <queue>
#include <functional>
#include <cassert>

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
static byte findChild(const Face &f, const Vec3f &cubeLow, const Vec3f &cubeMid, const Vec3f &cubeHigh);

Octree::Octree(Face ** _faceptrs, int len) {
	vector<Face *> __faceptrs;
	for (int i = 0; i < len; i++)
		__faceptrs.push_back(_faceptrs[i]);
	root = new OctreeNode(__faceptrs, nullptr, -1);
}

Octree::~Octree() {
	if (root != nullptr)
		delete root;
}

void Octree::showAll(Node *ptr) const {
	if (ptr == nullptr)
		ptr = root;

	if (!ptr->isLeaf()) {
		cout << ptr->getSize() << " ";
		for (int i = 0; i < 8; i++)
			showAll(ptr->getChild(i));
	}
}




Node::OctreeNode()
	: lowest(Vec3f(INFTY)), highest(Vec3f(-INFTY)), dividing_center(Vec3f(0.)),
	parent(nullptr), children(nullptr), index(-1) {}

Node::OctreeNode(const vector<Face *> &_faceptrs, OctreeNode *_parent, byte _index)
	: faceptrs(_faceptrs), parent(_parent), index(_index) {
	lowest = findLowest(faceptrs);
	highest = findHighest(faceptrs);
	children = nullptr;

	if (faceptrs.size() < MAX_CHILDREN_PER_NODE) {
		return;
	}

	Vec3f center = findDividingCenter(faceptrs);
	vector<Face *> myChildren[8]{};
	for (vector<Face *>::iterator it = faceptrs.begin(); it != faceptrs.end();) {
		byte idx = findChild(**it, lowest, center, highest);
		if (idx != (byte)-1) {
			myChildren[idx].push_back(*it);
			it = faceptrs.erase(it);
		}
		else
			++it;
	}

	if (faceptrs.size() != _faceptrs.size()) {
		dividing_center = center;
		children = new Node*[8];
		for (int i = 0; i < 8; i++) {
			children[i] = new Node(myChildren[i], this, i);
		}
	}
}

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
	return faceptrs.size() == 0;
}

int Node::getSize() const {
	return faceptrs.size();
}

Node *Node::getChild(byte idx) const {
	assert(!isLeaf());
	return children[idx];
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