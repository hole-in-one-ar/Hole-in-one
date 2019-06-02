#pragma once

#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>

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
	Vector3 getOrigin() const {
		return { m[3], m[7], m[11] };
	}
	Vector3 getViewOrigin() const {
		Vector3 P = { -m[3], -m[7], -m[11] };
		Vector3 RTx = { m[0], m[4], m[8] };
		Vector3 RTy = { m[1], m[5], m[9] };
		Vector3 RTz = { m[2], m[6], m[10] };
		return Vector3{
			Vector3::dot(RTx, P),
			Vector3::dot(RTy, P),
			Vector3::dot(RTz, P),
		};
	}
	Vector3 getNormal() const {
		return { m[2], m[6], m[10] };
	}
	Transform inverse() const {
		Vector3 P = { -m[3], -m[7], -m[11] };
		Vector3 RTx = { m[0], m[4], m[8] };
		Vector3 RTy = { m[1], m[5], m[9] };
		Vector3 RTz = { m[2], m[6], m[10] };
		return Transform {
			RTx.x, RTx.y, RTx.z, Vector3::dot(RTx, P),
			RTy.x, RTy.y, RTy.z, Vector3::dot(RTy, P),
			RTz.x, RTz.y, RTz.z, Vector3::dot(RTz, P),
			0, 0, 0, 1
		};
	}
	Transform transfer(Transform base) const {
		Transform t;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				t.m[i * 4 + j] = 0;
				for (int k = 0; k < 4; k++) {
					t.m[i * 4 + j] += m[i * 4 + k] * base.m[k * 4 + j];
				}
			}
		}
		return t;
	}
};

struct BallPos {
	Vector3 p;
	Real r;
};

struct HolePos {
	Vector2 p;
	Real r;
};

using Image = cv::Mat;
