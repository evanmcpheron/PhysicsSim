#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3
{
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    float magnitude() const;
    Vector3 normalized() const;
    float dot(const Vector3 &other) const;
    Vector3 cross(const Vector3 &other) const;

    Vector3 operator+(const Vector3 &other) const;
    Vector3 operator-(const Vector3 &other) const;
    Vector3 operator*(float scalar) const;
};

#endif // VECTOR3_H
