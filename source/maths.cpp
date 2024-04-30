#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <cmath>

#include "maths.hpp"

// Quaternion constructors
Quaternion::Quaternion() {}

Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

// Quaternion to rotation matrix conversion
glm::mat4 Quaternion::quatToMat()
{
    float s = 2.0f / (w * w + x * x + y * y + z * z);
    float xs = x * s, ys = y * s, zs = z * s;
    float xx = x * xs, xy = x * ys, xz = x * zs;
    float yy = y * ys, yz = y * zs, zz = z * zs;
    float xw = w * xs, yw = w * ys, zw = w * zs;

    glm::mat4 R = glm::mat4(1.0f);
    R[0][0] = 1.0f - (yy + zz); R[0][1] = xy + zw;          R[0][2] = xz - yw;
    R[1][0] = xy - zw;          R[1][1] = 1.0f - (xx + zz); R[1][2] = yz + xw;
    R[2][0] = xz + yw;          R[2][1] = yz - xw;          R[2][2] = 1.0f - (xx + yy);

    return R;
}

// Euler angles to quaternion
void Quaternion::eulerToQuat(const float pitch, const float yaw, const float roll)
{
    float cr, cp, cy, sr, sp, sy;
    cr = cos(0.5f * roll);
    cp = cos(0.5f * pitch);
    cy = cos(0.5f * yaw);
    sr = sin(0.5f * roll);
    sp = sin(0.5f * pitch);
    sy = sin(0.5f * yaw);

    Quaternion q;
    w = cp * cr * cy - sp * sr * sy;
    x = sp * cr * cy + cp * sr * sy;
    y = cp * cr * sy - sp * sr * cy;
    z = cp * sr * cy + sp * cr * sy;
}


// Convert an angle from degrees to radians
float Maths::radians(const float angle)
{
    return angle * 3.1415927f / 180.0f;
}

// Calculate the norm of a vector
float Maths::norm(const glm::vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalise a vector
glm::vec3 Maths::normalize(const glm::vec3 v)
{
    return v / Maths::norm(v);
}

// Calculate the translation matrix
glm::mat4 Maths::translate(const glm::mat4 mat, const glm::vec3 t)
{
    glm::mat4 T = glm::mat4(1.0f);
    T[3][0] = t.x; T[3][1] = t.y; T[3][2] = t.z;
    
    return T * mat;
}

// Calculate the scaling matrix
glm::mat4 Maths::scale(const glm::mat4 mat, const glm::vec3 s)
{
    glm::mat4 S = glm::mat4(1.0f);
    S[0][0] = s.x; S[1][1] = s.y; S[2][2] = s.z;
    
    return S * mat;
}

glm::mat4 Maths::rotate(const glm::mat4 mat, const float angle, const glm::vec3 vec)
{
    glm::vec3 v = Maths::normalize(vec);
    float cs = cos(0.5f * angle);
    float sn = sin(0.5f * angle);
    Quaternion q(cs, sn * v.x, sn * v.y, sn * v.z);

    return q.quatToMat() * mat;
}

// SLERP
Quaternion Maths::slerp(Quaternion q1, Quaternion q2, const float t)
{
    // Check if we are going the "long" way around the sphere
    float q1DotQ2 = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    if (q1DotQ2 < 0)
    {
        // Change signs of q2 to ensure we go the short way round
        q2.w *= -q2.w;
        q2.x *= -q2.x;
        q2.y *= -q2.y;
        q2.z *= -q2.z;
        q1DotQ2 = -q1DotQ2;
    }

    // Calculate angle between quaternions q1 and q2
    float theta = acos(q1DotQ2);

    // Calculate interpolated quaternion qt
    Quaternion qt;
    float denom = sin(theta);
    if (denom > 0.001f)
    {
        // Use SLERP
        float fact1 = sin((1.0f - t) * theta);
        float fact2 = sin(t * theta);
        qt.w = fact1 * q1.w + fact2 * q2.w;
        qt.x = fact1 * q1.x + fact2 * q2.x;
        qt.y = fact1 * q1.y + fact2 * q2.y;
        qt.z = fact1 * q1.z + fact2 * q2.z;
    }
    else
    {
        // Use LERP if sin(theta) is small
        qt.w = (1.0f - t) * q1.w + t * q2.w;
        qt.x = (1.0f - t) * q1.x + t * q2.x;
        qt.y = (1.0f - t) * q1.y + t * q2.y;
        qt.z = (1.0f - t) * q1.z + t * q2.z;
    }

    return qt;
}

//// Calculate the rotation matrix
//glm::mat4 Maths::rotate(const glm::mat4 mat, const float angle, const glm::vec3 vector)
//{
//    glm::vec3 v = Maths::normalize(vector);
//    float cs = cos(angle);
//    float sn = sin(angle);
//    
//    glm::mat4 R = glm::mat4(1.0f);
//    R[0][0] = v.x * v.x * (1 - cs) + cs;
//    R[0][1] = v.x * v.y * (1 - cs) + v.z * sn;
//    R[0][2] = v.x * v.z * (1 - cs) - v.y * sn;
//    R[1][0] = v.y * v.x * (1 - cs) - v.z * sn;
//    R[1][1] = v.y * v.y * (1 - cs) + cs;
//    R[1][2] = v.y * v.z * (1 - cs) + v.x * sn;
//    R[2][0] = v.z * v.x * (1 - cs) + v.y * sn;
//    R[2][1] = v.z * v.y * (1 - cs) - v.x * sn;
//    R[2][2] = v.z * v.z * (1 - cs) + cs;
//    return R * mat;
//
//}
