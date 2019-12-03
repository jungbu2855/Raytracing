#pragma once
#include "vec.h"

typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

class Material {
private:
	Vec4f color;			// RGBA (RGB + opacity) in [0,1]^4
	float refraction_index;

public:
	//constructor
	Material() : color(Vec4f()), refraction_index(0) {};
	Material(Vec4f _color) : refraction_index(0) { this->color = _color; };
	Material(float _refration_index) : color(Vec4f()) { this->refraction_index = _refration_index; };
	Material(Vec4f _color, float _refration_index) { this->color = _color; this->refraction_index = _refration_index; };
	Material(const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
	}

	//Assignment 
	Material& operator= (const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
		return *this;
	}

	//getter and setter function
	Vec3f getcolor() { return this->color; }
	float getrefraction_index() { return this->refraction_index; }
	float getopaque() { return this->color[3]; }

	void setcolor(Vec3f _color) { this->color = _color; }
	void setrefraction_index(float _refraction_index) { this->refraction_index = _refraction_index; }
	void setopaque(float _opaque) { this->color[3] = _opaque; }
};