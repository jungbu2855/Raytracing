#pragma once
#include "vec.h"

typedef Vec3<float> Vec3f;

class Ray {
private:
	Vec3f origin;			// Ray origin
	Vec3f direction;		// Ray direction vector
	Vec3f color;			// Ray color, RGB on [0,1]^3
	float intensity;		// Ray intensity, on [0,1]
	int   collisions;		// Number of collisions went through
public:
	// Constructors
	Ray() : origin(Vec3f()), direction(Vec3f()), color(Vec3f()), intensity(0), collisions(0) {}
	Ray(Vec3f _origin, Vec3f _direction) : origin(Vec3f()), direction(_direction), color(Vec3f()), intensity(0), collisions(0) {}
	Ray(Vec3f _origin, Vec3f _direction, Vec3f _color, float _intensity) : origin(Vec3f()), direction(_direction), color(_color), intensity(_intensity), collisions(0) {}
	Ray(Vec3f _origin, Vec3f _direction, Vec3f _color, float _intensity, int _collisions) : origin(Vec3f()), direction(_direction), color(_color), intensity(_intensity), collisions(_collisions) {}
	Ray(const Ray &_ray) {
		this->origin = _ray.origin;
		this->direction = _ray.direction;
		this->color = _ray.color;
		this->intensity = _ray.intensity;
		this->collisions = _ray.collisions;
	}

	// Operators

	// Assignment
	Ray &operator= (const Ray &_ray) {
		this->origin = _ray.origin;
		this->direction = _ray.direction;
		this->color = _ray.color;
		this->intensity = _ray.intensity;
		this->collisions = _ray.collisions;
	}
	
	/* Light addition
	 * It merges two rays, with weighted averaging. */
	const Ray &operator+ (const Ray &_right) const {
		
	}
	
	// Functions

};