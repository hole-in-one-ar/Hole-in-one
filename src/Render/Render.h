#pragma once

#include "Util.h"
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <map>

using Shader = GLuint;
using Program = GLuint;
using VAO = GLuint;
using VBO = GLuint;
using FileName = std::string;
using Resource = std::string;
using Uniform = std::string;
using UniformLoc = GLuint;

class Texture {
	bool initialized;
	GLuint id;
	unsigned int w, h;
public:
	Texture();
	void set(Image);
	GLuint use();
};

struct Model {
	VBO vbo;
	unsigned int vertexCount;
};

struct Mesh {
	VAO vao;
	unsigned int vertexCount;
};

class Material {
	Program program;
	std::string loadFile(FileName fn);
	Shader compileShader(GLuint type, Resource src);
	Program compileProgram(Shader vertex, Shader fragment);
	std::map<Uniform, UniformLoc> uniformLocation;
	std::map<Uniform, GLuint>   paramI1;
	std::map<Uniform, float>   paramV1;
	std::map<Uniform, Vector2> paramV2;
	std::map<Uniform, Vector3> paramV3;
	std::map<Uniform, Matrix4> paramM4;
	UniformLoc uniformLoc(Uniform u);
public:
	Material();
	Material(Resource vs, Resource fs);
	void setParam(Uniform name, Texture value);
	void setParam(Uniform name, float value);
	void setParam(Uniform name, Vector2 value);
	void setParam(Uniform name, Vector3 value);
	void setParam(Uniform name, Matrix4 value);
	void setParam(Uniform name, Transform value);
	void use();
};

struct DefaultUniform {
	Vector3 C;
	Transform V;
	Matrix4 P;
};

class Object {
	Mesh mesh;
	Material mat;
	DefaultUniform &uni;
public:
	Object(DefaultUniform &uni);
	void build(Model model, Material mat);
	template <class T> void setParam(Uniform name, T value) {
		mat.setParam(name, value);
	}
	void render();
};

class Render {
	float width, height;
	DefaultUniform uni;
	Texture bgTex;
	Object background, ball, hole, ground, holeSide, ballShadow;
	void buildObjects();
	void buildModels(Model &planeModel, Model &ballModel, Model &holeSideModel);
	void sendModelData(Model&, std::vector<Vector3>&);
public:
	Render(float w, float h);
	void setCamera(Transform);
	void drawBall(BallPos);
	void drawHole(HolePos);
	void drawBackground(Image);
};