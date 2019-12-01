#include <iostream>
#include "bmploader.h"
#include "vec.h"
#include "mesh.h"
#include "material.h"
#include "raytracer.h"
#include "definitions.h"
using namespace std;

int execute();

int main() {
	return execute();
}

int execute() {
	// Load mesh
	Material material;
	Mesh mesh("bunny.off", material);

	// Configure
	Light light(0, 10, 0, 1, 1, 1, 1);
	Camera camera(0, 0, 2, 0, 0, 0, 0, 1, 0, 900, 60, 1., 0.5, 10);

	// Run
	RayTracer rayTracer(&mesh, &light, camera);
	Face f;
	Vec3f v;
	Ray ray;
	if (rayTracer.intersect(Ray({ 0,0,2 }, { 0,0,-1 }), f, v))
		cout << v << endl;
	return 0;
}