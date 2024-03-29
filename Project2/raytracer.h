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
	Octree   *octree;
	Mesh     *meshes;
	Light    *lights;
	Camera    camera;

	int n_meshes;
	int n_lights;

public:
	RayTracer(Mesh *_meshes, int n_meshes, Light *_lights, int n_lights, const Camera &_camera); // initializer

	/* intersection() gives whether the ray intersects with faces in the space.
	 * params: ray      - the ray casted
	 *         ret_face - the face that would be returned as intersection face
	 *		   ret_vec  - the point that would be returned as intersection point
	 * return value: true if there is any face intersecting                      */
	bool intersect_slow(const Ray &ray, Face &ret_face, Vec3d &ret_vec) const;
	bool intersect(const Ray &ray, Face &ret_face, Vec3d &ret_vec) const;

	/* cast() does intersection test with the given ray,
	 * generating new rays, and determining colors.
	 * params: ray       - the ray that will be casted
	 *					 - the last ray casted recursively
	 *         prev_face - the face that ray origin resides
	 * return value: RGB color of the ray casted           */
	const Vec4d cast(const Ray &ray, int depth) const;

	/* render() triggers the whole rendering process. It returns pixels. */
	Vec3d **render() const;

	/* params:
	 *   (Ray)incident : incident ray
	 *   (Face)face : the face
	 *   (Vec3d)intersection_pos : the intersecting point
	 * return:
	 *   (Vec4f) Color vector + intensity (RGBI)         */
	Vec4d shadow(const Ray &incident, const Face& face, const Vec3d &intersection_pos) const;
};