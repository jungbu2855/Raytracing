#pragma once

#include "vec.h"
#include "material.h"
#include "mesh.h"
#include "ray.h"
#include "octree.h"
#include "definitions.h"

class RayTracer {
private:
	Octree   *octree;
	Mesh     *meshes;
	Light    *lights;
	Camera   *camera;

public:
	const Face *intersect(Ray ray) const;
	const Vec3f cast(Ray ray) const;
	const Vec3f *render() const;
};

const Ray find_primary_ray(int h, int w, Camera camera);