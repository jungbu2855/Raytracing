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
void Ray::setCollisions(int _collisions) { collisions = _collisions; }
void Ray::setRefraction_index(float _refraction_index) { refraction_index = _refraction_index; }

// Functions

void Ray::attenuate(const Vec3f& to_pos) {
	float distance = this->getOrigin().distance(to_pos);

	float temp = this->getIntensity();
	temp /= 1.0 + 0.005 * pow(distance, 2) + 0.005 * distance;

	this->setIntensity(temp);
}

Ray Ray::reflect(const Face& face, const Vec3f &intersection_pos) const {
	Vec3f new_direction;
	float temp;

	//create new ray with new origin and new direction.
	temp = face.normal.dot(this->getDirection());
	new_direction = this->getDirection() - 2 * temp * face.normal;
	new_direction.normalize();

	Ray new_ray(intersection_pos, new_direction);

	//set the new intensity with attenuation and opacity value.
	Ray newray_temp(this->getOrigin(), this->getDirection(), this->getIntensity());
	newray_temp.attenuate(intersection_pos);

	new_ray.setIntensity(newray_temp.getIntensity() * face.material->getmirror());

	//add one to the collision number
	int new_collisions = this->getCollisions() + 1;
	new_ray.setCollisions(new_collisions);

	return new_ray;
}

Ray Ray::refract(const Face& face, const Vec3f &intersection_pos) const {
	Vec3f new_direction;
	float NI, n, refindex_prev, refindex_next;
	NI = this->getDirection().dot(face.normal);

	//calculate the relative refraction index
	refindex_prev = this->getRefraction_index();
	refindex_next = face.material->getrefraction_index();

	n = refindex_next / refindex_prev;

	//if the opacity of face.material is 1, this material has no tranmitted ray.
	if (face.material->getopacity() != 1) {
		float middle = 1 - (n * n) * (1 - NI * NI);

		if (middle < 0)
			return reflect(face, intersection_pos);
		new_direction = (n * NI - sqrt(middle)) * face.normal - n * this->getDirection();
		new_direction.normalize();
		Ray new_ray(intersection_pos, new_direction);

		//update the refraction_index for new_ray
		new_ray.setRefraction_index(refindex_next);

		//set the new intensity with attenuation and opacity value.
		Ray newray_temp(this->getOrigin(), this->getDirection(), this->getIntensity());
		newray_temp.attenuate(intersection_pos);

		new_ray.setIntensity(newray_temp.getIntensity() * (1 - face.material->getopacity()));

		//add one to the collision number
		int new_collisions = this->getCollisions() + 1;
		new_ray.setCollisions(new_collisions);

		return new_ray;
	}
	else
		return *this;
}