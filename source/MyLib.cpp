#include "MyLib.h"
#include <glm/glm.hpp>

float MyLib::Dot(glm::vec2 a, glm::vec2 b) {
	return (a[0] * b[0] + a[1] * b[1]);
}

float MyLib::Dot(glm::vec3 a, glm::vec3 b) {
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

float MyLib::Length(glm::vec3 vec3) {
	return sqrt((vec3[0] * vec3[0]) + (vec3[1] * vec3[1]) + (vec3[2] * vec3[2]));
}

float MyLib::Radians(float degrees) {
	return (PI / 180.0f) * degrees;
}

float MyLib::Degrees(float radians) {
	return (180.0f / PI) * radians;
}

//---------------------------------------------------

glm::vec3 MyLib::Cross(glm::vec3 a, glm::vec3 b) {
	return glm::vec3((a[1] * b[2] - a[2] * b[1]), (a[2] * b[0] - a[0] * b[2]), (a[0] * b[1] - a[1] * b[0]));
}

glm::vec3 MyLib::Normalise(glm::vec3 vec3) {
	return (vec3 / MyLib::Length(vec3));
}

glm::vec3 MyLib::Unit(glm::vec3 vec3) {
	return (vec3 / MyLib::Length(vec3));
}

//---------------------------------------------------

glm::mat2 MyLib::Inverse(glm::mat2 mat) {
	//A^-1 = (ad - bc)^-1 * |d -b|
	//						|-c a|

	glm::mat2 A = glm::mat2(1.0f);
	A[0][0] = mat[1][1], A[1][0] = -mat[1][0];
	A[0][1] = -mat[0][1], A[1][1] = mat[0][0];
	A *= 1 / (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]);

	return A;
}

glm::mat2 MyLib::Multiply(glm::mat2 a, glm::mat2 b) {
	//OpenGL does their matrices in column-first order.
	//reglar equation =
	// |a1 a2| x |b1 b2| = | ((a1,a2) . (b1,b3)) ((a1,a2) . (b2,b4)) |
	// |a3 a4|   |b3 b4|   | ((a3,a4) . (b1,b3)) ((a3,a4) . (b2,b4)) |
	glm::mat2 A = glm::mat2(1.0f);

	float result[] = {MyLib::Dot(glm::vec2(a[0][0], a[1][0]), glm::vec2(b[0][0], b[0][1])), //top left
					  MyLib::Dot(glm::vec2(a[0][1], a[1][1]), glm::vec2(b[0][0], b[0][1])), //bottom left
					  MyLib::Dot(glm::vec2(a[0][0], a[1][0]), glm::vec2(b[1][0], b[1][1])), //top right
					  MyLib::Dot(glm::vec2(a[0][1], a[1][1]), glm::vec2(b[1][0], b[1][1])) };//bottom right
	A[0][0] = result[0];
	A[0][1] = result[1];
	A[1][0] = result[2];
	A[1][1] = result[3];
	
	return A;
}

glm::mat2 MyLib::Transpose(glm::mat2 mat) {
	glm::mat2 A = glm::mat2(1.0f);
	A[0][0] = mat[0][0], A[0][1] = mat[1][0];
	A[1][0] = mat[0][1], A[1][1] = mat[1][1];
	return A;
}

//---------------------------------------------------



glm::mat4 MyLib::Translate(glm::mat4 ApplyTo, glm::vec3 coords) {
	glm::mat4 A = glm::mat4(1.0f);
	A[3][0] = coords.x, A[3][1] = coords.y, A[3][2] = coords.z;
	A[3][3] = 1.0f;

	return ApplyTo * A;
}

glm::mat4 MyLib::Scale(glm::mat4 ApplyTo, glm::vec3 scale) {
	glm::mat4 A = glm::mat4(1.0f);
	A[0][0] = scale.x, A[1][1] = scale.y, A[2][2] = scale.z;
	A[3][3] = 1.0f;

	return ApplyTo * A;
}

glm::mat4 MyLib::Rotate(glm::mat4 ApplyTo, float radians, glm::vec3 rotationVector) {

	glm::mat4 R = glm::mat4(1.0f);
	glm::vec3 RotUnit = MyLib::Normalise(rotationVector);

	const float sinA = sin(radians);
	const float cosA = cos(radians);
	const float oneMinusCos = (1 - cosA);	

	//top row
	R[0][0] = cosA + (RotUnit.x * RotUnit.x) * oneMinusCos;
	R[0][1] = RotUnit.x * RotUnit.y * oneMinusCos + sinA * RotUnit.z;
	R[0][2] = RotUnit.x * RotUnit.z * oneMinusCos - sinA * RotUnit.y;

	//middle row
	R[1][0] = RotUnit.y * RotUnit.x * oneMinusCos - sinA * RotUnit.z;
	R[1][1] = cosA + (RotUnit.y * RotUnit.y) * oneMinusCos;
	R[1][2] = RotUnit.y * RotUnit.z * oneMinusCos + sinA * RotUnit.x;

	//bottom row
	R[2][0] = RotUnit.z * RotUnit.x * oneMinusCos + sinA * RotUnit.y;
	R[2][1] = RotUnit.z * RotUnit.y * oneMinusCos - sinA * RotUnit.x;
	R[2][2] = cosA + (RotUnit.z * RotUnit.z) * oneMinusCos;


	//redundant
	//last column
	//R[3][0], R[3][1], R[3][2] = 0.0f;
	//last row
	//R[0][3], R[1][3], R[2][3] = 0.0f;
	//R[3][3] = 1.0f; //manditory homogenous co-ordinate value

	glm::mat4 Result = glm::mat4(1.0f);
	Result[0] = ApplyTo[0] * R[0][0] + ApplyTo[1] * R[0][1] + ApplyTo[2] * R[0][2];
	Result[1] = ApplyTo[0] * R[1][0] + ApplyTo[1] * R[1][1] + ApplyTo[2] * R[1][2];
	Result[2] = ApplyTo[0] * R[2][0] + ApplyTo[1] * R[2][1] + ApplyTo[2] * R[2][2];
	Result[3] = ApplyTo[3];
	return Result;
}

glm::mat4 MyLib::Transform(glm::mat4 ApplyTo, glm::vec3 position, glm::vec3 rotationVector, float radians, glm::vec3 scaleFactor) {
	return MyLib::Translate(ApplyTo,position) * MyLib::Rotate(ApplyTo, radians, rotationVector) * MyLib::Scale(ApplyTo,scaleFactor);
}

//---------------------------------------------------

glm::mat4 MyLib::LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
	// Calculate view matrix
	//glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 0.0f);
	//glm::vec3 target = glm::vec3(0.0f, 0.0f, -4.0f);
	//glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 cameraFront = MyLib::Normalise(target - position);
	glm::vec3 cameraRight = MyLib::Normalise(MyLib::Cross(cameraFront, worldUp));
	glm::vec3 cameraUp = MyLib::Normalise(MyLib::Cross(cameraRight, cameraFront));

	glm::mat4 align = glm::mat4(1.0f);

	glm::mat4 translate = MyLib::Translate(glm::mat4(1.0f), -position);
	for (unsigned int i = 0; i < 3; i++)
	{
		//translate[3][i] = -cameraPos[i];
		align[i][0] = cameraRight[i];
		align[i][1] = cameraUp[i];
		align[i][2] = -cameraFront[i];
	}
	

	glm::mat4 view = align * translate;
	return view;
}


glm::mat4 MyLib::Perspective(float fov, float aspect, float near, float far) {
	// Calculate perspective projection matrix
	float top, right;
	//float right, top, near, far, fov, aspect;
	//fov = glm::radians(45.0f);
	//aspect = 1024.0f / 768.0f;
	//near = 0.2f;
	//far = 100.0f;
	top = near * glm::tan(fov / 2);
	right = aspect * top;

	glm::mat4 projection = glm::mat4(1.0f);
	projection[0][0] = near / right;
	projection[1][1] = near / top;
	projection[2][2] = -(far + near) / (far - near);
	projection[2][3] = -1.0f;
	projection[3][2] = -2 * far * near / (far - near);;
	projection[3][3] = 0.0f;
	return projection;
}