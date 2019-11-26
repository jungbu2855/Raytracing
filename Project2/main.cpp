#include <iostream>
#include "bmploader.h"
#include "vec.h"

using namespace std;

typedef Vec3<float> Vec3f;

int main() {
	Vec3f a(1), b(2);
	float c[] = { 1,2,3,4 };
	Vec3f d(c);
	cout << -a << +b << endl << a + b << endl << a - b << endl;
	a.set(1, 3);
	b.setv(c);
	cout << a.getv()[1] << endl << b << 3*d * 2<< endl;
	cout << a.get(X) << endl;
	cout << (a += 3) << endl;
	cout << a[Y] << endl;
	cout << a[X] + b[X] << endl;
	a[X] = -1;
	cout << a.cross(b) << endl;
	Vec3f cross = a.cross(b);
	cout << a << endl;
	cout << a << endl << a.normalize().norm() << a << endl;
	cout << a.norm();

	Vec4<float> aa(1);
	Vec4<float> bb(2);
	cout << (aa > bb);
}