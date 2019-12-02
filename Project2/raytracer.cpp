#include "raytracer.h"
#include "definitions.h"

#include <vector>
#include <cmath>

static const Ray find_primary_ray(int h, int w, const Camera &camera);
static bool intersect_face(const Ray &ray, const Face &face, Vec3f &ret_vec);
static Vec3f colorRGBItoRGB(const Vec4f &rgbi);
static Vec4f shadow(const Face& face, const Vec3f intersection_pos);
static Vec4f setFinalColor(const Vec4f *c, int num);

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

/* Return value: RGB + light intensity */
const Vec4f RayTracer::cast(const Ray &ray, const Face &prev_face) const {
	// Get nearest intersection
	Face face;
	Vec3f pos;
	if (!intersect(ray, face, pos)) {
		// Return diffuse color only if it does not have intersections with others
		if (ray.getCollisions() != 0) {
			// Shadow ray must be from the non-primary rays
			return shadow(prev_face, ray.getOrigin());
		}
		else
			return { 0,0,0,0 }; // black for non-intersecting primary rays
	}

	// Generating second rays
	Vec4f colors[] =
	{
		cast(ray.reflect(face, pos), face),		// reflecting ray
		cast(ray.refract(face, pos), face),		// refracting ray
		shadow(face, pos)						// shadow ray
	};
	return setFinalColor(colors, 3);
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
			Vec4f rgbi = cast(primary_ray, Face());
			pixels[i][j] = colorRGBItoRGB(rgbi);
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
	// It does not consider when the ray is INSIDE the face plane
	float r = n.dot(ray.getDirection());
	if (r == 0) {
		return false;
	}

	Vec3f v0 = *face.vertices[0];
	Vec3f p0 = ray.getOrigin();
	r = (n.dot(v0 - p0)) / r;

	// direction test
	if (r < 0) {
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
		return false;
	}

	// Now we know that the ray intersects with the face
	ret_vec = v0 + s * u + t * v;
	return true;
}

static Vec3f colorRGBItoRGB(const Vec4f &rgbi) {
	// verifying
	for (int i = 0; i < 3; i++)
		assert(rgbi[i] >= 0 && rgbi[i] <= 1);
	assert(rgbi[A] >= 0);

	Vec3f cand ({ rgbi[R] * rgbi[A], rgbi[G] * rgbi[A], rgbi[B] * rgbi[A] });
	if (rgbi[A] <= 1)
		return cand;
	else {
		Vec3f ret = cand;
		for (int i = 0; i < 3; i++) {
			if (cand[i] > 1) {
				ret[i] = 1;
				int next = (i + 1) % 3; int nnext = (i + 2) % 3;
				ret[next] += cand[next] == 1 ? 0 : cand[i] - 1;
				ret[next] = ret[next] > 1 ? 1 : ret[next];
				ret[nnext] += cand[nnext] == 1 ? 0 : cand[i] - 1;
				ret[nnext] = ret[nnext] > 1 ? 1 : ret[nnext];
			}
		}
		return ret;
	}

}

/* param:
 *   (Face)face : the face
 *   (Vec3f)intersection_pos : the intersecting point
 * returns:
 *   (Vec4f) Color vector + intensity (RGBI)         */
static Vec4f shadow(const Face& face, const Vec3f intersection_pos) {
	Vec3f new_direction;
	float temp;

	// light[] 각각에 대하여 그 광원을 향하는 빛을 만들어주세요.
	// 만들어진 shadow ray 마다 intersection test를 수행합니다.
	// ** intersection이 있더라도 광원보다 물체가 뒤에 있으면 통과시켜야 합니다.
	// intersection을 통과한 ray마다 해당하는 광원의 색과 material 색을 섞습니다.
	// 각각 만들어진 색은 setFinalColor()로 합쳐진 뒤 리턴됩니다.

	Ray new_ray(intersection_pos, new_direction);
	Vec4f new_color(0, 0, 0, 0);

	return new_color;
}

/* param:
 *   (Vec4f *)c : collection of {RGB, intensity}s
 *   (int)num : number of colors			      */
static Vec4f setFinalColor(const Vec4f *c, int num) {
	return { 0,0,0,0 };
}