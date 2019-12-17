#pragma once

#include "vec.h"
#include "material.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#ifndef INFTY
	#define INFTY 65535
#endif

// [Vec/Mat][3/4]<double> Shortcuts
typedef Vec3<double> Vec3d;
typedef Vec4<double> Vec4d;
typedef Mat3<double> Mat3d;
typedef Mat4<double> Mat4d;

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
	Vec3d *vertices[3];		// CCW direction be front. Triangles only.
	Vec3d normal;
	Material *material;
};

/* Representing a point light: position, RGB color, and intensity. */
struct Light {
	Vec3d position;			// xyz coordinate		
	Vec4d color;			// RGBI color + intensity

	// Easy initializer
	Light(double x, double y, double z, double r, double g, double b, double i)
		: position(Vec3d(x,y,z)), color(Vec4d(r,g,b,i)) {}
};

/* Representing a camera. Sets view and projection transform parameters.
 * Refer to: gluLookAt(eye[3], center[3], up[3]),
 *           gluPerspective(fovy, aspect, zNear, zFar)                  */
struct Camera {
	/* Camera position and direction */
	Vec3d position;		// eye position vector
	Vec3d center;		// the position which camera looking at
	Vec3d up;			// the up direction of the camera
	
	/* Camera projection properties */
	double height;		// projected image height
	double fovy;			// y-directed view angle (in radians)
	double aspect_ratio;	// width / height ratio
	double zNear, zFar;	// near and far clipping planes

	// Easy initializer
	Camera() {}
	Camera(double eyex, double eyey, double eyez,
		double centerx, double centery, double centerz,
		double upx, double upy, double upz,
		double _height, double _fovy_in_degree, double _aspect_ratio,
		double _zNear, double _zFar)
		: position(Vec3d(eyex, eyey, eyez)),
		center(Vec3d(centerx, centery, centerz)),
		up(Vec3d(upx, upy, upz)),
		height(_height), fovy(_fovy_in_degree * M_PI / 180.0f),
		aspect_ratio(_aspect_ratio), zNear(_zNear), zFar(_zFar) {}
};