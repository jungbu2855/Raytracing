#include <iostream>
#include "bmploader.h"
#include "vec.h"
#include "mesh.h"
#include "material.h"
#include "raytracer.h"
#include "definitions.h"
#include "octree.h"

using namespace std;

#define NUM_OBJS_TO_BE_RENDERED 1

int execute();

int main() {
	Octree o;
	Face *fp = new Face();
	fp->vertices[0] = new Vec3f(0., 1., -0.);
	fp->vertices[1] = new Vec3f(1., 1.01, -0.01);
	fp->vertices[2] = new Vec3f(2., 1.02, -0.001);
	Face *fp2 = new Face();
	fp2->vertices[0] = new Vec3f(1.5, 0.5, -0.5);
	fp2->vertices[1] = new Vec3f(0.5, -0.5, -1.5);
	fp2->vertices[2] = new Vec3f(-0.5, -1.5, -2.5);
	Face *fpp[2] = { fp, fp2 };
	o.insertAll(fpp, 2);
	
	Octree::OctreeNode n;
	vector<Face *> vec;
	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);
	n.push_faces(vec);
	cout << n.isLeaf() << endl;
	return execute();
}

int execute() {
	// vars

	Material material[NUM_OBJS_TO_BE_RENDERED];
	Mat4f models[NUM_OBJS_TO_BE_RENDERED];

	// Object 0:
	material[0] = Material();	// Material property
	models[0].loadIdentity();	// Model transform matrix

	Mesh meshes[NUM_OBJS_TO_BE_RENDERED] = {
		Mesh("bunny.off", material[0], models[0])
	};

	// Configure lights
	Light lights[] = {
		Light(0, 10, 0, 1, 1, 1, 1)
	};

	// Configure camera
	Camera camera(
		0, 0, 2,	// eye
		0, 0, 0,	// center
		0, 1, 0,	// up
		900,		// img height
		60,			// fovy
		1.,			// aspect
		0.5, 10		// zNear, zFar
	);

	// Run
	RayTracer rayTracer(meshes, 1, lights, 1, camera);
	Face f;
	Vec3f v;
	Ray ray;
	if (rayTracer.intersect(Ray({ 0,0,-2 }, { 0,0,1 }), f, v))
		cout << v << endl;
	cout << (abs(v[X] - 0.) < FLT_EPSILON ? "TRUE" : "FALSE") << endl;
	return 0;
}