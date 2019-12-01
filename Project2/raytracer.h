#pragma once

#include "vec.h"
#include "material.h"
#include "mesh.h"
#include "ray.h"
#include "octree.h"
#include "definitions.h"

/* RayTracer enables rendering based on more realistic optically modelled technique
 * It uses back-propagating rays from eye(camera) to the lights. */
class RayTracer {
private:
	//Octree   *octree;
	Mesh     *meshes;
	Light    *lights;
	Camera    camera;

public:
	RayTracer(Mesh *_meshes, Light *_lights, const Camera &_camera); // initializer

	/* intersection() gives whether the ray intersects with faces in the space.
	 * params: ray      - the ray casted
	 *         ret_face - the face that would be returned as intersection face
	 *		   ret_vec  - the point that would be returned as intersection point
	 * return value: true if there is any face intersecting                      */
	const bool intersect(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const;

	/* cast() does intersection test with the given ray,
	 * generating new rays, and determining colors.
	 * params: ray       - the ray that will be casted
	 *         prev_face - the face that ray origin resides
	 * return value: RGB color of the ray casted           */
	const Vec3f cast(const Ray &ray, const Face &prev_face) const;

	/* render() triggers the whole rendering process. It returns pixels. */
	Vec3f **render() const;
};