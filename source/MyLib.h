#pragma once
#include <glm/glm.hpp>
static const float PI = 3.14159265359f;

static class MyLib
{

public:

	//Misc (Vec2 & Vec3)
	static float Dot(glm::vec2 a, glm::vec2 b);
	static float Dot(glm::vec3 a, glm::vec3 b);
	static float Length(glm::vec3 vec3);
	static float Radians(float degrees);
	static float Degrees(float radians);

	//VECTOR
	static glm::vec3 Cross(glm::vec3 a, glm::vec3 b);
	static glm::vec3 Normalise(glm::vec3 vec3);
	static glm::vec3 Unit(glm::vec3 vec3);

	//2x2 Matrix
	static glm::mat2 Inverse(glm::mat2 mat);
	static glm::mat2 Multiply(glm::mat2 a, glm::mat2 b); //proof of concept, redundant in practise due to glm library operator overrides
	static glm::mat2 Transpose(glm::mat2 mat);


	//(LEGACY) 3D Matrix Transforms
	//these do not use Quarterions and suffer from Gimbal lock, they are not used in the final version of the project.
	static glm::mat4 Translate(glm::mat4 ApplyTo, glm::vec3 coords);
	static glm::mat4 Scale(glm::mat4 ApplyTo, glm::vec3 scale);
	static glm::mat4 Rotate(glm::mat4 ApplyTo, float radians, glm::vec3 rotationVector);
	static glm::mat4 Transform(glm::mat4 ApplyTo, glm::vec3 position, glm::vec3 rotationVector, float radians, glm::vec3 scale);


	//3D Projection
	static glm::mat4 LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);
	static glm::mat4 Perspective(float fov, float aspect, float near, float far);

};

