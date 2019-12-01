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
	float intensity;		// Ray intensity, on [0,infinity), default is 1 for light sources, 0 for others
	int   collisions;		// Number of collisions went through
	float refraction_index;	// 현재 빛이 지나고 있는 매질의 굴절율
public:
	// Constructors
	Ray();
	Ray(Vec3f _origin, Vec3f _direction);
	Ray(Vec3f _origin, Vec3f _direction, float _intensity);
	Ray(Vec3f _origin, Vec3f _direction, float _intensity, int _collisions);
	Ray(const Ray& _ray);

	// Operators

	// Assignment
	Ray& operator= (const Ray& _ray);

	// Getters, Setters

	Vec3f getOrigin() const;
	Vec3f getDirection() const;
	float getIntensity() const;
	int getCollisions() const;
	float getRefraction_index() const;

	void setOrigin(const Vec3f& _origin);
	void setDirection(const Vec3f& _direction);
	void setIntensity(float _intensity);
	void setCollisions(int _collisions);
	void setRefraction_index(float _refraction_index);

	// Functions

	// Attenuation
	void attenuate(const Vec3f& to_pos);

	// Generating a refected ray
	Ray reflect(const Face& face, const Vec3f &intersection_pos) const;

	// Generating a refracted ray
	Ray refract(const Face& face, const Vec3f &intersection_pos) const;
};