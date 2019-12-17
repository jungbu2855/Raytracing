#include "octree.h"
#include "definitions.h"

#include <queue>
#include <functional>
#include <cassert>

using namespace std; 

typedef Octree::OctreeNode Node;

struct less_Faceptr_x : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		double lx = (*(l->vertices[0]))[X] + (*(l->vertices[1]))[X] + (*(l->vertices[2]))[X];
		double rx = (*(r->vertices[0]))[X] + (*(r->vertices[1]))[X] + (*(r->vertices[2]))[X];
		return lx < rx;
	}
} less_Faceptr_x_obj;
struct less_Faceptr_y : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		double ly = (*(l->vertices[0]))[Y] + (*(l->vertices[1]))[Y] + (*(l->vertices[2]))[Y];
		double ry = (*(r->vertices[0]))[Y] + (*(r->vertices[1]))[Y] + (*(r->vertices[2]))[Y];
		return ly < ry;
	}
} less_Faceptr_y_obj;
struct less_Faceptr_z : less<Face *> {
	bool operator() (const Face *l, const Face *r) const {
		double lz = (*(l->vertices[0]))[Z] + (*(l->vertices[1]))[Z] + (*(l->vertices[2]))[Z];
		double rz = (*(r->vertices[0]))[Z] + (*(r->vertices[1]))[Z] + (*(r->vertices[2]))[Z];
		return lz < rz;
	}
} less_Faceptr_z_obj;

// Helper function prototypes
static const Vec3d findLowest(const vector<Face *> &_fptrs);
static const Vec3d findHighest(const vector<Face *> &_fptrs);
static const Vec3d min(const Vec3d &l, const Vec3d &r);
static const Vec3d max(const Vec3d &l, const Vec3d &r);
static const Vec3d findDividingCenter(vector<Face *> facePtrs);
static byte findChild(const Face &f, const Vec3d &cubeLow, const Vec3d &cubeMid, const Vec3d &cubeHigh);
static double intersect_face(const Ray &ray, const Face &face);

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
	static int size = 0;
	
	if (!ptr->isLeaf()) {
		for (int i = 0; i < 8; i++)
			showAll(ptr->getChild(i));
	}
}

bool Octree::getNearestIntersect(const Ray &ray, Face &ret_face, Vec3d &ret_vec) const {
	if (!root->penetratedBy(ray))
		return false;
	double r;
	if (root->nearestIntersect(ray, ret_face, r)) {
		ret_vec = ray.getOrigin() + r * ray.getDirection();
		return true;
	}
	else
		return false;
}



Node::OctreeNode()
	: lowest(Vec3d(INFTY)), highest(Vec3d(-INFTY)), dividing_center(Vec3d(0.)),
	parent(nullptr), children(nullptr), index(-1) {}

Node::OctreeNode(const vector<Face *> &_faceptrs, OctreeNode *_parent, byte _index)
	: faceptrs(_faceptrs), parent(_parent), index(_index) {
	lowest = findLowest(faceptrs);
	highest = findHighest(faceptrs);
	children = nullptr;

	if (faceptrs.size() < MAX_CHILDREN_PER_NODE) {
		return;
	}

	Vec3d center = findDividingCenter(faceptrs);
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

bool Node::nearestIntersect(const Ray &ray, Face &ret_face, double &ret_r) const {
	Face candidate_f;
	double min_r = INFTY;
	for (vector<Face *>::const_iterator it = faceptrs.begin(); it != faceptrs.end(); ++it) {
		double candidate_r = intersect_face(ray, **it);
		if (candidate_r != -1 && candidate_r < min_r) {
			candidate_f = **it;
			min_r = candidate_r;
		}
	}

	if (isLeaf()) {
		if (min_r != INFTY) {
			ret_face = candidate_f;
			ret_r = min_r;
			return true;
		}
		else
			return false;
	}

	// Iterate on the children
	for (int i = 0; i < 8; i++) {
		if (children[i]->penetratedBy(ray)) {
			Face r_face;
			double r_r;
			if (children[i]->nearestIntersect(ray, r_face, r_r) &&
				r_r < min_r) {
				min_r = r_r;
				candidate_f = r_face;
			}
		}
	}

	if (min_r != INFTY) {
		ret_face = candidate_f;
		ret_r = min_r;
		return true;
	}
	else
		return false;
}

bool Node::penetratedBy(const Ray &ray) const {
	Vec3d o = ray.getOrigin();
	Vec3d d = ray.getDirection();

	Vec3d low = {
		(lowest[X] - o[X]) / d[X],
		(lowest[Y] - o[Y]) / d[Y],
		(lowest[Z] - o[Z]) / d[Z] };
	Vec3d high = {
		(highest[X] - o[X]) / d[X],
		(highest[Y] - o[Y]) / d[Y],
		(highest[Z] - o[Z]) / d[Z],
	};

	Vec3d temp = min(low, high);
	double r_near = temp[X] > temp[Y] ? temp[X] : temp[Y];
	r_near = r_near > temp[Z] ? r_near : temp[Z];
	
	temp = max(low, high);
	double r_far = temp[X] < temp[Y] ? temp[X] : temp[Y];
	r_far = r_far < temp[Z] ? r_far : temp[Z];

	return r_far > r_near;
}




/* Helper functions */
static const Vec3d findLowest(const vector<Face *> &_fptrs) {
	Vec3d lowest(INFTY);
	for (int i = 0; i < _fptrs.size(); i++) {
		for (int j = 0; j < 3; j++) {
			Vec3d v = *(_fptrs[i]->vertices[j]);
			lowest = min(lowest, v);
		}
	}
	return lowest;
}

static const Vec3d findHighest(const vector<Face *> &_fptrs) {
	Vec3d highest(-INFTY);
	for (int i = 0; i < _fptrs.size(); i++) {
		for (int j = 0; j < 3; j++) {
			Vec3d v = *(_fptrs[i]->vertices[j]);
			highest = max(highest, v);
		}
	}
	return highest;
}

static const Vec3d min(const Vec3d &l, const Vec3d &r) {
	return Vec3d(
		l[X] < r[X] ? l[X] : r[X],
		l[Y] < r[Y] ? l[Y] : r[Y],
		l[Z] < r[Z] ? l[Z] : r[Z]
	);
}

static const Vec3d max(const Vec3d &l, const Vec3d &r) {
	return Vec3d(
		l[X] > r[X] ? l[X] : r[X],
		l[Y] > r[Y] ? l[Y] : r[Y],
		l[Z] > r[Z] ? l[Z] : r[Z]
	);
}

static const Vec3d findDividingCenter(vector<Face *> facePtrs) {
	Vec3d ret;
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

static byte findChild(const Face &f, const Vec3d &cubeLow, const Vec3d &cubeMid, const Vec3d &cubeHigh) {
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

static double intersect_face(const Ray &ray, const Face &face) {
	Vec3d n = face.normal;

	// parallel test
	// It does not consider when the ray is INSIDE the face plane
	double r = n.dot(ray.getDirection());
	if (abs(r) < FLT_EPSILON) {
		return -1;
	}

	Vec3d v0 = *face.vertices[0];
	Vec3d p0 = ray.getOrigin();
	r = (n.dot(v0 - p0)) / r;

	// direction test
	if (r < FLT_EPSILON) {
		return -1;
	}

	Vec3d p_i = p0 + r * ray.getDirection();

	Vec3d u = *face.vertices[1] - *face.vertices[0];
	Vec3d v = *face.vertices[2] - *face.vertices[0];
	Vec3d w = p_i - v0;

	double uv = u.dot(v);	double wv = w.dot(v);
	double vv = v.dot(v);	double wu = w.dot(u);
	double uu = u.dot(u);
	double uv2_uuvv = uv * uv - uu * vv;

	if (uv2_uuvv == 0.)
		return -1;

	double s = (uv * wv - vv * wu) / uv2_uuvv;
	double t = (uv * wu - uu * wv) / uv2_uuvv;

	// s, t range test
	if (s < -FLT_EPSILON || t < -FLT_EPSILON || s + t > 1 + FLT_EPSILON) {
		return -1;
	}

	// Now we know that the ray intersects with the face
	// Return parameter of the ray
	return r;
}