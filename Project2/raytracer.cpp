#include "raytracer.h"
#include "definitions.h"

const Face * RayTracer::intersect(Ray ray) const {
	return nullptr;
}

const Vec3f RayTracer::cast(Ray ray) const {
	return Vec3f();
}

const Vec3f * RayTracer::render() const {
	// int height = camera.height;
	// int width = camera.height * camera.aspect_ratio;
	// Vec3f **pixels = new Vec3f*[height];
	// for (int i = 0; i < height; i++)
	// 	pixels[i] = new Vec3f[width];
	// 
	// for (int i = 0; i < height; i++) {
	// 	for (int j = 0; j < width; j++) {
	// 		Ray primary_ray = find_primary_ray(i, j, camera);
	// 	}
	// }
	return nullptr;
}

const Ray find_primary_ray(int h, int w, Camera camera) {
	return Ray();
}