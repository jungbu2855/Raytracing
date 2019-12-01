#pragma once
#include "ray.h"
#include "definitions.h"

// Constructors
Ray::Ray() : origin(Vec3f()), direction(Vec3f()), intensity(0), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction) : origin(Vec3f(_origin)), direction(_direction), intensity(0), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction, float _intensity) : origin(Vec3f(_origin)), direction(_direction), intensity(_intensity), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction, float _intensity, int _collisions) : origin(Vec3f(_origin)), direction(_direction), intensity(_intensity), collisions(_collisions), refraction_index(1) {}
Ray::Ray(const Ray& _ray) {
	this->origin = _ray.origin;
	this->direction = _ray.direction;
	this->intensity = _ray.intensity;
	this->collisions = _ray.collisions;
	this->refraction_index = _ray.refraction_index;
}

// Operators

// Assignment
Ray& Ray::operator= (const Ray& _ray) {
	this->origin = _ray.origin;
	this->direction = _ray.direction;
	this->intensity = _ray.intensity;
	this->collisions = _ray.collisions;
	this->refraction_index = _ray.refraction_index;
	return *this;
}

// Getters, Setters

Vec3f Ray::getOrigin() const { return origin; }
Vec3f Ray::getDirection() const { return direction; }
float Ray::getIntensity() const { return intensity; }
int Ray::getCollisions() const { return collisions; }
float Ray::getRefraction_index() const { return refraction_index; }

void Ray::setOrigin(const Vec3f& _origin) { origin = _origin; }
void Ray::setDirection(const Vec3f& _direction) { direction = _direction; }
void Ray::setIntensity(float _intensity) { intensity = _intensity; }
void Ray::setCollisions(int _collisions) { collisions = collisions; }
void Ray::setRefraction_index(float _refraction_index) { refraction_index = _refraction_index; }

// Functions

Ray Ray::reflect(const Face& face, const Vec3f &intersection_pos) const {
	Vec3f new_direction;
	float temp;
	temp = face.normal.dot(this->getDirection());
	new_direction = this->getDirection() - 2 * temp * face.normal;

	Ray new_ray(intersection_pos, new_direction);

	Vec3f new_color, black(0, 0, 0);

	if (new_color == black) {
		new_color = face.material->getcolor();
	}
	else
		new_color = (new_color + face.material->getcolor()) / 2;

	return new_ray;
}

Ray Ray::refract(const Face& face, const Vec3f &intersection_pos) const {
	Vec3f new_direction;
	float NI, n, refindex_prev, refindex_next;
	NI = this->getDirection().dot(face.normal);

	refindex_prev = this->getRefraction_index();
	refindex_next = face.material->getrefraction_index();

	n = refindex_next / refindex_prev;


	if (!face.material->getopaque()) {
		float middle = 1 - (n * n) * (1 - NI * NI);
		new_direction = (n * NI - sqrt(middle)) * face.normal - n * this->getDirection();

		Ray new_ray(intersection_pos, new_direction);

		//refraction_index 추가
		new_ray.setRefraction_index(refindex_next);

		//여기도 색혼합 추가해야되네요. 일단 시간이 없어서 ㅠ
		return new_ray;
	}
	else
		return *this;
}