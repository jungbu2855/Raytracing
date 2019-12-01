#include "raytracer.h"
#include "definitions.h"

#include <vector>
#include <cmath>

static const Ray find_primary_ray(int h, int w, const Camera &camera);
static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec);

struct FaceVec3 {
	Face  f;
	Vec3f v;
};

RayTracer::RayTracer(Mesh *_meshes, Light *_lights, const Camera &_camera)
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
	
}

Vec3f ** RayTracer::render() const {
	// init local vars
	int height = camera.height;
	int width = camera.height * camera.aspect_ratio;
	Vec3f **pixels = new Vec3f*[height];	// RGB pixel container
	for (int i = 0; i < height; i++)
		pixels[i] = new Vec3f[width];
	
	// Main behavior
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// find primary ray for each pixel
			Ray primary_ray = find_primary_ray(i, j, camera);
			// cast the primary ray to space, collecting pixel colors
			pixels[i][j] = cast(primary_ray);
		}
	}
	
	// Now you have colored whole pixels
	return pixels;
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
	return Ray(origin, ray_dir);
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

static Ray shadow(const Ray& incident, const Face& face, const Vec3f ret_vec) {
	Vec3f new_direction;
	float temp;
	temp = face.normal.dot(incident.getDirection());
	new_direction = incident.getDirection() - 2 * temp * face.normal;

	Ray new_ray(ret_vec, new_direction);

	Vec3f new_color(0, 0, 0);

	new_ray.setColor(new_color);

	return new_ray;
}