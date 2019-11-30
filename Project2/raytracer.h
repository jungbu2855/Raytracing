#pragma once

#include "vec.h"
#include "material.h"
#include "mesh.h"
#include "ray.h"

class Octree;
class Mesh;
class Material;
struct Light;
struct Face;
struct Camera;

typedef Vec3<float> Vec3f;

class RayTracer {
private:
	Octree   *octree;
	Mesh     *meshes;
	Material *materials;
	Light    *lights;
	Camera   *camera;

public:
	const Face *intersect(Ray ray) const;
	const Vec3f cast(Ray ray) const;
	const Vec3f *render() const;
};

const Ray find_primary_ray(int h, int w, Camera camera);