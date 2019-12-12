#include "raytracer.h"
#include "definitions.h"

#include <vector>
#include <cmath>
#include <cassert>

#include <thread>

static const Ray find_primary_ray(int h, int w, const Camera &camera);
static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec);
static Vec3f colorRGBItoRGB(const Vec4f &rgbi);
static Vec4f setFinalColor(const Vec4f *c, int num);

static Vec3f **pixels;
static thread *threads;

constexpr int MAX_RAY_DEPTH = 5;

struct FaceVec3 {
	Face  f;
	Vec3f v;
};

RayTracer::RayTracer(Mesh *_meshes, int _n_meshes, Light *_lights, int _n_lights, const Camera &_camera)
	: meshes(_meshes), lights(_lights), n_meshes(_n_meshes), n_lights(_n_lights), camera(_camera) {
	Face **allFaces;
	int n_allFaces = 0;
	for (int i = 0; i < _n_meshes; i++) {
		n_allFaces += _meshes[i].get_size();
	}
	allFaces = new Face*[n_allFaces];

	int idx = 0;
	for (int i = 0; i < _n_meshes; i++) {
		Face *faces = _meshes[i].get_faces();
		for (int j = 0; j < _meshes[i].get_size(); j++) {
			allFaces[idx++] = &(faces[j]);
		}
	}

	octree = new Octree(allFaces, n_allFaces);

	delete allFaces;
}

bool RayTracer::intersect_slow(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const {
	return octree->getNearestIntersect(ray, ret_face, ret_vec);
}

bool RayTracer::intersect(const Ray &ray, Face &ret_face, Vec3f &ret_vec) const {
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

/* Return value: RGB + light intensity */
const Vec4f RayTracer::cast(const Ray &ray, const Face &prev_face, int depth) const {
	// Early termination
	if (depth > MAX_RAY_DEPTH || ray.getIntensity() == 0)
		return { 0,0,0,0 };	// Transparent (no color)
	// Get nearest intersection
	Face face;
	Vec3f pos;
	if (!intersect(ray, face, pos)) {
		// Return diffuse color only if it does not have intersections with others
		if (ray.getCollisions() != 0) {
			// Shadow ray must be from the non-primary rays
			return shadow(ray, prev_face, ray.getOrigin()); // 여기 ray 수정하기
		}
		else
			return { 1,0,0,1 }; // black for non-intersecting primary rays
	}

	// Generating second rays
	Vec4f colors[] =
	{
		//cast(ray.reflect(face, pos), face, depth+1),		// reflecting ray
		//cast(ray.refract(face, pos), face, depth+1),		// refracting ray
		shadow(ray, face, pos)					// shadow ray
	};
	return setFinalColor(colors, 1);
}

void render_helper(int i, int j, const Camera &cam, const RayTracer &inst) {
	// find primary ray for each pixel
	Ray primary_ray = find_primary_ray(i, j, cam);
	// cast the primary ray to space, collecting pixel colors
	Vec4f rgbi = inst.cast(primary_ray, Face(), 0);
	pixels[i][j] = colorRGBItoRGB(rgbi);
}

Vec3f ** RayTracer::render() const {
	// init local vars
	int height = camera.height;
	int width = camera.height * camera.aspect_ratio;
	pixels = new Vec3f*[height];	// RGB pixel container
	for (int i = 0; i < height; i++)
		pixels[i] = new Vec3f[width];

	threads = new thread[width];
	
	// Main behavior
	cout << "Complete:";
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			threads[j] = thread(render_helper, i, j, camera, *this);
		}
		for (int j = 0; j < width; j++) {
			threads[j].join();
		}
		if (i % ((height >= 100) ? (height / 100) : (1)) == 0)
			cout << "#";
	}
	
	// Now you have colored whole pixels
	return pixels;
}

Vec4f RayTracer::shadow(const Ray &incident, const Face& face, const Vec3f &intersection_pos) const {
	Vec3f view = -(incident.getDirection());
	Vec4f *results = new Vec4f[n_lights];
	for (int i = 0; i < n_lights; i++) {
		Vec3f shad_dir = lights[i].position - intersection_pos;
		shad_dir.normalize();
		Ray shad (intersection_pos, shad_dir);
		Face destf;	Vec3f destv;
		if (intersect(shad, destf, destv)
			&& intersection_pos.distance(destv) < intersection_pos.distance(lights[i].position)) {
			results[i] = { 0, 0, 1, 1 };
			continue;
		}

		Ray refl = incident.reflect(face, intersection_pos);
		Vec4f diffuse;
		for (int j = 0; j < 3; j++) {
			diffuse[j] = lights[i].color[j] * (face.material->getcolor())[j];
		}
		diffuse[3] = face.material->getopacity() * lights[i].color[3] * fmax((shad.getDirection()).dot(face.normal), 0);
		results[i] = diffuse;
	}

	Vec4f ret = setFinalColor(results, n_lights);
	delete results;
	return ret;
}

static const Ray find_primary_ray(int h, int w, const Camera &camera) {
	// init vars
	Vec3f origin = camera.position;
	Vec3f view_dir = camera.center - camera.position;
	float h_max = camera.zNear * tan(camera.fovy / 2);
	float w_max = h_max * camera.aspect_ratio;

	// Initialize primary ray to eye(0,0,0), center(0,0,-1), up(0,1,0)
	// Perspective view
	float y = (2 * (float)h / camera.height - 1) * h_max;
	float x = (2 * (float)w / (camera.height * camera.aspect_ratio) - 1) * w_max;
	Vec3f ray_dir = { x,y,-1 };

	// View transformation
	Mat4f view; view.loadIdentity();

	// (1) rotate (dir)
	Vec3f axis = view_dir.cross({0,0,-1});
	float sin = axis.norm() / view_dir.norm();
	float cos = view_dir.dot({ 0,0,-1 }) / view_dir.norm();
	float angle = cos >= 0 ? asin(sin) : acos(cos);
	view *= rotate(angle, axis);

	// (2) rotate (up)
	Mat3f view3 = view;
	Vec3f orig_up = { 0,1,0 };
	orig_up = view3 * orig_up;
	Vec3f new_up = camera.up - (view_dir.dot(camera.up) / view_dir.dot(view_dir)) * view_dir;

	axis = new_up.cross(orig_up);
	sin = axis.norm() / new_up.norm();
	cos = new_up.dot(orig_up) / new_up.norm();
	angle = cos >= 0 ? asin(sin) : acos(cos);
	view = rotate(angle, axis) * Mat4f(view3);

	ray_dir = Mat3f(view) * ray_dir;
	return Ray(origin, ray_dir, 1);
}

static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec) {
	Vec3f n = face.normal;

	// parallel test
	// It does not consider when the ray is INSIDE the face plane
	float r = n.dot(ray.getDirection());
	if (abs(r) < 2 * FLT_EPSILON)
		return false;

	Vec3f v0 = *face.vertices[0];
	Vec3f p0 = ray.getOrigin();
	r = (n.dot(v0 - p0)) / r;

	// direction test
	if (r < 10 * FLT_EPSILON) {
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

	if (uv2_uuvv == 0.f)
		return false;

	float s = (uv * wv - vv * wu) / uv2_uuvv;
	float t = (uv * wu - uu * wv) / uv2_uuvv;

	// s, t range test
	if (s < 0.f || t < 0.f || s + t > 1) {
		return false;
	}

	// Now we know that the ray intersects with the face
	ret_vec = v0 + s * u + t * v;
	assert(ret_vec.distance(ray.getOrigin()) > 10*FLT_EPSILON);
	return true;
}

static Vec3f colorRGBItoRGB(const Vec4f &rgbi) {
	// verifying
	for (int i = 0; i < 3; i++)
		assert(rgbi[i] >= -0.f && rgbi[i] <= 1.f);
	assert(rgbi[A] >= 0);

	Vec3f ret ({ rgbi[R] * rgbi[A], rgbi[G] * rgbi[A], rgbi[B] * rgbi[A] });
	for (int i = 0; i < 3; i++) 
		ret[i] = ret[i] > 1 ? 1 : ret[i];
	return ret;
}

/* param:
 *   (Vec4f *)c : collection of {RGB, intensity}s
 *   (int)num : number of colors			      */
// 일단 테스트 용으로 제 맘대로 만들었는데 아마 어딘가 바뀌어야할 것 같아요
static Vec4f setFinalColor(const Vec4f *c, int num) {
	Vec4f color = { 0,0,0,0 };
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < 4; j++) {
			assert(c[i][j] >= -0.f &&
				c[i][j] <= 1.0f);
		}
		color[R] += c[i][R] * c[i][A];
		color[G] += c[i][G] * c[i][A];
		color[B] += c[i][B] * c[i][A];
		color[A] += c[i][A];
	}

	for (int i = 0; i < 4; i++) {
		color[i] = color[A] > FLT_EPSILON ? color[i] / color[A] : 0;
		color[i] = color[i] > 1. ? 1. : color[i];
		assert(color[i] >= -0.f && color[i] <= 1.f);
	}
	return color;
}