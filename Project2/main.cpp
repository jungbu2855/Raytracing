#include <iostream>
#include "bmploader.h"
#include "vec.h"
#include "mesh.h"
#include "material.h"
#include "raytracer.h"
#include "definitions.h"
#include "octree.h"

using namespace std;

constexpr int NUM_OBJS_TO_BE_RENDERED = 1;

int execute();

int main() {
	Face *fp = new Face();
	fp->vertices[0] = new Vec3f(0., 1., -0.);
	fp->vertices[1] = new Vec3f(1., 1.01, -0.01);
	fp->vertices[2] = new Vec3f(2., 1.02, -0.001);
	Face *fp2 = new Face();
	fp2->vertices[0] = new Vec3f(1.5, 0.5, -0.5);
	fp2->vertices[1] = new Vec3f(0.5, -0.5, -1.5);
	fp2->vertices[2] = new Vec3f(-0.5, -1.5, -2.5);
	Face *fpp[2] = { fp, fp2 };
//	o.insertAll(fpp, 2);
	
	Octree::OctreeNode n;
	vector<Face *> vec;
//	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);	vec.push_back(fp); vec.push_back(fp2);
//	n.push_faces(vec);
	cout << n.isLeaf() << endl;
	return execute();
}

int execute() {
	// vars

	Material material[NUM_OBJS_TO_BE_RENDERED];
	Mat4f models[NUM_OBJS_TO_BE_RENDERED];

	// Object 0:
	material[0] = Material(.8, 100.0, 0.1);	// Material property
	models[0].loadIdentity();	// Model transform matrix

	Mesh meshes[NUM_OBJS_TO_BE_RENDERED] = {
		Mesh("sphere.off", material[0], models[0])
	};

	// Configure lights
	Light lights[] = {
		//Light(0, 10, 0, .5, .5, .9, 1),
		Light(-10, 0, 10, .3, .9, .5, 1)
	};

	// Configure camera
	Camera camera(
		0, 0, 2,	// eye
		0, 0, 0,	// center
		0, 1, 0,	// up
		90,		// img height
		60,			// fovy
		1.,			// aspect
		0.5, 10		// zNear, zFar
	);

	// Run
	RayTracer rayTracer(meshes, sizeof meshes / sizeof (Mesh), lights, sizeof lights / sizeof (Light), camera);
	Vec3f** result = rayTracer.render();
	
	int h = camera.height;
	int w = h * camera.aspect_ratio;
	uchar4 *converted = new uchar4[h*w];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			converted[h*i+j].x = result[i][j][R] * 255;
			converted[h*i+j].y = result[i][j][G] * 255;
			converted[h*i+j].z = result[i][j][B] * 255;
		}

	}

	SaveBMPFile((uchar4 *)converted, h, w, "BUNNY.BMP", "90-90.bmp");
	return 0;
}