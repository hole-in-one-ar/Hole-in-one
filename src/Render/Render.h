#pragma once

#include "Util.h"
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <map>

using Shader = GLuint;
using Program = GLuint;
using VBO = GLuint;
using FileName = std::string;
using Resource = std::string;
using Uniform = std::string;
using UniformLoc = GLuint;

struct Mesh {
	VBO vbo;
	unsigned int vertexCount;
};

class Material {
	Program program;
	std::string loadFile(FileName fn);
	Shader compileShader(GLuint type, Resource src);
	Program compileProgram(Shader vertex, Shader fragment);
	std::map<Uniform, UniformLoc> uniformLocation;
	std::map<Uniform, float>   paramV1;
	std::map<Uniform, Vector2> paramV2;
	std::map<Uniform, Vector3> paramV3;
	std::map<Uniform, Matrix4> paramM4;
	UniformLoc uniformLoc(Uniform u);
public:
	Material();
	Material(Resource vs, Resource fs);
	void setParam(Uniform name, float value);
	void setParam(Uniform name, Vector2 value);
	void setParam(Uniform name, Vector3 value);
	void setParam(Uniform name, Matrix4 value);
	void setParam(Uniform name, Transform value);
	void use();
};

class Object {
	Mesh mesh;
	Material mat;
public:
	Object();
	void build(Mesh mesh, Material mat);
	template <class T> void setParam(Uniform name, T value) {
		mat.setParam(name, value);
	}
	void render();
};

class Render {
	Transform camera;
	Object ball, hole, holeSide;
	void buildObjects();
	void buildModels(Mesh &ballModel, Mesh &holeModel, Mesh &holeSideModel);
	void sendModelData(Mesh&, std::vector<Vector3>&);
public:
	Render(float w, float h);
	void setCamera(Transform);
	void drawBall(Ball);
	void drawHole(Hole);
	void drawBackground(Image);
};