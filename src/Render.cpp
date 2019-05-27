#include "Render.h"
#include <string>
#include <iostream>
#include <fstream>

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
	glGetProgramiv(s, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		GLint len;
		glGetProgramiv(s, GL_INFO_LOG_LENGTH, &len);
		const unsigned int bufSize = 1024;
		char buf[bufSize];
		glGetProgramInfoLog(s, bufSize, &len, buf);
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
	glGetShaderiv(p, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		GLint len;
		glGetShaderiv(p, GL_INFO_LOG_LENGTH, &len);
		const unsigned int bufSize = 1024;
		char buf[bufSize];
		glGetShaderInfoLog(p, bufSize, &len, buf);
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
	glEnableVertexAttribArray(0);
}

UniformLoc Material::uniformLoc(Uniform u) {
	auto it = uniformLocation.find(u);
	if (it == uniformLocation.end()) {
		UniformLoc loc = glGetUniformLocation(program, u.c_str());
		return uniformLocation[u] = loc;
	}
	return it->second;
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

void Material::use() {
	glUseProgram(program);
	for (auto p : paramV1) {
		glUniform1f(uniformLoc(p.first), p.second);
	}
	for (auto p : paramV2) {
		glUniform2f(uniformLoc(p.first), p.second.x, p.second.y);
	}
	for (auto p : paramV3) {
		glUniform3f(uniformLoc(p.first), p.second.x, p.second.y, p.second.z);
	}
	glVertexAttribPointer(0,3,GL_FLOAT,false,0,0);
}

Object::Object() {};

void Object::build(Mesh me, Material ma) {
	mesh = me;
	mat = ma;
}

void Object::render() {
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	mat.use();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertexCount);
}

Render::Render() {
	glewInit();

	buildObjects();
}

void Render::buildObjects() {
	VBO buffers[3];
	glGenBuffers(3, buffers);
	Mesh ballModel = { buffers[0], 0 };
	Mesh holeModel = { buffers[1], 0 };
	Mesh holeSideModel = { buffers[2], 0 };
	buildModels(ballModel, holeModel, holeSideModel);

	Material ballMat("default.vert", "ball.frag");
	Material holeMat("default.vert", "ball.frag");
	Material holeSideMat("default.vert", "ball.frag");

	ball.build(ballModel, ballMat);
	hole.build(holeModel, holeMat);
	holeSide.build(holeSideModel, holeSideMat);
}

Vector3 sphereCoord(float theta, float phi) { // latitude, longitude
	return {
		cos(phi) * cos(theta),
		sin(phi) * cos(theta),
		sin(theta)
	};
}

void Render::sendModelData(Mesh &mesh, std::vector<Vector3> &verts) {
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vector3), verts.data(), GL_STATIC_DRAW);
	mesh.vertexCount = verts.size();
}

void Render::buildModels(Mesh &ballModel, Mesh &holeModel, Mesh &holeSideModel) {
	std::vector<Vector3> verts;
	
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
	// holeModel
	verts.clear();
	verts.push_back({1,1,0});
	verts.push_back({-1,1,0});
	verts.push_back({1,-1,0});
	verts.push_back({-1,-1,0});
	sendModelData(holeModel, verts);
	// holeSideModel
	verts.clear();
	const unsigned int angle = 24;
	const float unitAngle = 2 * PI / longitude;
	for (int i = 0; i <= angle; i++) {
		const float a = i * unitAngle;
		verts.push_back({ cos(a), sin(a), 0 });
		verts.push_back({ cos(a), sin(a), 1 });
	}
	sendModelData(holeSideModel, verts);
}

void Render::setCamera(Transform transform) {
	camera = transform;
}

void Render::drawBall(Ball b) {
	static float time = 0;
	time++;
	ball.setParam("time", time);
	ball.render();
}

void Render::drawHole(Hole h) {
}

void Render::drawBackground(Image bg) {
}
