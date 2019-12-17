#pragma once
#include "vec.h"
#include "material.h"
#include "definitions.h"

/* Mesh class loads a triangular mesh from the .off formatted
 * file, computes the face normals, and fetches material property. */
class Mesh {
public:
	enum Shape {
		TRIANGLE,
		SQUARE,
		CUBE,
		SPHERE
	};
private:
	Vec3d *vertices;	// All vertices it contains
	Face *faces;		// (coords, normals, *material prop)
	int mesh_size;		// number of faces in the mesh
	double mesh_dim;		// Maximum length among x, y, z direction dimensions.
	Material material;	// Material property
public:
	// Constructor: Mesh file read & loader. It does everything needed.
	Mesh(const char *filename, const Material &mat, const Mat4d &_model, double dim = 1);
	Mesh(Shape shape, const Material &mat, const Mat4d &_model, double dim = 1);
	~Mesh();

	// Getters
	int get_size() { return mesh_size; }
	int get_dimension() { return mesh_dim; }
	const Material *get_material() { return &material; }
	const Face *get_const_faces() { return faces; }
	Face *get_faces() { return faces; }

private:
	void offFileLoader(const char *filename, const Mat4d &_model);
};