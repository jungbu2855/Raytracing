#include "raytracer.h"
#include "definitions.h"

#include <vector>
#include <cmath>
#include <cassert>

#include <thread>

static const Ray find_primary_ray(int h, int w, const Camera &camera);
static bool intersect_face(const Ray &ray, const Face &face, Vec3d &ret_vec);
static Vec3d colorRGBItoRGB(const Vec4d &rgbi);
static Vec4d setFinalColor(const Vec4d *c, int num);

static Vec3d **pixels;
static thread *threads;

constexpr int MAX_RAY_DEPTH = 5;

struct FaceVec3 {
	Face  f;
	Vec3d v;
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

bool RayTracer::intersect(const Ray &ray, Face &ret_face, Vec3d &ret_vec) const {
	return octree->getNearestIntersect(ray, ret_face, ret_vec);
}

bool RayTracer::intersect_slow(const Ray &ray, Face &ret_face, Vec3d &ret_vec) const {
	// Search whole space, find candidates
	vector<FaceVec3> candidates;
	for (int i = 0; i < meshes->get_size(); i++) {
		Face face = (meshes->get_faces())[i];
		Vec3d cand_vec;
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
	double mind = 1000000;
	FaceVec3 nearest;
	for (int i = 0; i < candidates.size(); i++) {
		FaceVec3 candidate = candidates[i];
		Vec3d v_o = candidate.v - ray.getOrigin();
		double dist2 = v_o.dot(v_o);
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
const Vec4d RayTracer::cast(const Ray &ray, int depth) const {
	// Early termination
	if (depth > MAX_RAY_DEPTH || ray.getIntensity() == 0)
		return { 0,0,0,0 };	// Transparent (no color)
	// Get nearest intersection
	Face face;
	Vec3d pos;
	if (!intersect(ray, face, pos)) {
		return { 0,0,0,1 }; // return black for non-intersecting ray
	}

	
	// Generating second rays
	if (face.material->getopacity() < 1 - FLT_EPSILON) {
		if (face.material->getmirror() > FLT_EPSILON) {
			Vec4d colors[] =
			{
				cast(ray.reflect(face, pos), depth + 1),		// reflecting ray
				cast(ray.refract(face, pos), depth + 1),		// refracting ray
				shadow(ray, face, pos)							// shadow ray
			};
			return setFinalColor(colors, 3);
		}
		else {
			Vec4d colors[] =
			{
				cast(ray.refract(face, pos), depth + 1),		// refracting ray
				shadow(ray, face, pos)							// shadow ray
			};
			return setFinalColor(colors, 2);
		}
	}
	else {
		if (face.material->getmirror() > FLT_EPSILON) {
			Vec4d colors[] =
			{
				cast(ray.reflect(face, pos), depth + 1),		// reflecting ray
				shadow(ray, face, pos)							// shadow ray
			};
			return setFinalColor(colors, 2);
		}
		else {
			return shadow(ray, face, pos);
		}
	}
}

void render_helper(int i, int j, const Camera &cam, const RayTracer &inst) {
	// find primary ray for each pixel
	Ray primary_ray = find_primary_ray(i, j, cam);
	// cast the primary ray to space, collecting pixel colors
	Vec4d rgbi = inst.cast(primary_ray, 0);
	pixels[i][j] = colorRGBItoRGB(rgbi);
}

Vec3d ** RayTracer::render() const {
	// init local vars
	int height = camera.height;
	int width = camera.height * camera.aspect_ratio;
	pixels = new Vec3d*[height];	// RGB pixel container
	for (int i = 0; i < height; i++)
		pixels[i] = new Vec3d[width];

	threads = new thread[width];
	
	// Main behavior
	cout << "Complete:";
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			threads[j] = thread(render_helper, i, j, camera, *this);
			//render_helper(i, j, camera, *this);
		}
		for (int j = 0; j < width; j++) {
			threads[j].join();
		}
		if (i % ((height >= 100) ? (height / 100) : (1)) == 0)
			cout << "#";
	}

	delete[] threads;
	
	// Now you have colored whole pixels
	return pixels;
}

Vec4d RayTracer::shadow(const Ray &incident, const Face& face, const Vec3d &intersection_pos) const {
	Vec3d view = -(incident.getDirection());
	view.normalize();
	Vec4d *results = new Vec4d[n_lights];
	for (int i = 0; i < n_lights; i++) {
		Vec3d shad_dir = lights[i].position - intersection_pos;
		shad_dir.normalize();
		Ray shad (intersection_pos, shad_dir, 1.0f);
		Face destf;	Vec3d destv;
		if (intersect(shad, destf, destv)
			&& intersection_pos.distance(destv) < intersection_pos.distance(lights[i].position)) {
			results[i] = { 0, 0, 0, face.material->getopacity() };
			continue;
		}

		shad.attenuate(lights[i].position);
		double att = shad.getIntensity();
		Vec3d refl_dir = intersection_pos - lights[i].position;
		refl_dir.normalize();
		Ray refl = Ray(lights[i].position, refl_dir).reflect(face, intersection_pos);

		Vec4d diffuse, specular;
		diffuse = (lights[i].color * lights[i].color[A] *
			face.material->getcolor() * face.material->getopacity()) *
			fmax(shad_dir.dot(face.normal), 0);
		diffuse[3] = 1;

		double shininess = face.material->getmirror() < 0.95 ?
			1 / (1 - face.material->getmirror()) * 10:
			200.;
		specular = (lights[i].color * lights[i].color[A]) *
			pow(fmax(view.dot(refl.getDirection()), 0), shininess);
		specular[3] = 1;
		results[i] = diffuse + specular;
		for (int j = 0; j < 3; j++)
			results[i][j] = results[i][j] > 1.f ? 1.f : results[i][j];
		results[i][A] = att;
	}

	Vec4d ret = setFinalColor(results, n_lights);
	delete[] results;
	return ret;
}

static const Ray find_primary_ray(int h, int w, const Camera &camera) {
	// init vars
	Vec3d origin = camera.position;
	double h_max = camera.zNear * tan(camera.fovy / 2);
	double w_max = h_max * camera.aspect_ratio;

	// Initialize primary ray to eye(0,0,0), center(0,0,-1), up(0,1,0)
	// Perspective view
	double y = (2 * (double)h / camera.height - 1) * h_max;
	double x = (2 * (double)w / (camera.height * camera.aspect_ratio) - 1) * w_max;
	Vec3d ray_dir = { x,y,-1 };

	// View transformation
	Mat4d view;

	Vec3d forward = - camera.center + camera.position;
	forward.normalize();
	Vec3d left = camera.up.cross(forward);
	left.normalize();
	Vec3d up = forward.cross(left);
	up.normalize();
	view.setMat_ij(left[X], 0, 0);
	view.setMat_ij(left[Y], 1, 0);
	view.setMat_ij(left[Z], 2, 0);
	view.setMat_ij(up[X], 0, 1);
	view.setMat_ij(up[Y], 1, 1);
	view.setMat_ij(up[Z], 2, 1);
	view.setMat_ij(forward[X], 0, 2);
	view.setMat_ij(forward[Y], 1, 2);
	view.setMat_ij(forward[Z], 2, 2);

	view.setMat_ij(camera.position[X], 0, 3);
	view.setMat_ij(camera.position[Y], 1, 3);
	view.setMat_ij(camera.position[Z], 2, 3);
	view.setMat_ij(1, 3, 3);

	ray_dir = Mat3d(view) * ray_dir;
	return Ray(origin, ray_dir, 1);
}

static bool intersect_face(const Ray &ray, const Face &face, Vec3d &ret_vec) {
	Vec3d n = face.normal;

	// parallel test
	// It does not consider when the ray is INSIDE the face plane
	double r = n.dot(ray.getDirection());
	if (abs(r) < 2 * FLT_EPSILON)
		return false;

	Vec3d v0 = *face.vertices[0];
	Vec3d p0 = ray.getOrigin();
	r = (n.dot(v0 - p0)) / r;

	// direction test
	if (r < 16 * FLT_EPSILON) {
		return false;
	}

	Vec3d p_i = p0 + r * ray.getDirection();

	Vec3d u = *face.vertices[1] - *face.vertices[0];
	Vec3d v = *face.vertices[2] - *face.vertices[0];
	Vec3d w = p_i - v0;

	double uv = u.dot(v);	double wv = w.dot(v);
	double vv = v.dot(v);	double wu = w.dot(u);
	double uu = u.dot(u);
	double uv2_uuvv = uv * uv - uu * vv;

	if (uv2_uuvv == 0.f)
		return false;

	double s = (uv * wv - vv * wu) / uv2_uuvv;
	double t = (uv * wu - uu * wv) / uv2_uuvv;

	// s, t range test
	if (s < -0.f || t < -0.f || s + t > 1) {
		return false;
	}

	// Now we know that the ray intersects with the face
	ret_vec = v0 + s * u + t * v;
	assert(ret_vec.distance(ray.getOrigin()) > 10*FLT_EPSILON);
	return true;
}

static Vec3d colorRGBItoRGB(const Vec4d &rgbi) {
	// verifying
	for (int i = 0; i < 3; i++)
		assert(rgbi[i] >= -0.f && rgbi[i] <= 1.f);
	assert(rgbi[A] >= 0);

	Vec3d ret ({ rgbi[R] * rgbi[A], rgbi[G] * rgbi[A], rgbi[B] * rgbi[A] });
	for (int i = 0; i < 3; i++) 
		ret[i] = ret[i] > 1 ? 1 : ret[i];
	return ret;
}

/* param:
 *   (Vec4d *)c : collection of {RGB, intensity}s
 *   (int)num : number of colors			      */
static Vec4d setFinalColor(const Vec4d *c, int num) {
	Vec4d color = { 0,0,0,0 };
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
		color[i] = color[A] > 8*FLT_EPSILON ? color[i] / color[A] : 0;
		color[i] = color[i] > 1. ? 1. : color[i];
		assert(color[i] >= -0.f && color[i] <= 1.f);
	}
	return color;
}