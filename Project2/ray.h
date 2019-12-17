#pragma once
#include "vec.h"
#include "definitions.h"

class Ray {
private:
	Vec3d origin;			// Ray origin
	Vec3d direction;		// Ray direction vector
	double intensity;		// Ray intensity, on [0,infinity), default is 1 for light sources, 0 for others
	int   collisions;		// Number of collisions went through
	double refraction_index;	// 현재 빛이 지나고 있는 매질의 굴절율
public:
	// Constructors
	Ray();
	Ray(Vec3d _origin, Vec3d _direction);
	Ray(Vec3d _origin, Vec3d _direction, double _intensity);
	Ray(Vec3d _origin, Vec3d _direction, double _intensity, int _collisions);
	Ray(const Ray& _ray);

	// Operators

	// Assignment
	Ray& operator= (const Ray& _ray);

	// Getters, Setters

	Vec3d getOrigin() const;
	Vec3d getDirection() const;
	double getIntensity() const;
	int getCollisions() const;
	double getRefraction_index() const;

	void setOrigin(const Vec3d& _origin);
	void setDirection(const Vec3d& _direction);
	void setIntensity(double _intensity);
	void setCollisions(int _collisions);
	void setRefraction_index(double _refraction_index);

	// Functions

	// Attenuation
	void attenuate(const Vec3d& to_pos);

	// Generating a refected ray
	Ray reflect(const Face& face, const Vec3d &intersection_pos) const;

	// Generating a refracted ray
	Ray refract(const Face& face, const Vec3d &intersection_pos) const;
};