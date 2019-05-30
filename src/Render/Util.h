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
	static float dot(Vector3 a, Vector3 b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	static Vector3 normalize(Vector3 v) {
		float d = Vector3::dot(v,v);
		float l = sqrt(d);
		return { v.x / l, v.y / l, v.z / l };
	}
	static Vector3 cross(Vector3 a, Vector3 b) {
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}
};

struct Matrix4 {
	Real m[16];
};

struct Transform {
	Real m[16];
	static Transform lookAt(Vector3 from, Vector3 to) {
		from.x *= -1, from.y *= -1, from.z *= -1;
		to.x *= -1, to.y *= -1, to.z *= -1;
		Vector3 F {
			to.x - from.x,
			to.y - from.y,
			to.z - from.z
		};
		F = Vector3::normalize(F);
		Vector3 U { 0, 0, 1 };
		Vector3 R = Vector3::cross(F, U);
		R = Vector3::normalize(R);
		U = Vector3::cross(F, R);
		auto& O = from;
		Vector3 V = {
			Vector3::dot(O, R),
			Vector3::dot(O, U),
			Vector3::dot(O, F),
		};

		return Transform {
			R.x, R.y, R.z, V.x,
			U.x, U.y, U.z, V.y,
			F.x, F.y, F.z, V.z,
			0, 0, 0, 1
		};
	}
};

struct Ball {
	Vector3 p;
	Real r;
};

struct Hole {
	Vector2 p;
	Real r;
};

struct Image {
};
