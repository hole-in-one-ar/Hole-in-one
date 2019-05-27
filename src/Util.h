#pragma once

#include <cmath>
#include <vector>

using Real = float;
const Real PI = 3.1415926535;

struct Vector2 {
	Real x, y;
};

struct Vector3 {
	Real x, y, z;
};

struct Transform {
};

struct Ball {
	Vector3 p;
	Real r;
};

struct Hole {
	Vector2 p;
};

struct Image {
};
