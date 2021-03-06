#include "Render.h"
#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
uint nextP2(uint x) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

GLuint textureIndex = 0;

Texture::Texture() : initialized(false), w(0), h(0) {
	index = textureIndex;
	std::cout << "make texture " << textureIndex << std::endl;
	textureIndex++;
}

void Texture::set(Image img) {
	if (!initialized) {
		glGenTextures(1, &id);
	}
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, id);
	cv::Size p2Size(nextP2(img.cols), nextP2(img.rows));
	//cv::resize(img, img, p2Size);
	if (w == img.cols && h == img.rows) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	} else {
		w = img.cols;
		h = img.rows;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	}
	if (!initialized) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		initialized = true;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::use() {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, id);
	return index;
}

Material::Material() {}

std::string Material::loadFile(FileName fn) {
	std::ifstream ifs(fn);
	if (!ifs) {
		throw std::runtime_error("Failed to load file: " + std::string(fn));
	}
	std::string cont;
	std::string line;
	while (std::getline(ifs, line)) {
		cont += line + "\n";
	}
	return cont;
}

Shader Material::compileShader(GLuint type, Resource src) {
	const Shader s = glCreateShader(type);
	std::string content = loadFile("../res/" + src);
	const char* contSrc = content.c_str();
	const int contLen = content.size() + 1;
	glShaderSource(s, 1, &contSrc, &contLen);
	glCompileShader(s);

	GLint res;
	glGetShaderiv(s, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		GLint len;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
		const unsigned int bufSize = 1024;
		char buf[bufSize];
		glGetShaderInfoLog(s, bufSize, &len, buf);
		std::cerr << buf << std::endl;
		throw std::runtime_error("Failed to compile shader");
	}
	return s;
}

Program Material::compileProgram(Shader vertex, Shader fragment) {
	const Program p = glCreateProgram();
	glAttachShader(p, vertex);
	glAttachShader(p, fragment);
	glLinkProgram(p);

	GLint res;
	glGetProgramiv(p, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		GLint len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
		const unsigned int bufSize = 1024;
		char buf[bufSize];
		glGetProgramInfoLog(p, bufSize, &len, buf);
		std::cerr << buf << std::endl;
		throw std::runtime_error("Failed to link program");
	}
	return p;
}

Material::Material(Resource vs, Resource fs) {
	Shader v = compileShader(GL_VERTEX_SHADER, vs);
	Shader f = compileShader(GL_FRAGMENT_SHADER, fs);
	program = compileProgram(v, f);
	glBindAttribLocation(program, 0, "vertex");
}

UniformLoc Material::uniformLoc(Uniform u) {
	auto it = uniformLocation.find(u);
	if (it == uniformLocation.end()) {
		UniformLoc loc = glGetUniformLocation(program, u.c_str());
		return uniformLocation[u] = loc;
	}
	return it->second;
}

void Material::setParam(Uniform name, Texture v) {
	paramI1[name] = v.use();
}
void Material::setParam(Uniform name, uint v) {
	paramI1[name] = v;
}
void Material::setParam(Uniform name, float v) {
	paramV1[name] = v;
}
void Material::setParam(Uniform name, Vector2 v) {
	paramV2[name] = v;
}
void Material::setParam(Uniform name, Vector3 v) {
	paramV3[name] = v;
}
void Material::setParam(Uniform name, Matrix4 v) {
	paramM4[name] = v;
}
void Material::setParam(Uniform name, Transform v) {
	paramM4[name] = *reinterpret_cast<Matrix4*>(&v);
}

void Material::use() {
	glUseProgram(program);
	for (auto p : paramI1) {
		glUniform1i(uniformLoc(p.first), p.second);
	}
	for (auto p : paramV1) {
		glUniform1f(uniformLoc(p.first), p.second);
	}
	for (auto p : paramV2) {
		glUniform2f(uniformLoc(p.first), p.second.x, p.second.y);
	}
	for (auto p : paramV3) {
		glUniform3f(uniformLoc(p.first), p.second.x, p.second.y, p.second.z);
	}
	for (auto p : paramM4) {
		glUniformMatrix4fv(uniformLoc(p.first), 1, true, p.second.m);
	}
}

Object::Object(DefaultUniform &uni) : uni(uni) {}

void Object::build(Model mo, Material ma) {
	mesh.vertexCount = mo.vertexCount;
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, mo.vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,false,0,0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	mat = ma;
}

void Object::render() {
	glBindVertexArray(mesh.vao);
	mat.setParam("cameraPos", uni.C);
	mat.setParam("V", uni.V);
	mat.setParam("P", uni.P);
	mat.use();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertexCount);
	glBindVertexArray(0);
}

Render::Render(float w, float h)
	: width(w), height(h)
	, background(uni), ball(uni), hole(uni), ground(uni), holeSide(uni), ballShadow(uni), shooter(uni), text(uni), particle(uni), ring(uni), arrow(uni) {
	glewInit();
	buildObjects();
	
	float fovy = PI * 0.465; // 中に来る: FOVがでかい
	float aspect = w / h;
	float near = 0.01;
	float far = 100.0;
	float f = 1 / tan(fovy / 2);
	uni.P = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (far + near) / (near - far), (2 * far * near) / (near - far),
		0, 0, -1, 0
	};
}

void GLAPIENTRY errorCB( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
	if(type == GL_DEBUG_TYPE_ERROR) std::cerr << "[GL ERROR] " << message << std::endl;
}

void Render::buildObjects() {
	VBO buffers[5];
	glGenBuffers(5, buffers);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(errorCB,0);
	
	Model planeModel = { buffers[0], 0 };
	Model ballModel = { buffers[1], 0 };
	Model holeSideModel = { buffers[2], 0 };
	Model shooterModel = { buffers[3], 0 };
	Model particleModel = { buffers[4], 0 };
	buildModels(planeModel, ballModel, holeSideModel, shooterModel, particleModel);

	Material backgroundMat("background.vert", "background.frag");
	Material ballMat("default.vert", "ball.frag");
	Material holeMat("default.vert", "hole.frag");
	Material groundMat("default.vert", "ground.frag");
	Material holeSideMat("default.vert", "holeSide.frag");
	Material ballShadowMat("default.vert", "ballShadow.frag");
	Material shooterMat("shooter.vert", "shooter.frag");
	Material textMat("text.vert", "text.frag");
	Material particleMat("particle.vert", "particle.frag");
	Material ringMat("default.vert", "ring.frag");
	Material arrowMat("arrow.vert", "arrow.frag");

	background.build(planeModel, backgroundMat);
	ball.build(ballModel, ballMat);
	hole.build(planeModel, holeMat);
	ground.build(planeModel, groundMat);
	holeSide.build(holeSideModel, holeSideMat);
	ballShadow.build(planeModel, ballShadowMat);
	shooter.build(shooterModel, shooterMat);
	text.build(planeModel, textMat);
	particle.build(particleModel, particleMat);
	ring.build(planeModel, ringMat);
	arrow.build(planeModel, arrowMat);

	digitTex.set(cv::imread("../res/digit.png"));
}

void Render::sendModelData(Model &model, std::vector<Vector3> &verts) {
	glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vector3), verts.data(), GL_STATIC_DRAW);
	model.vertexCount = verts.size();
}

Vector3 sphereCoord(float theta, float phi) { // latitude, longitude
	return {
		cos(phi) * cos(theta),
		sin(phi) * cos(theta),
		sin(theta)
	};
}

void Render::buildModels(Model &planeModel, Model &ballModel, Model &holeSideModel, Model &shooterModel, Model &particleModel) {
	std::vector<Vector3> verts;
	
	// planeModel
	verts.clear();
	verts.push_back({1,1,0});
	verts.push_back({-1,1,0});
	verts.push_back({1,-1,0});
	verts.push_back({-1,-1,0});
	sendModelData(planeModel, verts);
	// ballModel
	verts.clear();
	const unsigned int latitude = 12;
	const unsigned int longitude = 24;
	const float unitLat = PI / latitude;
	const float unitLon = 2 * PI / longitude;
	for (int i = 0; i < latitude; i++) {
		const float lat = i * unitLat - PI / 2;
		verts.push_back(sphereCoord(lat, 0));
		for (int j = 0; j <= longitude; j++) {
			const float lon = j * unitLon;
			verts.push_back(sphereCoord(lat, lon));
			verts.push_back(sphereCoord(lat + unitLat, lon));
		}
		verts.push_back(sphereCoord(lat + unitLat, 2 * PI));
	}
	sendModelData(ballModel, verts);
	// holeSideModel
	verts.clear();
	const unsigned int angle = 24;
	const float unitAngle = 2 * PI / longitude;
	for (int i = 0; i <= angle; i++) {
		const float a = i * unitAngle;
		verts.push_back({ cos(a), sin(a), 0 });
		verts.push_back({ cos(a), sin(a), -100 });
	}
	sendModelData(holeSideModel, verts);
	// shooterModel
	verts.clear();
	const float r = 1.5;
	for (int i = 0; i <= angle; i++) {
		const float a = i * unitAngle;
		verts.push_back({ cos(a), sin(a), -1.0 });
		verts.push_back({ cos(a) * r, sin(a) * r, 1.0 });
	}
	verts.push_back({ r, 0, 1.0 });
	verts.push_back({ 0, 0, -1.0 });
	for (int i = 0; i <= angle; i++) {
		const float a = i * unitAngle;
		verts.push_back({ 0, 0, -1.0 });
		verts.push_back({ cos(a), sin(a), -1.0 });
	}
	sendModelData(shooterModel, verts);
	// particleModel
	verts.clear();
	for (int i = 0; i < 30; i++) {
		float x = i * 2;
		verts.push_back({ x - 0.5f, -0.5f, 0 });
		verts.push_back({ x - 0.5f, -0.5f, 0 });
		verts.push_back({ x + 0.5f, -0.5f, 0 });
		verts.push_back({ x - 0.5f, +0.5f, 0 });
		verts.push_back({ x + 0.5f, +0.5f, 0 });
		verts.push_back({ x + 0.5f, +0.5f, 0 });
	}
	sendModelData(particleModel, verts);
}

void Render::setCamera(Transform transform) {
	uni.C = transform.getViewOrigin();
	uni.V = transform;
}

void Render::drawBall(BallPos b, HolePos h) {
	ball.setParam("o", b.p);
	ball.setParam("s", b.r);
	ball.setParam("image", bgTex);
	ball.render();
	if (b.p.z > 0) {
		glDepthMask(0);
		ballShadow.setParam("o", Vector3{ b.p.x, b.p.y, 0.002f });
		ballShadow.setParam("s", b.r*2);
		ballShadow.setParam("b", b.p);
		ballShadow.setParam("r", b.r);
		ballShadow.setParam("h", h.p);
		ballShadow.setParam("hr", h.r * 0.75f);
		ballShadow.render();
		glDepthMask(1);
	}
}

void Render::drawHole(HolePos h) {
	float r = h.r * 0.75f;
	Vector3 o { h.p.x, h.p.y, 0 };
	// Writing Stencil
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(0, 0, 0, 0);
	glDepthMask(0);
	hole.setParam("o", o);
	hole.setParam("s", r);
	hole.render();
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDepthMask(1);
	// Check Stencil
	glStencilFunc(GL_NOTEQUAL, 1, 1);
	ground.setParam("o", Vector3{ 0, 0, 0.001f });
	ground.setParam("s", 1000.f);
	ground.render();
	glStencilFunc(GL_ALWAYS, 0, 1);
	glColorMask(1, 1, 1, 1);

	holeSide.setParam("o", o);
	holeSide.setParam("s", r);
	holeSide.setParam("image", bgTex);
	holeSide.render();
}

void Render::drawShooter(Vector3 origin, Vector3 normal, float scale) {
	shooter.setParam("o", origin);
	shooter.setParam("n", normal);
	shooter.setParam("s", scale);
	shooter.setParam("image", bgTex);
	shooter.render();
}

void Render::drawBackground(Image bg) {
	glDepthMask(0);
	bgTex.set(bg);
	background.setParam("image", bgTex);
	background.render();
	glDepthMask(1);
}

void Render::drawScore(Vector3 origin, float scale, float diffAngle, float thickness, float color, uint value) {
	glDepthMask(0);
	text.setParam("o", origin);
	text.setParam("s", scale);
	text.setParam("diffAngle", diffAngle);
	text.setParam("thickness", thickness);
	text.setParam("color", color);
	text.setParam("value", value);
	text.setParam("digit", digitTex);
	text.render();
	glDepthMask(1);
}

void Render::drawParticle(Vector3 origin, float scale, float time, uint seed) {
	glDepthMask(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	particle.setParam("o", origin);
	particle.setParam("s", scale);
	particle.setParam("time", time);
	particle.setParam("seed", seed);
	particle.render();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(1);
}

void Render::drawRing(Vector2 origin, float i, float o) {
	glDepthMask(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	ring.setParam("o", Vector3{ origin.x, origin.y, 0.002f } );
	ring.setParam("s", o);
	ring.setParam("i", i/o);
	ring.render();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(1);
}

void Render::drawArrow(Vector3 origin, float s, float a) {
	arrow.setParam("o", origin);
	arrow.setParam("s", s);
	arrow.setParam("a", a);
	arrow.render();
}

