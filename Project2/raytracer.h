#pragma once

#include "vec.h"
#include "material.h"
#include "mesh.h"
#include "ray.h"
#include "octree.h"
#include "definitions.h"

class RayTracer {
private:
	//Octree   *octree;
	Mesh     *meshes;
	Light    *lights;
	Camera   *camera;

public:
	RayTracer(Mesh *_meshes, Light *_lights, Camera *_camera);
	const bool intersect(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const;
	const Vec3f cast(Ray ray) const;
	const Vec3f *render() const;
};

const Ray find_primary_ray(int h, int w, Camera camera);