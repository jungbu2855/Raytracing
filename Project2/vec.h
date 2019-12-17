#pragma once

#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

template <typename T> class Vec4;
template <typename T> class Vec3;
template <typename T> class Mat4;
template <typename T> class Mat3;

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
	Vec4(Vec3<T> v3) { for (int i = 0; i < 3; i++) e[i] = v3[i]; e[3] = 1; }
	template <typename U> Vec4(Vec4<U> v) { for (int i = 0; i < 4; i++) this->e[i] = T(v.e[i]); }
	
	// Getter, Setters
	T get(int i) {
		assert(i >= 0 && i < 4);
		return e[i];
	}
	const T *getv() { return e; }
	void set(int i, T _e) {
		assert(i >= 0 &&i < 4);
		e[i] = _e;
	}
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
			this->e[i] = right.e[i];
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
	const Vec4<T> operator/ (T c) const {								// scalar /
		return Vec4<T>(c/e[0], c/e[1], c/e[2], c/e[3]);
	}
	const Vec4<T> operator* (const Vec4<T> &right) const {			// component-wise *
		return Vec4<T>(this->e[0] * right.e[0], this->e[1] * right.e[1],
			this->e[2] * right.e[2], this->e[3] * right.e[3]);
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
	Vec4<T> &operator*= (const Vec4<T> &right) {				// *= (component-wise)
		for (int i = 0; i < 4; i++)
			this->e[i] *= right.e[i];
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Vec4<T> &right) {	// <<
		os << "[ ";
		for (int i = 0; i < 4; i++)
			os << (right.e[i] < 0 ? "" : " ") << right.e[i] << " ";
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
	// available: dot(), cross(), norm(), normalize(), distance()

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
		T norm = this->norm();
		T norm_inv = norm != 0 ? 1 / norm : 0;
		return ((*this) *= norm_inv);
	}

	// Distance
	const T distance(const Vec4<T> &right) const {
		Vec4<T> v = right - (*this);
		return v.norm();
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
	Vec3(Vec4<T> v4) { for (int i = 0; i < 3; i++) e[i] = (v4[3] != 0 ? v4[i] / v4[3] : v4[i]); }
	template <typename U> Vec3(Vec3<U> v) { for (int i = 0; i < 3; i++) this->e[i] = T(v.e[i]); }
	
	// Getter, Setters
	T get(int i) {
		assert(i >= 0 && i < 3);
		return e[i];
	}
	const T *getv() { return e; }
	void set(int i, T _e) {
		assert(i >= 0 && i < 3);
		e[i] = _e;
	}
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
			this->e[i] = right.e[i];
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
	const Vec3<T> operator* (const Vec3<T> &right) const {			// component-wise *
		return Vec3<T>(this->e[0] * right.e[0], this->e[1] * right.e[1],
			this->e[2] * right.e[2]);
	}
	const Vec3<T> operator/ (T c) const {								// scalar /
		return Vec3<T>(e[0] / c, e[1] / c, e[2] / c);
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
	Vec3<T> &operator*= (const Vec3<T> &right) {				// *= (component-wise)
		for (int i = 0; i < 3; i++)
			this->e[i] *= right.e[i];
		return *this;
	}
	Vec3<T> &operator/= (T c) {											// /= (scalar)
		for (int i = 0; i < 3; i++)
			this->e[i] /= c;
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Vec3<T> &right) {		// <<
		os << "[ ";
		for (int i = 0; i < 3; i++)
			os << (right.e[i] < 0 ? "" : " ") << right.e[i] << " ";
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
	// available: dot(), cross(), norm(), normalize(), distance()

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
		ret[0] = e[1] * right[2] - e[2] * right[1];
		ret[1] = e[2] * right[0] - e[0] * right[2];
		ret[2] = e[0] * right[1] - e[1] * right[0];
		return ret;
	}

	// Norm || v ||
	const T norm() const {
		return sqrt(this->dot(*this));
	}

	// Normalize
	Vec3<T> &normalize() {
		T norm = this->norm();
		T norm_inv = norm != 0 ? 1 / norm : 0;
		return ((*this) *= norm_inv);
	}

	// Distance
	const T distance(const Vec3<T> &right) const {
		Vec3<T> v = right - (*this);
		return v.norm();
	}
};

template <typename T>
class Mat4 {
private:
	T m[4][4];
public:
	// Constructors
	Mat4() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				m[i][j] = 0;
		}
	}
	Mat4(T _m[4][4]) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				m[i][j] = _m[i][j];
		}
	}
	Mat4(Mat3<T> m3) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				m[i][j] = m3.get_ij(i,j);
			m[i][3] = 0;
		}
		m[3][3] = 1;
	}

	// Getter, Setters
	const T *getMat() const { return m; }
	T get_ij(int i, int j) const {
		assert(i >= 0 && i < 4 && j >= 0 && j < 4);
		return m[i][j];
	}
	void setMat(T _m[4][4]) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				m[i][j] = _m[i][j];
		}
	}
	void setMat_ij(T e, int i, int j) {
		assert(i >= 0 && i < 4 && j >= 0 && j < 4);
		m[i][j] = e;
	}

	// Operators
	Mat4<T> &operator= (const Mat4<T> &right) {							// =
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				this->m[i][j] = right.m[i][j];
		}
		return *this;
	}
	const Mat4<T> operator+ () const { return Mat4<T>(*this); }			// +
	const Mat4<T> operator+ (const Mat4<T> &right) const {
		Mat4<T> ret = *this;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret.m[i][j] += right.m[i][j];
		}
		return ret;
	}
	const Mat4<T> operator- () const {									// -
		Mat4<T> ret;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret.m[i][j] = -(this->m[i][j]);
		}
		return ret;
	}
	const Mat4<T> operator- (const Mat4<T> &right) const {
		Mat4<T> ret = *this;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret.m[i][j] -= right.m[i][j];
		}
		return ret;
	}
	const Mat4<T> operator* (T c) const {								// scalar *
		Mat4<T> ret = *this;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret.m[i][j] *= c;
		}
		return ret;
	}
	friend const Mat4<T> operator* (T c, const Mat4<T> &right) {
		Mat4<T> ret = right;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret.m[i][j] *= c;
		}
		return ret;
	}
	const Mat4<T> operator* (const Mat4<T> &right) const {				// matmul
		Mat4<T> ret;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++)
					ret.m[i][j] += this->m[i][k] * right.m[k][j];
			}
		}
		return ret;
	}
	const Vec4<T> operator* (const Vec4<T> &v) const {					// mat * vec
		Vec4<T> ret;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret[i] += m[i][j] * v[j];
		}
		return ret;
	}
	friend const Vec4<T> operator* (const Vec4<T> &v, const Mat4<T> &right) {
		Vec4<T> ret;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				ret[i] += right.m[i][j] * v[j];
		}
		return ret;
	}
	Mat4<T> &operator+= (const Mat4<T> &right) {						// +=
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				this->m[i][j] += right.m[i][j];
		}
		return *this;
	}
	Mat4<T> &operator-= (const Mat4<T> &right) {						// -=
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				this->m[i][j] -= right.m[i][j];
		}
		return *this;
	}
	Mat4<T> &operator*= (T c) {											// *= (scalar)
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				this->m[i][j] *= c;
		}
		return *this;
	}
	Mat4<T> &operator*= (const Mat4<T> right) {							// *= (matmul)
		*this = (*this) * right;
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Mat4<T> &right) {	// <<
		os << "[";
		for (int i = 0; i < 4; i++) {
			os << (i == 0 ? "[ " : " [ ");
			for (int j = 0; j < 4; j++)
				os << (right.m[i][j] < 0 ? "" : " ") << right.m[i][j] << " ";
			os << (i != 3 ? "]\n" : "]");
		}
		os << "]";
		return os;
	}
	bool operator== (const Mat4<T> &right) const {						// relational
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (this->m[i][j] != right.m[i][j])
					return false;
			}
		}
		return true;
	}
	bool operator!= (const Mat4<T> &right) const { return !((*this) == right); }

	// Functions
	const Mat4<T> &loadIdentity() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				this->m[i][j] = (i == j ? T(1) : T(0));
		}
		return *this;
	}
};

template <typename T>
class Mat3 {
private:
	T m[3][3];
public:
	// Constructors
	Mat3() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				m[i][j] = 0;
		}
	}
	Mat3(T _m[3][3]) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				m[i][j] = _m[i][j];
		}
	}
	Mat3(Mat4<T> _m) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				m[i][j] = _m.get_ij(i,j);
		}
	}

	// Getter, Setters
	const T *getMat() const { return m; }
	T get_ij(int i, int j) const {
		assert(i >= 0 && i < 3 && j >= 0 && j < 3);
		return m[i][j];
	}
	void setMat(T _m[3][3]) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				m[i][j] = _m[i][j];
		}
	}
	void setMat_ij(T e, int i, int j) {
		assert(i >= 0 && i < 3 && j >= 0 && j < 3);
		m[i][j] = e;
	}

	// Operators
	Mat3<T> &operator= (const Mat3<T> &right) {							// =
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				this->m[i][j] = right.m[i][j];
		}
		return *this;
	}
	const Mat3<T> operator+ () const { return Mat3<T>(*this); }			// +
	const Mat3<T> operator+ (const Mat3<T> &right) const {
		Mat3<T> ret = *this;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret.m[i][j] += right.m[i][j];
		}
		return ret;
	}
	const Mat3<T> operator- () const {									// -
		Mat3<T> ret;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret.m[i][j] = -(this->m[i][j]);
		}
		return ret;
	}
	const Mat3<T> operator- (const Mat3<T> &right) const {
		Mat3<T> ret = *this;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret.m[i][j] -= right.m[i][j];
		}
		return ret;
	}
	const Mat3<T> operator* (T c) const {								// scalar *
		Mat3<T> ret = *this;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret.m[i][j] *= c;
		}
		return ret;
	}
	friend const Mat3<T> operator* (T c, const Mat3<T> &right) {
		Mat3<T> ret = right;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret.m[i][j] *= c;
		}
		return ret;
	}
	const Mat3<T> operator* (const Mat3<T> &right) const {				// matmul
		Mat3<T> ret;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++)
					ret.m[i][j] += this->m[i][k] * right.m[k][j];
			}
		}
		return ret;
	}
	const Vec3<T> operator* (const Vec3<T> &v) const {					// mat * vec
		Vec3<T> ret;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret[i] += m[i][j] * v[j];
		}
		return ret;
	}
	friend const Vec3<T> operator* (const Vec3<T> &v, const Mat3<T> &right) {
		Vec3<T> ret;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				ret[i] += right.m[i][j] * v[j];
		}
		return ret;
	}
	Mat3<T> &operator+= (const Mat3<T> &right) {						// +=
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				this->m[i][j] += right.m[i][j];
		}
		return *this;
	}
	Mat3<T> &operator-= (const Mat3<T> &right) {						// -=
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				this->m[i][j] -= right.m[i][j];
		}
		return *this;
	}
	Mat3<T> &operator*= (T c) {											// *= (scalar)
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				this->m[i][j] *= c;
		}
		return *this;
	}
	Mat3<T> &operator*= (const Mat3<T> right) {							// *= (matmul)
		*this = (*this) * right;
		return *this;
	}
	friend ostream& operator<< (ostream& os, const Mat3<T> &right) {	// <<
		os << "[";
		for (int i = 0; i < 3; i++) {
			os << (i == 0 ? "[ " : " [ ");
			for (int j = 0; j < 3; j++)
				os << (right.m[i][j] < 0 ? "" : " ") << right.m[i][j] << " ";
			os << (i != 2 ? "]\n" : "]");
		}
		os << "]";
		return os;
	}
	bool operator== (const Mat3<T> &right) const {						// relational
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (this->m[i][j] != right.m[i][j])
					return false;
			}
		}
		return true;
	}
	bool operator!= (const Mat3<T> &right) const { return !((*this) == right); }

	// Functions
	const Mat3<T> &loadIdentity() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				this->m[i][j] = (i == j ? 1 : 0);
		}
		return *this;
	}
};


// Transformation functions

template <typename T>
const Mat4<T> rotate(T rad, const Vec3<T> &axis) {
	T _cos = cos(rad);
	T _sin = sin(rad);
	Vec3<T> _axis = axis;
	_axis.normalize();

	T temp[4][4] = {
		{_cos + _axis[0] * _axis[0] * (1 - _cos), _axis[0] * _axis[1] * (1 - _cos) - _axis[2] * _sin, _axis[0] * _axis[2] * (1 - _cos) + _axis[1] * _sin, 0},
		{_axis[1] * _axis[0] * (1 - _cos) + _axis[2] * _sin, _cos + _axis[1] * _axis[1] * (1 - _cos), _axis[1] * _axis[2] * (1 - _cos) - _axis[0] * _sin, 0},
		{_axis[2] * _axis[0] * (1 - _cos) - _axis[1] * _sin, _axis[2] * _axis[1] * (1 - _cos) + _axis[0] * _sin, _cos + _axis[2] * _axis[2] * (1 - _cos), 0},
		{0, 0, 0, 1}
	};
	return Mat4<T>(temp);
}

template <typename T>
const Mat4<T> translate(const Vec3<T> &displacement) {
	Mat4<T> ret;
	ret.loadIdentity();
	for (int i = 0; i < 3; i++) {
		ret.setMat_ij(displacement[i], i, 3);
	}
	return ret;
}

template <typename T>
const Mat4<T> scale(T scaleX, T scaleY, T scaleZ) {
	Mat4<T> ret;
	ret.setMat_ij(scaleX, 0, 0);
	ret.setMat_ij(scaleY, 1, 1);
	ret.setMat_ij(scaleZ, 2, 2);
	ret.setMat_ij(1, 3, 3);
	return ret;
}

template <typename T>
const Mat4<T> scale(T factor) {
	return scale(factor, factor, factor);
}