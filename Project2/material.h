#pragma once
#include "vec.h"

typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

class Material {
private:
	Vec4f color;			// RGBA (RGB + opacity) in [0,1]^4
	float refraction_index;
	float mirror;

public:
	//constructor
	Material() : color(Vec4f()), refraction_index(0), mirror(1) {};
	Material(Vec4f _color) : refraction_index(0), mirror(1) { this->color = _color; };
	Material(float _refration_index) : color(Vec4f()), mirror(1) { this->refraction_index = _refration_index; };
	Material(Vec4f _color, float _refration_index) { this->color = _color; this->refraction_index = _refration_index; };
	Material(Vec4f _color, float _refration_index, float _mirror) { this->color = _color; this->refraction_index = _refration_index; this->mirror = _mirror; };
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
	Vec3f getcolor() { return this->color; }
	float getrefraction_index() { return this->refraction_index; }
	float getopacity() { return this->color[3]; }
	float getmirror() { return this->mirror; }

	void setcolor(Vec3f _color) { this->color = _color; }
	void setrefraction_index(float _refraction_index) { this->refraction_index = _refraction_index; }
	void setopacity(float _opacity) { this->color[3] = _opacity; }
	void setmirror(float _mirror) { this->mirror = _mirror; }
};