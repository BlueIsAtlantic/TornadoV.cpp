#include "MathEx.h"
#include <algorithm>
#include <cmath>

float MathEx::_cos[720];
float MathEx::_sin[720];

void MathEx::Initialize() {
    for (int i = 0; i < 360; i++) {
        _cos[i] = (float)cos(ToRadians(360 - i));
        _sin[i] = (float)sin(ToRadians(360 - i));
        _cos[i + 360] = (float)cos(ToRadians(i));
        _sin[i + 360] = (float)sin(ToRadians(i));
    }
}

float MathEx::Cos(double value) {
    int deg = (int)ToDegrees(value);
    return value < 0 ? _cos[std::clamp(-deg, 0, 359)] : _cos[std::clamp(deg + 360, 360, 719)];
}

float MathEx::Sin(double value) {
    int deg = (int)ToDegrees(value);
    return value < 0 ? _sin[std::clamp(-deg, 0, 359)] : _sin[std::clamp(deg + 360, 360, 719)];
}

Vector3 MathEx::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
    Vector3 a = Subtract(target, current);
    float magnitude = Length(a);
    if (magnitude <= maxDistanceDelta || magnitude == 0.0f) {
        return target;
    }
    return Add(current, Multiply(Divide(a, magnitude), maxDistanceDelta));
}

Vector3 MathEx::AnglesToForward(Vector3 position, Vector3 angles, float length) {
    float num = (float)sin(angles.x * M_PI / 180.0) * length;
    float num1 = (float)sqrt(length * length - num * num);
    float num2 = (float)sin(angles.y * M_PI / 180.0) * num1;
    float num3 = (float)cos(angles.y * M_PI / 180.0) * num1;
    Vector3 result;
    result.x = position.x + num3;
    result.y = position.y + num2;
    result.z = position.z - num;
    return result;
}

Quaternion MathEx::Euler(float x, float y, float z) {
    float halfPhi = 0.5f * x;   // Roll
    float halfTheta = 0.5f * y; // Pitch
    float halfPsi = 0.5f * z;   // Yaw

    float cosHalfPhi = cosf(halfPhi);
    float sinHalfPhi = sinf(halfPhi);
    float cosHalfTheta = cosf(halfTheta);
    float sinHalfTheta = sinf(halfTheta);
    float cosHalfPsi = cosf(halfPsi);
    float sinHalfPsi = sinf(halfPsi);

    // MATCH C# MathEx.cs: SHVDN Quaternion(x, y, z, w) constructor
    // Formula for Hamilton product of three rotations (Roll-Pitch-Yaw)
    return {
        cosHalfPhi * cosHalfTheta * cosHalfPsi - sinHalfPhi * sinHalfTheta * sinHalfPsi, // X
        sinHalfPhi * cosHalfTheta * cosHalfPsi + cosHalfPhi * sinHalfTheta * sinHalfPsi, // Y
        cosHalfPhi * sinHalfTheta * cosHalfPsi - sinHalfPhi * cosHalfTheta * sinHalfPsi, // Z
        cosHalfPhi * cosHalfTheta * sinHalfPsi + sinHalfPhi * sinHalfTheta * cosHalfPsi  // W
    };
}

Vector3 MathEx::MultiplyVector(Vector3 vec, Quaternion quat) {
    float num = quat.x * 2.0f;
    float num2 = quat.y * 2.0f;
    float num3 = quat.z * 2.0f;
    float num4 = quat.x * num;
    float num5 = quat.y * num2;
    float num6 = quat.z * num3;
    float num7 = quat.x * num2;
    float num8 = quat.x * num3;
    float num9 = quat.y * num3;
    float num10 = quat.w * num;
    float num11 = quat.w * num2;
    float num12 = quat.w * num3;

    return {
        (1.0f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z, 0,
        (num7 + num12) * vec.x + (1.0f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z, 0,
        (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.0f - (num4 + num5)) * vec.z, 0
    };
}

float MathEx::Length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 MathEx::Normalize(Vector3 v) {
    float len = Length(v);
    if (len == 0) return { 0, 0, 0, 0, 0, 0 };
    return Divide(v, len);
}

float MathEx::Distance(Vector3 a, Vector3 b) {
    return Length(Subtract(a, b));
}

float MathEx::Distance2D(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

Vector3 MathEx::Cross(Vector3 a, Vector3 b) {
    return {
        a.y * b.z - a.z * b.y, 0,
        a.z * b.x - a.x * b.z, 0,
        a.x * b.y - a.y * b.x, 0
    };
}

Vector3 MathEx::Add(Vector3 a, Vector3 b) {
    return { a.x + b.x, 0, a.y + b.y, 0, a.z + b.z, 0 };
}

Vector3 MathEx::Subtract(Vector3 a, Vector3 b) {
    return { a.x - b.x, 0, a.y - b.y, 0, a.z - b.z, 0 };
}

Vector3 MathEx::Multiply(Vector3 a, float b) {
    return { a.x * b, 0, a.y * b, 0, a.z * b, 0 };
}

Vector3 MathEx::Divide(Vector3 a, float b) {
    return { a.x / b, 0, a.y / b, 0, a.z / b, 0 };
}

Vector3 MathEx::Lerp(Vector3 a, Vector3 b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return Add(a, Multiply(Subtract(b, a), t));
}
