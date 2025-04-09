#pragma once
#include <cmath>
#include <algorithm>

struct Vector3
{
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    double Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 Normalized() const
    {
        double len = Length();
        return (len > 0.0) ? Vector3(x / len, y / len, z / len) : Vector3(0, 0, 0);
    }

    double Dot(const Vector3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 Cross(const Vector3 &other) const
    {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    Vector3 operator*(double scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator+(const Vector3 &other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3 &other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    double AngleBetween(const Vector3 &other) const
    {
        double dot = Dot(other);
        double lenProduct = Length() * other.Length();
        if (lenProduct == 0.0)
            return 0.0;

        double clamped = std::clamp(dot / lenProduct, -1.0, 1.0);
        return std::acos(clamped);
    }
};
