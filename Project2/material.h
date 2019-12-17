#pragma once
#include "vec.h"

typedef Vec3<double> Vec3d;
typedef Vec4<double> Vec4d;

class Material {
private:
	Vec4d color;			// RGBA (RGB + opacity) in [0,1]^4
	double refraction_index;
	double mirror;

public:
	//constructor
	Material() : color(Vec4d()), refraction_index(0), mirror(1) {};
	Material(Vec4d _color) : refraction_index(0), mirror(1) { this->color = _color; };
	Material(double _refration_index) : color(Vec4d()), mirror(1) { this->refraction_index = _refration_index; };
	Material(Vec4d _color, double _refration_index) { this->color = _color; this->refraction_index = _refration_index; };
	Material(Vec4d _color, double _refration_index, double _mirror) { this->color = _color; this->refraction_index = _refration_index; this->mirror = _mirror; };
	Material(const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
		this->mirror = _material.mirror;
	}

	//Assignment 
	Material& operator= (const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
		this->mirror = _material.mirror;
		return *this;
	}

	//getter and setter function
	Vec3d getcolor() { return this->color; }
	double getrefraction_index() { return this->refraction_index; }
	double getopacity() { return this->color[3]; }
	double getmirror() { return this->mirror; }

	void setcolor(Vec3d _color) { this->color = _color; }
	void setrefraction_index(double _refraction_index) { this->refraction_index = _refraction_index; }
	void setopacity(double _opacity) { this->color[3] = _opacity; }
	void setmirror(double _mirror) { this->mirror = _mirror; }
};