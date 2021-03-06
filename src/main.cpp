#include "Render/Render.h"
#include "Render/Window.h"

#include "Physics/PhysicsBall.h"
#include "Physics/Hole.h"
#include "Physics/Physics.h"
#include "Physics/Vec3.h"

#include "Vision/Tracking.h"

#include <random>

struct Effect {
	Vector3 position;
	unsigned int score;
	float time;
	Vector2 hole;
};

float eb(float x) {
	x = 1 - x;
	return 1 - x * x * (3 * x - 2);
}

int main() {
	Window window(1280, 720, "app");
	Render render(1280, 720);
	Physics physics;
	Tracking tracking;
	std::vector<Effect> effects;
	//render.setCamera(Transform::lookAt(Vector3{ 0,3,1 }, Vector3{ 0,0,0 }));
	//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
	bool holeIn = false;
	tracking.resetCamera([&](Transform m) -> bool {
		//render.setCamera(Transform::lookAt(Vector3{ 20,0,6 }, Vector3{ 0,0,4 }));
		if (holeIn) {
			return false;
		} else {
			render.setCamera(m);
			return true;
		}
	});

	int holeWaiting = -1; // -1: stable, 0-100: animation
	HolePos hole{ {0,0}, 0.06f };
	physics.setHole(hole.p.x, hole.p.y, hole.r);
	//physics.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0), 1);
	//physics.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0), 500);
	float t = 0;
	std::random_device rd;
	std::mt19937 random(rd());
	std::uniform_real_distribution<float> uniform;
	float arrowTime = 0;
	float arrowWait = -1;
	unsigned int score = 0;
	auto addScore = [&]() {
		score++;
		if (score % 5 == 0) {
			physics.setHole(-1000, -1000, hole.r);
			holeWaiting = 0;
		}
		if (score == 1) {
			arrowWait = 0;
		}
	};
	while (window.alive()) {
		tracking.update();
		render.drawBackground(tracking.cameraImage);
		t += 0.1;
		// Hole
		if (holeWaiting == -1) {
			render.drawHole(hole);
		} else {
			HolePos h = hole;
			if (holeWaiting < 50) h.r *= eb(1. - holeWaiting / 50.0);
			else h.r *= eb((holeWaiting - 50.0) / 100.0);
			render.drawHole(h);
			holeWaiting += 5;
			if (holeWaiting == 50) {
				float r = std::sqrt(uniform(random)) * 0.1;
				float a = uniform(random) * 3.1415926535 * 2;
				hole.p.x = cos(a) * r;
				hole.p.y = sin(a) * r;
			} else if (holeWaiting == 150) {
				physics.setHole(hole.p.x, hole.p.y, hole.r);
				holeWaiting = -1;
			}
		}
		if (arrowWait < 1.0) {
			//arrowTime += 0.2;
			if(arrowWait > -0.5) arrowWait += 0.1f;
			float t = arrowWait < 0.f ? 0 : arrowWait;
			float move = 1. - eb(1. - t);
			render.drawArrow({ 0, 0, 0.04f - 0.2f * move }, 0.05f, arrowTime);
		}
		// Shooter
		if (tracking.aliveController()) {
			Transform tr = tracking.controller();
			Vector3 p = tr.getOrigin();
			Vector3 n = tr.getNormal();
			render.drawShooter(p, n, 0.02f);
			if (t > 1.0) {
				t = 0;
				float f = 100.0f + 50.0f * uniform(random);
				Vector3 v = { n.x*f, n.y*f, n.z*f };
				float r = 0.02f;
				if (p.z > r) {
					physics.addBalls(Vec3(p.x, p.y, p.z), r, Vec3(v.x, v.y, v.z), 1);
				}
			}
		}
		// Balls
		std::vector<int> removeIndices;
		unsigned int ix = 0;
		for (const auto &b : physics.getBalls()) {
			auto p = b.getOrigin();
			float r = b.getRadium();
			if (abs(p.x) < 1 && abs(p.y) < 1 && p.z > -1.0) {
				render.drawBall({ {p.x,p.y,p.z}, r }, hole);
			}
			if(abs(p.x) > 1 || abs(p.y) > 1 || p.z < 0.0) {
				removeIndices.push_back(ix);
				if (p.z < 0.0) {
					addScore();
					effects.push_back({ {p.x,p.y,p.z}, score, 0.f, hole.p });
					holeIn = true;
					std::cout << "Hole In!" << std::endl;
				}
			}
			ix++;
		}
		for (auto i : removeIndices) {
			physics.removeBall(i);
		}
		// Effects
		for (auto &e : effects) {
			Vector3 p = e.position;
			p.z += (1 - exp(-e.time*2.0)) * 0.03;
			p.z += e.time*0.01f;
			float s = (1 - exp(-e.time*2.0)) * 0.03 + 0.01;
			float t = 1 - std::max(std::min(e.time/2.0f, 1.f), 0.f);
			float da = (t*t*(1.1*t-0.1)) * 20.0;
			float thickness = std::max(0.f, std::min(1.f, 5.f - e.time));
			float color = std::max(0.f, std::min(1.f, (5.f - e.time) / 4.f));
			render.drawParticle(e.position, 0.02, e.time, e.score);
			render.drawScore(p, s, da, thickness, color, e.score);
			if (e.time < 2.f) {
				float ringRadi = (1. - pow(1. - e.time / 1.f, 3.0)) * 0.1f + hole.r * 0.75f;
				float ringW = pow(1. - e.time / 1.f, 5.0) * 0.06f;
				render.drawRing(e.hole, ringRadi - ringW, ringRadi);
			}
			e.time += 0.1;
		}
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect e) -> bool {
			return e.time > 5.0f;
		}), effects.end());
		// Physics
		for(int i=0;i<10;i++) physics.simulation();
		window.refresh();
	}
	return 0;
}

