#include <iostream>
#include "bmploader.h"
#include "vec.h"
#include "mesh.h"
#include "material.h"
#include "raytracer.h"
#include "definitions.h"
#include "octree.h"

using namespace std;

constexpr int NUM_OBJS_TO_BE_RENDERED = 8;

int execute();

int main() {

	return execute();
}

int execute() {
	// vars

	Material material[NUM_OBJS_TO_BE_RENDERED];
	Mat4d models[NUM_OBJS_TO_BE_RENDERED];

	// Object 0:
	material[0] = Material(Vec4d(.8, 1, .9, 1), 100.0, 0.);	// Material property
	material[1] = Material(Vec4d(.7, .7, .7, 1), 100.0, 1.);
	material[2] = Material(Vec4d(1., 0., 0., 1), 100.0, 0.0);
	material[3] = Material(Vec4d(0.663, 0.875, 0.929, 1), 100.0, 0.0);
	material[4] = Material(Vec4d(0., 0., 0., 1), 100.0, 0.0);
	material[5] = Material(Vec4d(0.345, 0.667, 0.31, 1), 100.0, 0.0);
	material[6] = Material(Vec4d(1., 0., 0., 1), 100.0, 0.0);
	material[7] = Material(Vec4d(1., 1., 1., 0.), 1.4, 0.0);
	models[0] = rotate(M_PI / 3, Vec3d(1, 0, 0));	// Model transform matrix
	models[1] = translate(Vec3d(0, -0.5, 0));
	models[2] = translate(Vec3d(0.2, 1., 0)) * rotate(M_PI / 4, Vec3d(0, 1, 0));
	models[3] = translate(Vec3d(0, 0, -3.5)) * rotate(M_PI / 2, Vec3d(1, 0, 0));
	models[4] = translate(Vec3d(-1., -0.3, 1.)) * rotate(M_PI / 3, Vec3d(0, 1, 1));
	models[5] = translate(Vec3d(1.5, 0, 0)) * rotate(M_PI / 2.2, Vec3d(0, -1, 0)) * rotate(M_PI / 2, Vec3d(1, 0, 0));
	models[6] = translate(Vec3d(1, 0., 0.3)) * rotate(M_PI / 2, Vec3d(1, 0, 0));
	models[7] = translate(Vec3d(0, 0.3, 2.));

	cout << models[0] << endl;

	Mesh meshes[NUM_OBJS_TO_BE_RENDERED] = {
		Mesh("bunny.off", material[0], models[0], 1),
		Mesh(Mesh::SQUARE, material[1], models[1], 10),		//mirror floor
		Mesh(Mesh::SQUARE, material[3], models[3], 10),		//sky blue back board
		Mesh("sphere.off", material[2], models[2], 0.7),
		Mesh(Mesh::CUBE, material[4], models[4], 0.5),
		Mesh(Mesh::SQUARE, material[5], models[5], 5),		//green right board
		Mesh("sphere.off", material[6], models[6], 0.4),		//sphere2
		Mesh("sphere.off", material[7], models[7], 1)			//sphere3
	};

	// Configure lights
	Light lights[] = {
		Light(1, 6, 6, 1., 1., 1., 1),
		Light(-1, 6, 6, 1., 1., 1., 1)
	};

	// Configure camera
	Camera camera(
		0, 0, 5,	// eye
		0, 0, 0,	// center
		0, 1, 0,	// up
		90, 		// img height
		60,			// fovy
		1.,			// aspect
		0.5, 10		// zNear, zFar
	);

	// Run
	RayTracer rayTracer(meshes, sizeof meshes / sizeof(Mesh), lights, sizeof lights / sizeof(Light), camera);
	Vec3d** result = rayTracer.render();

	int h = camera.height;
	int w = h * camera.aspect_ratio;
	uchar4 *converted = new uchar4[h*w];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			converted[h*i + j].x = result[i][j][R] * 255;
			converted[h*i + j].y = result[i][j][G] * 255;
			converted[h*i + j].z = result[i][j][B] * 255;
		}

	}

	SaveBMPFile((uchar4 *)converted, h, w, "BUNNY2.BMP", "90-90.bmp");
	return 0;
}