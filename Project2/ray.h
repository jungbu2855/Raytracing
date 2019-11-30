#pragma once
#include "vec.h"
#include "definitions.h"

#ifndef MAX_RAY_DEPTH
	#define MAX_RAY_DEPTH 5
#endif

class Ray {
private:
	Vec3f origin;			// Ray origin
	Vec3f direction;		// Ray direction vector
	Vec3f color;			// Ray color, RGB on [0,1]^3
	float intensity;		// Ray intensity, on [0,infinity), default is 1 for light sources, 0 for others
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

	// Getters, Setters

	Vec3f getOrigin() const { return origin; }
	Vec3f getDirection() const { return direction; }
	Vec3f getColor() const { return color; }
	float getIntensity() const { return intensity; }
	int getCollisions() const { return collisions; }

	void setOrigin(const Vec3f &_origin) { origin = _origin; }
	void setDirection(const Vec3f &_direction) { direction = _direction; }
	void setColor(const Vec3f &_color) { color = _color; }
	void setIntensity(float _intensity) { intensity = _intensity; }
	void setCollisions(int _collisions) { collisions = collisions; }

	// Functions

	// Attenuation
	void attenuate(const Vec3f &to_pos);

	// Generating a refected ray
	Ray reflect(const Ray &incident, const Face &face) const;

	// Generating a refracted ray
	Ray refract(const Ray &incident, const Face &face) const;

	// Generating a shadow ray
	Ray shadow(const Ray &incident, const Light &light) const;
};