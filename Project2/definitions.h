#pragma once

#include "vec.h"
#include "material.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846f
#endif

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

/* Representing a camera. Sets view and projection transform parameters.
 * Refer to: gluLookAt(eye[3], center[3], up[3]),
 *           gluPerspective(fovy, aspect, zNear, zFar)                  */
struct Camera {
	/* Camera position and direction */
	Vec3f position;		// eye position vector
	Vec3f center;		// the position which camera looking at
	Vec3f up;			// the up direction of the camera
	
	/* Camera projection properties */
	float height;		// projected image height
	float fovy;			// y-directed view angle (in radians)
	float aspect_ratio;	// width / height ratio
	float zNear, zFar;	// near and far clipping planes

	// Easy initializer
	Camera() {}
	Camera(float eyex, float eyey, float eyez,
		float centerx, float centery, float centerz,
		float upx, float upy, float upz,
		float _height, float _fovy_in_degree, float _aspect_ratio,
		float _zNear, float _zFar)
		: position(Vec3f(eyex, eyey, eyez)),
		center(Vec3f(centerx, centery, centerz)),
		up(Vec3f(upx, upy, upz)),
		height(_height), fovy(_fovy_in_degree * M_PI / 180.0f),
		aspect_ratio(_aspect_ratio), zNear(_zNear), zFar(_zFar) {}
};