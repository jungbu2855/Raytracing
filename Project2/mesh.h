#pragma once
#include "vec.h"
#include "material.h"
#include "definitions.h"

/* Mesh class loads a triangular mesh from the .off formatted
 * file, computes the face normals, and fetches material property. */
class Mesh {
private:
	Vec3f *vertices;	// (v1, v2, v3) in CCW direction
	Face *faces;		// (coords, normals, *material prop)
	int mesh_size;		// number of faces in the mesh
	int mesh_dim;		// Maximum length among x, y, z direction dimensions.
	Material material;	// Material property
public:
	// Constructor: Mesh file read & loader. It does everything needed.
	Mesh(const char *filename, const Material &mat, int dim = 1);
	~Mesh();
};