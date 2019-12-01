#pragma once
#include "vec.h"

typedef Vec3<float> Vec3f;

class Material {
	// Todo
private:
	Vec3f color;
	float refraction_index;
	bool opaque;		//�� ��ü�� ���������� �Ǵ��ϴ� �ǵ�, refraction_index�� 0�̸� �������ΰɷ� �ص� �ɰ� �����ϴ�.
		

public:
	//constructor
	Material() : color(Vec3f()), refraction_index(0), opaque(true) {};
	Material(Vec3f _color) : refraction_index(0), opaque(true) { this->color = _color; };
	Material(float _refration_index) : color(Vec3f()), opaque(true) { this->refraction_index = _refration_index; };
	Material(bool _opaque) : color(Vec3f()), refraction_index(0) { this->opaque = _opaque; };
	Material(Vec3f _color, float _refration_index) : opaque(true) { this->color = _color; this->refraction_index = _refration_index; };
	Material(Vec3f _color, bool _opaque) : refraction_index(0) { this->color = _color; this->opaque = _opaque; };
	Material(float _refraction_index, bool _opaque) : color(Vec3f()) { this->refraction_index = _refraction_index; this->opaque = _opaque; };
	Material(Vec3f _color, float _refraction_index, bool _opaque) {this->color = _color; this->refraction_index = _refraction_index; this->opaque = _opaque; };
	Material(const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
		this->opaque = _material.opaque;
	}

	//Assignment 
	Material& operator= (const Material& _material) {
		this->color = _material.color;
		this->refraction_index = _material.refraction_index;
		this->opaque = _material.opaque;
	}

	//getter and setter function
	Vec3f getcolor() { return this->color; }
	float getrefraction_index() { return this->refraction_index; }
	bool getopaque() { return this->opaque; }

	void setcolor(Vec3f _color) { this->color = _color; }
	void setrefraction_index(float _refraction_index) { this->refraction_index = _refraction_index; }
	void setopaque(bool _opaque) { this->opaque = _opaque; }

	//Ȥ�� �길 ������ �ϴ� �޼ҵ尡 �ֳ���? �� ray���� ���̴� Ư���鸸 ������ �ȴ� �����ϰ� �־ ������ �ʾ�����, �����صνŰ� �ִٸ� ������ �ּ���.
};