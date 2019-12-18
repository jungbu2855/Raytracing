#include "mesh.h"
#include "definitions.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

static double INF = 1000;

static void update_maxmin(const Vec3d &v, Vec3d &max, Vec3d &min);
static void get_normal(Face &f);

Mesh::Mesh(const char *filename, const Material &mat, const Mat4d &_model, double dim)
	: mesh_dim(dim), material(mat) {
	offFileLoader(filename, _model);
}

/* Simple-shape mesh loader */
/* 웬만하면 유저가 model matrix로 직관적으로 조작할 수 있도록 가로*세로*깊이 모두 1로 맞춰주시고 0,0,0을 중심점으로, z축에 평행하거나 z축 위에 있도록 만들어주세요. */
Mesh::Mesh(Shape shape, const Material &mat, const Mat4d &_model, double dim)
	: mesh_dim(dim), material(mat) {
	// Model transformation matrix
	Mat4d model = _model * scale(dim);

	switch (shape) {
	case TRIANGLE:
		this->mesh_size = 1;

		vertices = new Vec3d[3];
		faces = new Face[mesh_size];

		double point1;
		point1 = sqrt(2) / 2;

		this->vertices[0] = Vec3d(-0.5, 0, point1);
		this->vertices[1] = Vec3d(0.5, 0, point1);
		this->vertices[2] = Vec3d(0, 0, -point1);

		for (int i = 0; i < 3; i++)
			vertices[i] = model * vertices[i];

		for (int i = 0; i < 4; i++) {
			faces[0].vertices[i] = &vertices[i];
		}
		get_normal(faces[0]);
		faces[0].material = &material;

		break;

	case SQUARE:
		this->mesh_size = 2;

		vertices = new Vec3d[4];
		faces = new Face[mesh_size];

		this->vertices[0] = Vec3d(0.5, 0, 0.5);
		this->vertices[1] = Vec3d(-0.5, 0, 0.5);
		this->vertices[2] = Vec3d(-0.5, 0, -0.5);
		this->vertices[3] = Vec3d(0.5, 0, -0.5);

		for (int i = 0; i < 4; i++)
			vertices[i] = model * vertices[i];

		faces[0].vertices[0] = &vertices[0];
		faces[0].vertices[1] = &vertices[2];
		faces[0].vertices[2] = &vertices[1];

		faces[1].vertices[0] = &vertices[0];
		faces[1].vertices[1] = &vertices[3];
		faces[1].vertices[2] = &vertices[2];

		for (int i = 0; i < 2; i++) {
			get_normal(faces[i]);
			faces[i].material = &material;
		}

		break;

	case CUBE:

		this->mesh_size = 12;

		vertices = new Vec3d[8];
		faces = new Face[mesh_size];

		this->vertices[0] = Vec3d(0.5, 0.5, -0.5);
		this->vertices[1] = Vec3d(0.5, 0.5, 0.5);
		this->vertices[2] = Vec3d(-0.5, 0.5, 0.5);
		this->vertices[3] = Vec3d(-0.5, 0.5, -0.5);
		this->vertices[4] = Vec3d(0.5, -0.5, -0.5);
		this->vertices[5] = Vec3d(0.5, -0.5, 0.5);
		this->vertices[6] = Vec3d(-0.5, -0.5, 0.5);
		this->vertices[7] = Vec3d(-0.5, -0.5, -0.5);

		for (int i = 0; i < 8; i++)
			vertices[i] = model * vertices[i];

		faces[0].vertices[0] = &vertices[0];
		faces[0].vertices[1] = &vertices[1];
		faces[0].vertices[2] = &vertices[3];

		faces[1].vertices[0] = &vertices[0];
		faces[1].vertices[1] = &vertices[3];
		faces[1].vertices[2] = &vertices[4];

		faces[2].vertices[0] = &vertices[0];
		faces[2].vertices[1] = &vertices[1];
		faces[2].vertices[2] = &vertices[4];

		faces[3].vertices[0] = &vertices[1];
		faces[3].vertices[1] = &vertices[2];
		faces[3].vertices[2] = &vertices[3];

		faces[4].vertices[0] = &vertices[1];
		faces[4].vertices[1] = &vertices[2];
		faces[4].vertices[2] = &vertices[6];

		faces[5].vertices[0] = &vertices[2];
		faces[5].vertices[1] = &vertices[3];
		faces[5].vertices[2] = &vertices[6];

		faces[6].vertices[0] = &vertices[1];
		faces[6].vertices[1] = &vertices[4];
		faces[6].vertices[2] = &vertices[5];

		faces[7].vertices[0] = &vertices[1];
		faces[7].vertices[1] = &vertices[5];
		faces[7].vertices[2] = &vertices[6];

		faces[8].vertices[0] = &vertices[4];
		faces[8].vertices[1] = &vertices[5];
		faces[8].vertices[2] = &vertices[6];

		faces[9].vertices[0] = &vertices[3];
		faces[9].vertices[1] = &vertices[4];
		faces[9].vertices[2] = &vertices[7];

		faces[10].vertices[0] = &vertices[3];
		faces[10].vertices[1] = &vertices[6];
		faces[10].vertices[2] = &vertices[7];

		faces[11].vertices[0] = &vertices[4];
		faces[11].vertices[1] = &vertices[6];
		faces[11].vertices[2] = &vertices[7];

		for (int i = 0; i < 12; i++) {
			get_normal(faces[i]);
			faces[i].material = &material;
		}

		break;

	case SPHERE:
		offFileLoader("sphere.off", _model);
		break;
	default:
		;
	}
}

Mesh::~Mesh() {
	delete vertices;
	delete faces;
}

static void update_maxmin(const Vec3d &v, Vec3d &max, Vec3d &min)
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
	Vec3d vec12 = *f.vertices[1] - *f.vertices[0];
	Vec3d vec13 = *f.vertices[2] - *f.vertices[0];

	f.normal = vec12.cross(vec13);
	f.normal.normalize();
}

void Mesh::offFileLoader(const char *filename, const Mat4d &_model) {
	FILE *f;
	char buf[64];
	int nv, nf;

	// Open file
	ifstream fileio(filename, ios::in);
	assert(fileio.is_open());

	// 1. First line should contain OFF only
	fileio >> buf;
	assert(strcmp(buf, "OFF") == 0);

	// 2. Second line should contain total number of v, f, and (# of edges)
	fileio >> nv >> nf >> buf;
	assert(nv >= 0 && nf >= 0);

	// 2+. Try to allocate mem space
	vertices = new Vec3d[nv];
	faces = new Face[nf];
	assert(vertices != 0 && faces != 0);
	mesh_size = nf;

	// 3. Vertex Coordinates
	Vec3d v_max = { -INF, -INF, -INF };
	Vec3d v_min = { INF, INF, INF };

	// 3-1. Read the positions (w/ finding max/min)
	for (int i = 0; i < nv; ++i)
	{
		fileio >> (vertices[i][X]) >> (vertices[i][Y]) >> (vertices[i][Z]);
		update_maxmin(vertices[i], v_max, v_min);
	}

	// 3-2. Tune the vertices to be centered, properly sized
	Vec3d center = { (v_max[X] + v_min[X]) / 2,
		(v_max[Y] + v_min[Y]) / 2, (v_max[Z] + v_min[Z]) / 2 };
	double mulfact;
	if (v_max[X] - v_min[X] > v_max[Y] - v_min[Y]
		&& v_max[X] - v_min[X] > v_max[Z] - v_min[Z])
		mulfact = mesh_dim / (v_max[X] - v_min[X]);
	else if (v_max[Y] - v_min[Y] > v_max[Z] - v_min[Z])
		mulfact = mesh_dim / (v_max[Y] - v_min[Y]);
	else
		mulfact = mesh_dim / (v_max[Z] - v_min[Z]);

	Mat4d model = _model * scale(mulfact) * translate(-center);
	for (int i = 0; i < nv; ++i)
	{
		vertices[i] = model * vertices[i];
	}

	// 4. Face Coordinates (Only triangular faces)
	for (int i = 0; i < nf; ++i)
	{
		int idx1, idx2, idx3;
		fileio >> buf >> idx1 >> idx2 >> idx3;
		faces[i].vertices[0] = vertices + idx1;
		faces[i].vertices[1] = vertices + idx2;
		faces[i].vertices[2] = vertices + idx3;

		// get normal
		get_normal(faces[i]);

		faces[i].material = &material;
	}

	fileio.close();

}