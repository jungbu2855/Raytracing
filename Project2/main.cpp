#include <iostream>
#include "bmploader.h"
#include "vec.h"

using namespace std;

typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;
typedef Mat3<float> Mat3f;
typedef Mat4<float> Mat4f;
int main() {
	Mat3f m1;
	Mat3f m2;
	float m_[3][3] = { {1,2,3},{5,6,7},{1,3,5} };

	Vec3f v1(3,2,1);

	m1.setMat(m_);
	m2 = m1;
	Vec3f v2 = m1 * v1;
	Vec3f axis(0, 0, 1);
	Mat4f m3 = translate(v2); 
	m3 *= rotate(3.141592 / 2, axis);
	m3 *= -scale(2.f, 2.f, 3.f);
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed);
	cout << m1 << endl << v2 << endl << m3;
}