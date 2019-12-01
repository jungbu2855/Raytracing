#pragma once

#include "vec.h"
#include "material.h"

// [Vec/Mat][3/4]<float> Shortcuts
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;
typedef Mat3<float> Mat3f;
typedef Mat4<float> Mat4f;

// enums: vec3f[X] will returns vec3f[0], and so on.
enum Axis {
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};

enum Color {
	R = 0,
	G = 1,
	B = 2,
	A = 3
};

/* Representing a face: 3 vertices, face normal, and material properties.
 * Vertices and material properties should be stored by pointers to save memory. */
struct Face {
	Vec3f *vertices[3];		// CCW direction be front. Triangles only.
	Vec3f normal;
	Material *material;
};

/* Representing a point light: position, RGB color, and intensity. */
struct Light {
	Vec3f position;			// xyz coordinate		
	Vec3f color;			// RGB color
	float intensity = 1.0;	// light intensity, is 1.0 in default.

	// Easy initializer
	Light(float x, float y, float z, float r, float g, float b, float i)
		: position(Vec3f(x,y,z)), color(Vec3f(r,g,b)), intensity(i) {}
};

/* Representing a camera: its members are equivalent to the function parameters
 * of glLookAt(eye[3], center[3], up[3]) */
struct Camera {
	Vec3f position;		// eye position vector
	Vec3f center;		// the position which camera looking at
	Vec3f up;			// the up direction of the camera

	// Easy initializer
	Camera(float eyex, float eyey, float eyez,
		float centerx, float centery, float centerz,
		float upx, float upy, float upz)
		: position(Vec3f(eyex, eyey, eyez)),
		center(Vec3f(centerx, centery, centerz)),
		up(Vec3f(upx, upy, upz)) {}
};