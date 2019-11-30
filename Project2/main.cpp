#include <iostream>
#include "bmploader.h"
#include "vec.h"
#include "mesh.h"
#include "definitions.h"
#include "material.h"
using namespace std;

int main() {
	Material material;
	Mesh mesh("bunny.off", material);
}