#pragma once
#include "vec.h"
#include "material.h"
#include "definitions.h"

/* Mesh class loads a triangular mesh from the .off formatted
 * file, computes the face normals, and fetches material property. */
class Mesh {
private:
	Face *faces;
	int mesh_size;
	Material material;
public:
	Mesh(char *filename);
};