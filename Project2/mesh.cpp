#include "mesh.h"
#include "definitions.h"

#include <iostream>
#include <cstring>
#include <cassert>

using namespace std;

static float INF = 1000;

static void update_maxmin(const Vec3f &v, Vec3f &max, Vec3f &min);
static void get_normal(Face &f);

Mesh::Mesh(const char *filename, const Material &mat, int dim) : mesh_dim(dim), material(mat)
{
	FILE *f;
	char buf[64];
	int nv, nf;

	// Open file
	assert(fopen_s(&f, filename, "r") == 0);

	// 1. First line should contain OFF only
	fscanf_s(f, "%s\n", buf, (unsigned int )(sizeof buf));
	assert(strcmp(buf, "OFF") == 0);

	// 2. Second line should contain total number of v, f, and 0
	fscanf_s(f, "%d %d 0\n", &nv, &nf);
	assert(nv >= 0 && nf >= 0);

	cout << "Mesh file opened" << endl;

	// 2+. Try to allocate mem space
	vertices = new Vec3f[nv];
	faces = new Face[nf];
	assert(vertices != 0 && faces != 0);
	mesh_size = nf;

	// 3. Vertex Coordinates
	Vec3f v_max = { -INF, -INF, -INF };
	Vec3f v_min = { INF, INF, INF };

	// 3-1. Read the positions (w/ finding max/min)
	for (int i = 0; i < nv; ++i)
	{
		fscanf_s(f, "%f %f %f\n", &(vertices[i][X]), &(vertices[i][Y]), &(vertices[i][Z]));
		update_maxmin(vertices[i], v_max, v_min);
	}

	// 3-2. Tune the vertices to be centered, properly sized
	Vec3f center = { (v_max[X] + v_min[X]) / 2,
		(v_max[Y] + v_min[Y]) / 2, (v_max[Z] + v_min[Z]) / 2 };
	float mulfact;
	if (v_max[X] - v_min[X] > v_max[Y] - v_min[Y]
	 && v_max[X] - v_min[X] > v_max[Z] - v_min[Z])
		mulfact = dim / (v_max[X] - v_min[X]);
	else if (v_max[Y] - v_min[Y] > v_max[Z] - v_min[Z])
		mulfact = dim / (v_max[Y] - v_min[Y]);
	else
		mulfact = dim / (v_max[Z] - v_min[Z]);

	for (int i = 0; i < nv; ++i)
	{
		vertices[i][X] = (vertices[i][X] - center[X]) * mulfact;
		vertices[i][Y] = (vertices[i][Y] - center[Y]) * mulfact;
		vertices[i][Z] = (vertices[i][Z] - center[Z]) * mulfact;
	}

	cout << "Vertex read" << endl;

	// 4. Face Coordinates (Only triangular faces)
	for (int i = 0; i < nf; ++i)
	{
		int idx1, idx2, idx3;
		fscanf_s(f, "3 %d %d %d\n", &idx1, &idx2, &idx3);
		faces[i].vertices[0] = vertices + idx1;
		faces[i].vertices[1] = vertices + idx2;
		faces[i].vertices[2] = vertices + idx3;

		// get normal
		get_normal(faces[i]);
	}

	cout << "Faces read" << endl;
}

Mesh::~Mesh() {
	delete vertices;
	delete faces;
}

static void update_maxmin(const Vec3f &v, Vec3f &max, Vec3f &min)
{
	if (v[X] > max[X])
		max[X] = v[X];
	if (v[X] < min[X])
		min[X] = v[X];
	if (v[Y] > max[Y])
		max[Y] = v[Y];
	if (v[Y] < min[Y])
		min[Y] = v[Y];
	if (v[Z] > max[Z])
		max[Z] = v[Z];
	if (v[Z] < min[Z])
		min[Z] = v[Z];
}

static void get_normal(Face &f)
{
	Vec3f vec12 = *f.vertices[1] - *f.vertices[0];
	Vec3f vec13 = *f.vertices[2] - *f.vertices[0];

	f.normal = vec12.cross(vec13);
	f.normal.normalize();
}