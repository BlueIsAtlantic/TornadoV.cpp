#pragma once

#include <cmath>
#include <vector>
#include "types.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Quaternion {
    float x, y, z, w;
};

class MathEx {
public:
    static constexpr double RadToDeg = 180.0 / M_PI;
    static constexpr double DegToRad = M_PI / 180.0;

    static void Initialize();
    static float Cos(double value);
    static float Sin(double value);
    
    static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta);
    static Vector3 AnglesToForward(Vector3 position, Vector3 angles, float length);
    static Vector3 RotationToDirection(Vector3 rotation);
    static Quaternion Euler(float x, float y, float z);
    static Vector3 MultiplyVector(Vector3 vec, Quaternion quat);

    // Vector3 Helpers
    static float Length(Vector3 v);
    static Vector3 Normalize(Vector3 v);
    static float Distance(Vector3 a, Vector3 b);
    static float Distance2D(Vector3 a, Vector3 b);
    static Vector3 Cross(Vector3 a, Vector3 b);
    static Vector3 Add(Vector3 a, Vector3 b);
    static Vector3 Subtract(Vector3 a, Vector3 b);
    static Vector3 Multiply(Vector3 a, float b);
    static Vector3 Divide(Vector3 a, float b);
    static Vector3 Lerp(Vector3 a, Vector3 b, float t);

private:
    static float _cos[720];
    static float _sin[720];
};

inline double ToRadians(double val) { return val * MathEx::DegToRad; }
inline double ToDegrees(double val) { return val * MathEx::RadToDeg; }
