#pragma once

#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

enum Axis {
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};

enum Color {
	R = 0,
	G = 1,
	B = 2,
	A = 3
};

template <typename T>
class Vec4 {
private:
	T e[4];
public:
	// Constructors
	Vec4() { for (int i = 0; i < 4; i++) e[i] = T(0); }
	Vec4(T val) { for (int i = 0; i < 4; i++) e[i] = val; }
	Vec4(T _x, T _y, T _z, T _w) { e[0] = _x; e[1] = _y; e[2] = _z; e[3] = _w; }
	Vec4(T v[4]) { for (int i = 0; i < 4; i++) e[i] = v[i]; }
	template <typename U> Vec4(Vec4<U> v) { for (int i = 0; i < 4; i++) this->e[i] = T(v.e[i]); }
	
	// Getter, Setters
	T get(int i) { return e[i]; }
	const T *getv() { return e; }
	void set(int i, T _e) { e[i] = _e; }
	void setv(T _e[]) { for (int i = 0; i < 4; i++) e[i] = _e[i]; }

	// Operators
	// available: [], =, +, -, *(scalar), +=, -=, *=(scalar), <<
	T &operator[] (int i) {
		assert(i >= 0 && i < 4);
		return this->e[i];
	}
	const T &operator[] (int i) const {
		assert(i >= 0 && i < 4);
		return this->e[i];
	}
	Vec4<T> &operator= (const Vec4<T> &right){							// =
		for (int i = 0; i < 4; i++)
			this->e[i] = right->e[i];
		return *this;
	}
	const Vec4<T> operator+ () const { return Vec4<T>(*this); }			// +
	const Vec4<T> operator+ (const Vec4<T> &right) const {
		return Vec4<T>(this->e[0] + right.e[0], this->e[1] + right.e[1],
			this->e[2] + right.e[2], this->e[3] + right.e[3]);
	}
	const Vec4<T> operator- () const {									// -
		return Vec4<T>(-e[0], -e[1], -e[2], -e[3]);
	}
	const Vec4<T> operator- (const Vec4<T> &right) const {
		return Vec4<T>(this->e[0] - right.e[0], this->e[1] - right.e[1],
			this->e[2] - right.e[2], this->e[3] - right.e[3]);
	}
	const Vec4<T> operator* (T c) const {								// scalar *
		return Vec4<T>(c*e[0], c*e[1], c*e[2], c*e[3]);
	}
	friend const Vec4<T> operator* (T c, const Vec4<T> &right) {
		return Vec4<T>(c*right.e[0], c*right.e[1], c*right.e[2], c*right.e[3]);
	}
	Vec4<T> &operator+= (const Vec4<T> &right) {						// +=
		for (int i = 0; i < 4; i++)
			this->e[i] += right.e[i];
		return *this;
	}
	Vec4<T> &operator-= (const Vec4<T> &right) {						// -=
		for (int i = 0; i < 4; i++)
			this->e[i] -= right.e[i];
		return *this;
	}
	Vec4<T> &operator*= (T c) {											// *= (scalar)
		for (int i = 0; i < 4; i++)
			this->e[i] *= c;
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Vec4<T> &right) {	// <<
		os << "[ ";
		for (int i = 0; i < 4; i++)
			os << right.e[i] << " ";
		os << "]";
		return os;
	}
	bool operator== (const Vec4<T> &right) const {						// relational
		return this->e[0] == right.e[0] && this->e[1] == right.e[1]
			&& this->e[2] == right.e[2] && this->e[3] == right.e[3];
	}
	bool operator!= (const Vec4<T> &right) const { return !((*this) == right); }
	bool operator<  (const Vec4<T> &right) const { return this->norm() < right.norm(); }
	bool operator<= (const Vec4<T> &right) const { return ((*this) == right) || ((*this) < right); }
	bool operator>  (const Vec4<T> &right) const { return !((*this) <= right); }
	bool operator>= (const Vec4<T> &right) const { return !((*this) < right); }

	// Functions
	// available: dot(), cross(), norm(), normalize()

	// Dot product
	const T dot(const Vec4<T> &right) const {
		T ret = 0;
		for (int i = 0; i < 4; i++)
			ret += this->e[i] * right.e[i];
		return ret;
	}

	// Norm || v ||
	const T norm() const {
		return sqrt(this->dot(*this));
	}

	// Normalize
	Vec4<T> &normalize() {
		T norm_inv = 1 / (this->norm());
		return ((*this) *= norm_inv);
	}
};

template <typename T>
class Vec3 {
private:
	T e[3];
public:
	// Constructors
	Vec3() { for (int i = 0; i < 3; i++) e[i] = T(0); }
	Vec3(T val) { for (int i = 0; i < 3; i++) e[i] = val; }
	Vec3(T _x, T _y, T _z) { e[0] = _x; e[1] = _y; e[2] = _z; }
	Vec3(T v[3]) { for (int i = 0; i < 3; i++) e[i] = v[i]; }
	template <typename U> Vec3(Vec3<U> v) { for (int i = 0; i < 3; i++) this->e[i] = T(v.e[i]); }
	
	// Getter, Setters
	T get(int i) { return e[i]; }
	const T *getv() { return e; }
	void set(int i, T _e) { e[i] = _e; }
	void setv(T _e[]) { for (int i = 0; i < 3; i++) e[i] = _e[i]; }

	// Operators
	// available: [], =, +, -, *(scalar), +=, -=, *=(scalar), <<
	T &operator[] (int i) {
		assert(i >= 0 && i < 3);
		return this->e[i];
	}
	const T &operator[] (int i) const {
		assert(i >= 0 && i < 3);
		return this->e[i];
	}
	Vec3<T> &operator= (const Vec3<T> &right){							// =
		for (int i = 0; i < 3; i++)
			this->e[i] = right->e[i];
		return *this;
	}
	const Vec3<T> operator+ () const { return Vec3<T>(*this); }			// +
	const Vec3<T> operator+ (const Vec3<T> &right) const {
		return Vec3<T>(this->e[0] + right.e[0], this->e[1] + right.e[1],
			this->e[2] + right.e[2]);
	}
	const Vec3<T> operator- () const {									// -
		return Vec3<T>(-e[0], -e[1], -e[2]);
	}
	const Vec3<T> operator- (const Vec3<T> &right) const {
		return Vec3<T>(this->e[0] - right.e[0], this->e[1] - right.e[1],
			this->e[2] - right.e[2]);
	}
	const Vec3<T> operator* (T c) const {								// scalar *
		return Vec3<T>(c*e[0], c*e[1], c*e[2]);
	}
	friend const Vec3<T> operator* (T c, const Vec3<T> &right) {
		return Vec3<T>(c*right.e[0], c*right.e[1], c*right.e[2]);
	}
	Vec3<T> &operator+= (const Vec3<T> &right) {						// +=
		for (int i = 0; i < 3; i++)
			this->e[i] += right.e[i];
		return *this;
	}
	Vec3<T> &operator-= (const Vec3<T> &right) {						// -=
		for (int i = 0; i < 3; i++)
			this->e[i] -= right.e[i];
		return *this;
	}
	Vec3<T> &operator*= (T c) {											// *= (scalar)
		for (int i = 0; i < 3; i++)
			this->e[i] *= c;
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Vec3<T> &right) {		// <<
		os << "[ ";
		for (int i = 0; i < 3; i++)
			os << right.e[i] << " ";
		os << "]";
		return os;
	}
	bool operator== (const Vec3<T> &right) const {						// relational
		return this->e[0] == right.e[0] && this->e[1] == right.e[1]
			&& this->e[2] == right.e[2];
	}
	bool operator!= (const Vec3<T> &right) const { return !((*this) == right); }
	bool operator<  (const Vec3<T> &right) const { return this->norm() < right.norm(); }
	bool operator<= (const Vec3<T> &right) const { return ((*this) == right) || ((*this) < right); }
	bool operator>  (const Vec3<T> &right) const { return !((*this) <= right); }
	bool operator>= (const Vec3<T> &right) const { return !((*this) < right); }

	// Functions
	// available: dot(), cross(), norm(), normalize()

	// Dot product
	const T dot(const Vec3<T> &right) const {
		T ret = 0;
		for (int i = 0; i < 3; i++)
			ret += this->e[i] * right.e[i];
		return ret;
	}

	// Cross product
	const Vec3<T> cross(const Vec3<T> &right) const {
		Vec3<T> ret;
		ret[X] = e[Y] * right[Z] - e[Z] * right[Y];
		ret[Y] = e[Z] * right[X] - e[X] * right[Z];
		ret[Z] = e[X] * right[Y] - e[Y] * right[X];
		return ret;
	}

	// Norm || v ||
	const T norm() const {
		return sqrt(this->dot(*this));
	}

	// Normalize
	Vec3<T> &normalize() {
		T norm_inv = 1 / (this->norm());
		return ((*this) *= norm_inv);
	}
};