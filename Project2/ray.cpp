#pragma once
#include "ray.h"
#include "definitions.h"

// Constructors
Ray::Ray() : origin(Vec3f()), direction(Vec3f()), color(Vec3f()), intensity(0), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction) : origin(Vec3f(_origin)), direction(_direction), color(Vec3f()), intensity(0), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction, Vec3f _color, float _intensity) : origin(Vec3f(_origin)), direction(_direction), color(_color), intensity(_intensity), collisions(0), refraction_index(1) {}
Ray::Ray(Vec3f _origin, Vec3f _direction, Vec3f _color, float _intensity, int _collisions) : origin(Vec3f(_origin)), direction(_direction), color(_color), intensity(_intensity), collisions(_collisions), refraction_index(1) {}
Ray::Ray(const Ray& _ray) {
	this->origin = _ray.origin;
	this->direction = _ray.direction;
	this->color = _ray.color;
	this->intensity = _ray.intensity;
	this->collisions = _ray.collisions;
	this->refraction_index = _ray.refraction_index;
}

// Operators

// Assignment
Ray& Ray::operator= (const Ray& _ray) {
	this->origin = _ray.origin;
	this->direction = _ray.direction;
	this->color = _ray.color;
	this->intensity = _ray.intensity;
	this->collisions = _ray.collisions;
	this->refraction_index = _ray.refraction_index;
	return *this;
}

// Getters, Setters

Vec3f Ray::getOrigin() const { return origin; }
Vec3f Ray::getDirection() const { return direction; }
Vec3f Ray::getColor() const { return color; }
float Ray::getIntensity() const { return intensity; }
int Ray::getCollisions() const { return collisions; }
float Ray::getRefraction_index() const { return refraction_index; }

void Ray::setOrigin(const Vec3f& _origin) { origin = _origin; }
void Ray::setDirection(const Vec3f& _direction) { direction = _direction; }
void Ray::setColor(const Vec3f& _color) { color = _color; }
void Ray::setIntensity(float _intensity) { intensity = _intensity; }
void Ray::setCollisions(int _collisions) { collisions = collisions; }
void Ray::setRefraction_index(float _refraction_index) { refraction_index = _refraction_index; }

// Functions

Ray Ray::reflect(const Ray& incident, const Face& face, const Vec3f ret_vec) const {
	Vec3f new_direction;
	float temp;
	temp = face.normal.dot(incident.getDirection());
	new_direction = incident.getDirection() - 2 * temp * face.normal;

	Ray new_ray(ret_vec, new_direction);

	Vec3f new_color, black(0, 0, 0);

	if (new_color == black) {
		new_color = face.material->getcolor();
	}
	else
		new_color = (new_color + face.material->getcolor()) / 2;

	return new_ray;
}

Ray Ray::refract(const Ray& incident, const Face& face, const Vec3f ret_vec) const {
	Vec3f new_direction;
	float NI, n, refindex_prev, refindex_next;
	NI = incident.getDirection().dot(face.normal);

	refindex_prev = incident.getRefraction_index();
	refindex_next = face.material->getrefraction_index();

	n = refindex_next / refindex_prev;


	if (!face.material->getopaque()) {
		float middle = 1 - (n * n) * (1 - NI * NI);
		new_direction = (n * NI - sqrt(middle)) * face.normal - n * incident.getDirection();

		Ray new_ray(ret_vec, new_direction);

		//refraction_index 추가
		new_ray.setRefraction_index(refindex_next);

		//여기도 색혼합 추가해야되네요. 일단 시간이 없어서 ㅠ
		return new_ray;
	}
	else
		return incident;
}