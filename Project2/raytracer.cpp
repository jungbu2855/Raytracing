#include "raytracer.h"
#include "definitions.h"

#include <vector>

static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec);

static struct FaceVec3 {
	Face  f;
	Vec3f v;
};

RayTracer::RayTracer(Mesh *_meshes, Light *_lights, Camera *_camera)
	: meshes(_meshes), lights(_lights), camera(_camera) {}

const bool RayTracer::intersect(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const {
	
	// Search whole space, find candidates
	vector<FaceVec3> candidates;
	for (int i = 0; i < meshes->get_size(); i++) {
		Face face = (meshes->get_faces())[i];
		Vec3f cand_vec;
		// face-wise searching helper function
		bool is_intersect = intersect_face(ray, face, cand_vec);
		if (is_intersect) {
			FaceVec3 candidate = { face, cand_vec };
			candidates.push_back(candidate);
		}
	}

	// no candidates
	if (candidates.empty())
		return false;

	// find some candidates: get the nearest candidate
	float mind = 1000000;
	FaceVec3 nearest;
	for (int i = 0; i < candidates.size(); i++) {
		FaceVec3 candidate = candidates[i];
		Vec3f v_o = candidate.v - ray.getOrigin();
		float dist2 = v_o.dot(v_o);
		if (dist2 < mind) {
			mind = dist2;
			nearest = candidate;
		}
	}

	ret_face = nearest.f;	// intersection face
	ret_vec = nearest.v;	// intersection point
	return true;
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

static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec) {
	Vec3f n = face.normal;

	// parallel test
	// It does not consider when the ray is INSIDE in the face plane
	float r = n.dot(ray.getDirection());
	if (r == 0) {
		//cout << "parallel test failed" << endl;
		return false;
	}

	Vec3f v0 = *face.vertices[0];
	Vec3f p0 = ray.getOrigin();
	r = (n.dot(v0 - p0)) / r;

	// direction test
	if (r < 0) {
		//cout << "direction test failed" << endl;
		return false;
	}

	Vec3f p_i = p0 + r * ray.getDirection();

	Vec3f u = *face.vertices[1] - *face.vertices[0];
	Vec3f v = *face.vertices[2] - *face.vertices[0];
	Vec3f w = p_i - v0;

	float uv = u.dot(v);	float wv = w.dot(v);
	float vv = v.dot(v);	float wu = w.dot(u);
	float uu = u.dot(u);
	float uv2_uuvv = uv * uv - uu * vv;

	float s = (uv * wv - vv * wu) / uv2_uuvv;
	float t = (uv * wu - uu * wv) / uv2_uuvv;

	// s, t range test
	if (s < 0 || t < 0 || s + t > 1) {
		//cout << "s, t range test failed" << endl;
		return false;
	}

	// Now we know that the ray intersects with the face
	ret_vec = v0 + s * u + t * v;
	cout << "intersect!! p0:" << p0 << " r:" << r << endl;
	return true;
}