#ifndef VOXEL_DYNAMICS_MATH
#define VOXEL_DYNAMICS_MATH

#include <cstring>
#include <math.h>
#include <stdint.h>
#define PI 3.141592653589793f

typedef unsigned int VDuint;

struct VDVector2
{
    float x, y;

    VDVector2() : x(0.0f), y(0.0f) {}

    VDVector2(float _x, float _y) : x(_x), y(_y){}

    VDVector2(const VDVector2& other) : x(other.x), y(other.y){}

    VDVector2& operator=(const VDVector2& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    VDVector2 operator+(const VDVector2& other) const
    {
        return VDVector2(x + other.x, y + other.y);
    }

    VDVector2& operator+=(const VDVector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    VDVector2 operator-(const VDVector2& other) const
    {
        return VDVector2(x - other.x, y - other.y);
    }

    VDVector2 operator*(float scalar) const
    {
        return VDVector2(x * scalar, y * scalar);
    }

};

struct VDVector3i;

struct VDVector3
{
    float x, y, z;

    VDVector3() : x(0.0f), y(0.0f), z(0.0f) {}

    VDVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    VDVector3(const VDVector3& other) : x(other.x), y(other.y), z(other.z) {}

    VDVector3(const VDVector3i& other);

    VDVector3& operator=(const VDVector3& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    VDVector3& operator=(const VDVector3i& other);

    VDVector3 operator+(const VDVector3& other) const
    {
        return VDVector3(x + other.x, y + other.y, z + other.z);
    }

    VDVector3& operator+=(const VDVector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    VDVector3 operator-(const VDVector3& other) const
    {
        return VDVector3(x - other.x, y - other.y, z - other.z);
    }

    VDVector3 operator*(float scalar) const
    {
        return VDVector3(x * scalar, y * scalar, z * scalar);
    }


    VDVector3 operator*(const VDVector3& other) const
    {
        return VDVector3(x * other.x, y * other.y, z * other.z);
    }

    bool operator==(const VDVector3& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    float length()
    {
        return sqrtf(x * x + y * y + z * z);
    }

    void normalize()
    {
        float l = length();
        if (l > 0.0f)
        {
            x /= l;
            y /= l;
            z /= l;
        }
        else
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
    }

    static VDVector3 right()
    {
        return VDVector3(1.0f, 0.0f, 0.0f);
    }

    static VDVector3 up()
    {
        return VDVector3(0.0f, 1.0f, 0.0f);
    }

    static VDVector3 forward()
    {
        return VDVector3(0.0f, 0.0f, 1.0f);
    }

    static VDVector3 left()
    {
        return VDVector3(-1.0f, 0.0f, 0.0f);
    }

    static VDVector3 down()
    {
        return VDVector3(0.0f, -1.0f, 0.0f);
    }

    static VDVector3 back()
    {
        return VDVector3(0.0f, 0.0f, -1.0f);
    }

    static VDVector3 zero()
    {
        return VDVector3(0.0f, 0.0f, 0.0f);
    }

    static VDVector3 one()
    {
        return VDVector3(1.0f, 1.0f, 1.0f);
    }

    static VDVector3 half()
    {
        return VDVector3(0.5f, 0.5f, 0.5f);
    }

    static VDVector3 uniformScale(float s)
    {
        return VDVector3(s, s, s);
    }

    VDVector3 xComponentVector() const
    {
        return VDVector3(x, 0, 0);
    }

    VDVector3 yComponentVector() const
    {
        return VDVector3(0, y, 0);
    }

    VDVector3 zComponentVector() const
    {
        return VDVector3(0, 0, z);
    }
};

struct VDVector3i
{
    int x, y, z;

    VDVector3i() : x(0), y(0), z(0) {}

    VDVector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

    VDVector3i(const VDVector3i& other) : x(other.x), y(other.y), z(other.z) {}

    VDVector3i(const VDVector3& other) : x((int)other.x), y((int)other.y), z((int)other.z) {}

    VDVector3i& operator=(const VDVector3i& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    VDVector3i operator+(const VDVector3i& other) const
    {
        return VDVector3i(x + other.x, y + other.y, z + other.z);
    }

    VDVector3i& operator+=(const VDVector3i& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    VDVector3i operator-(const VDVector3i& other) const
    {
        return VDVector3i(x - other.x, y - other.y, z - other.z);
    }

    VDVector3i operator*(const int& i) const
    {
        return VDVector3i(x * i, y * i, z * i);
    }

    VDVector3i operator/(const int& i) const
    {
        return VDVector3i(x / i, y / i, z / i);
    }
};

VDVector3& VDVector3::operator=(const VDVector3i& other) 
{
    // Assuming VDVector3i has x, y, z as public members of type int
    this->x = static_cast<float>(other.x);
    this->y = static_cast<float>(other.y);
    this->z = static_cast<float>(other.z);
    return *this;
}

VDVector3::VDVector3(const VDVector3i& other)
{
    this->x = static_cast<float>(other.x);
    this->y = static_cast<float>(other.y);
    this->z = static_cast<float>(other.z);
}

float VDAbs(float f)
{
    if (f < 0.0f)
        f *= -1.0f;
    return f;
}

float VDSign(float f)
{
    if (f < 0.0f)
        return -1.0f;
    if (f == 0.0f)
        return 0.0f;
    return 1.0f;
}


float VDAnd(float f1, float f2)
{
    if (VDSign(f1) != VDSign(f2))
        return 0.0f;
    return VDSign(f1 + f2);
}

VDVector3 VDAbs(VDVector3 v)
{
    return VDVector3(VDAbs(v.x), VDAbs(v.y), VDAbs(v.z));
}

VDVector3 VDAnd(VDVector3 v1, VDVector3 v2)
{
    return VDVector3(VDAnd(v1.x, v2.x), VDAnd(v1.y, v2.y), VDAnd(v1.z, v2.z));
}


VDVector3 VDSign(VDVector3 v)
{
    return VDVector3(VDSign(v.x), VDSign(v.y), VDSign(v.z));
}

float VDMax(float f1, float f2)
{
    if (f1 > f2)
        return f1;
    return f2;
}

VDVector3 VDMax(VDVector3 v1, VDVector3 v2)
{
    return VDVector3(VDMax(v1.x, v2.x), VDMax(v1.y, v2.y), VDMax(v1.z, v2.z));
}

VDuint VDMax(VDuint i1, VDuint i2)
{
    if (i1 > i2)
        return i1;
    return i2;
}

float VDMin(float f1, float f2)
{
    if (f1 < f2)
        return f1;
    return f2;
}

int VDMin(int i1, int i2)
{
    if (i1 < i2)
        return i1;
    return i2;
}

VDVector3 VDMin(VDVector3 v1, VDVector3 v2)
{
    return VDVector3(VDMin(v1.x, v2.x), VDMin(v1.y, v2.y), VDMin(v1.z, v2.z));
}

int VDMax(int i1, int i2)
{
    if (i1 > i2)
        return i1;
    return i2;
}

VDuint VDMin(VDuint i1, VDuint i2)
{
    if (i1 < i2)
        return i1;
    return i2;
}

VDVector3i VDMin(VDVector3i v1, VDVector3i v2)
{
    return VDVector3i(VDMin(v1.x, v2.x), VDMin(v1.y, v2.y), VDMin(v1.z, v2.z));
}

VDVector3i VDMax(VDVector3i v1, VDVector3i v2)
{
    return VDVector3i(VDMax(v1.x, v2.x), VDMax(v1.y, v2.y), VDMax(v1.z, v2.z));
}

bool VDNan(float f)
{
    return isnan(f);
}

bool VDNanAny(VDVector3 v)
{
    return (VDNan(v.x) || VDNan(v.y) || VDNan(v.z));
}

bool VDNanAll(VDVector3 v)
{
    return (VDNan(v.x) && VDNan(v.y) && VDNan(v.z));
}


VDVector3 VDAdd(VDVector3 v1, VDVector3 v2)
{
	return VDVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

VDVector3 VDSubtract(VDVector3 v1, VDVector3 v2)
{
	return VDVector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

VDVector3 VDScale(VDVector3 v, float scalar)
{
	return VDVector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

VDVector3 VDCross(VDVector3 v1, VDVector3 v2)
{
    return VDVector3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}

VDVector3 VDNormalize(VDVector3 v)
{
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length != 0.0f)
    {
        float invLength = 1.0f / length;
        return VDVector3(v.x * invLength, v.y * invLength, v.z * invLength);
    }
    return v;
}

float VDDot(VDVector3 v1, VDVector3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

VDVector3 VDNormalComponent(VDVector3 vector, VDVector3 planeNormal)
{
    return planeNormal * VDDot(vector, planeNormal);
}

VDVector3 VDTangentialComponent(VDVector3 vector, VDVector3 planeNormal)
{
    return vector - VDNormalComponent(vector, planeNormal);
}

struct VDMatrix
{
    float m[4][4];

    VDMatrix()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    VDMatrix(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }
    VDMatrix(float mat[4][4])
    {
        std::memcpy(m, mat, sizeof(m));
    }

    VDMatrix(const VDMatrix& other)
    {
        std::memcpy(m, other.m, sizeof(m));
    }

    VDMatrix& operator=(const VDMatrix& other)
    {
        if (this != &other)
        {
            std::memcpy(m, other.m, sizeof(m));
        }
        return *this;
    }

    VDMatrix operator*(const VDMatrix& other) const
    {
        VDMatrix result;

        result.m[0][0] = m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0] + m[0][3] * other.m[3][0];
        result.m[0][1] = m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1] + m[0][3] * other.m[3][1];
        result.m[0][2] = m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2] + m[0][3] * other.m[3][2];
        result.m[0][3] = m[0][0] * other.m[0][3] + m[0][1] * other.m[1][3] + m[0][2] * other.m[2][3] + m[0][3] * other.m[3][3];

        result.m[1][0] = m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0] + m[1][3] * other.m[3][0];
        result.m[1][1] = m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1] + m[1][3] * other.m[3][1];
        result.m[1][2] = m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2] + m[1][3] * other.m[3][2];
        result.m[1][3] = m[1][0] * other.m[0][3] + m[1][1] * other.m[1][3] + m[1][2] * other.m[2][3] + m[1][3] * other.m[3][3];

        result.m[2][0] = m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0] + m[2][3] * other.m[3][0];
        result.m[2][1] = m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1] + m[2][3] * other.m[3][1];
        result.m[2][2] = m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2] + m[2][3] * other.m[3][2];
        result.m[2][3] = m[2][0] * other.m[0][3] + m[2][1] * other.m[1][3] + m[2][2] * other.m[2][3] + m[2][3] * other.m[3][3];

        result.m[3][0] = m[3][0] * other.m[0][0] + m[3][1] * other.m[1][0] + m[3][2] * other.m[2][0] + m[3][3] * other.m[3][0];
        result.m[3][1] = m[3][0] * other.m[0][1] + m[3][1] * other.m[1][1] + m[3][2] * other.m[2][1] + m[3][3] * other.m[3][1];
        result.m[3][2] = m[3][0] * other.m[0][2] + m[3][1] * other.m[1][2] + m[3][2] * other.m[2][2] + m[3][3] * other.m[3][2];
        result.m[3][3] = m[3][0] * other.m[0][3] + m[3][1] * other.m[1][3] + m[3][2] * other.m[2][3] + m[3][3] * other.m[3][3];

        return result;
    }
};

VDMatrix VDTranslation(VDVector3 position)
{
    VDMatrix translationMatrix;

    translationMatrix.m[0][0] = 1.0f; translationMatrix.m[1][0] = 0.0f; translationMatrix.m[2][0] = 0.0f; translationMatrix.m[3][0] = position.x;
    translationMatrix.m[0][1] = 0.0f; translationMatrix.m[1][1] = 1.0f; translationMatrix.m[2][1] = 0.0f; translationMatrix.m[3][1] = position.y;
    translationMatrix.m[0][2] = 0.0f; translationMatrix.m[1][2] = 0.0f; translationMatrix.m[2][2] = 1.0f; translationMatrix.m[3][2] = position.z;
    translationMatrix.m[0][3] = 0.0f; translationMatrix.m[1][3] = 0.0f; translationMatrix.m[2][3] = 0.0f; translationMatrix.m[3][3] = 1.0f;

    return translationMatrix;
}

VDMatrix VDRotate(VDVector3 eulerAngles)
{
    float radX = eulerAngles.x;
    float radY = eulerAngles.y;
    float radZ = eulerAngles.z;

    float cosX = cosf(radX);
    float sinX = sinf(radX);
    float cosY = cosf(radY);
    float sinY = sinf(radY);
    float cosZ = cosf(radZ);
    float sinZ = sinf(radZ);

    VDMatrix rotationMatrix;

    rotationMatrix.m[0][0] = cosY * cosZ;
    rotationMatrix.m[1][0] = -cosY * sinZ;
    rotationMatrix.m[2][0] = sinY;
    rotationMatrix.m[3][0] = 0.0f;

    rotationMatrix.m[0][1] = cosX * sinZ + cosZ * sinX * sinY;
    rotationMatrix.m[1][1] = cosX * cosZ - sinX * sinY * sinZ;
    rotationMatrix.m[2][1] = -cosY * sinX;
    rotationMatrix.m[3][1] = 0.0f;

    rotationMatrix.m[0][2] = sinX * sinZ - cosX * cosZ * sinY;
    rotationMatrix.m[1][2] = cosZ * sinX + cosX * sinY * sinZ;
    rotationMatrix.m[2][2] = cosX * cosY;
    rotationMatrix.m[3][2] = 0.0f;

    rotationMatrix.m[0][3] = 0.0f;
    rotationMatrix.m[1][3] = 0.0f;
    rotationMatrix.m[2][3] = 0.0f;
    rotationMatrix.m[3][3] = 1.0f;

    return rotationMatrix;
}

VDMatrix VDScale(VDVector3 scale)
{
    VDMatrix scaleMatrix;

    scaleMatrix.m[0][0] = scale.x;
    scaleMatrix.m[1][0] = 0.0f;
    scaleMatrix.m[2][0] = 0.0f;
    scaleMatrix.m[3][0] = 0.0f;

    scaleMatrix.m[0][1] = 0.0f;
    scaleMatrix.m[1][1] = scale.y;
    scaleMatrix.m[2][1] = 0.0f;
    scaleMatrix.m[3][1] = 0.0f;

    scaleMatrix.m[0][2] = 0.0f;
    scaleMatrix.m[1][2] = 0.0f;
    scaleMatrix.m[2][2] = scale.z;
    scaleMatrix.m[3][2] = 0.0f;

    scaleMatrix.m[0][3] = 0.0f;
    scaleMatrix.m[1][3] = 0.0f;
    scaleMatrix.m[2][3] = 0.0f;
    scaleMatrix.m[3][3] = 1.0f;

    return scaleMatrix;
}

VDMatrix VDPerspective(float aspectRatio, float fovYRadians, float zNear, float zFar)
{
    float yScale = tanf(0.5f * ((float)PI - fovYRadians));
    float xScale = yScale / aspectRatio;
    float zRangeInverse = 1.0f / (zNear - zFar);
    float zScale = zFar * zRangeInverse;
    float zTranslation = zFar * zNear * zRangeInverse;
    return VDMatrix(
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, zScale, -1,
        0, 0, zTranslation, 0
    );
}

VDMatrix VDLookAt(VDVector3 eye, VDVector3 at, VDVector3 up)
{
    VDVector3 zAxis = VDNormalize(VDSubtract(eye, at));
    VDVector3 xAxis = VDNormalize(VDCross(up, zAxis));
    VDVector3 yAxis = VDCross(zAxis, xAxis);

    return VDMatrix(
        xAxis.x, yAxis.x, zAxis.x, 0,
        xAxis.y, yAxis.y, zAxis.y, 0,
        xAxis.z, yAxis.z, zAxis.z, 0,
        -VDDot(xAxis, eye), -VDDot(yAxis, eye), -VDDot(zAxis, eye), 1
    );
}

VDMatrix VDTranspose(VDMatrix mat)
{
    return VDMatrix(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

struct VDFrame
{
    VDVector3 right;
    VDVector3 up;
    VDVector3 forward;

    VDFrame()
    {
        right = VDVector3::right();
        up = VDVector3::up();
        forward = VDVector3::forward();
    }

    VDFrame(VDVector3 _right, VDVector3 _up, VDVector3 _forward)
    {
        right = _right;
        up = _up;
        forward = _forward;
    }

    VDFrame(const VDFrame& other)
    {
        right = other.right;
        up = other.up;
        forward = other.forward;
    }

    VDFrame& operator=(const VDFrame& other)
    {
        if (this != &other)
        {
            this->right = other.right;
            this->up = other.up;
            this->forward = other.forward;
        }

        return *this;
    }

    VDMatrix toRotationMatrix() const
    {
        VDMatrix mat;
        mat.m[0][0] = right.x;
        mat.m[0][1] = right.y;
        mat.m[0][2] = right.z;
        mat.m[1][0] = up.x;
        mat.m[1][1] = up.y;
        mat.m[1][2] = up.z;
        mat.m[2][0] = -forward.x;  // Negate forward for LH system
        mat.m[2][1] = -forward.y;  // Negate forward for LH system
        mat.m[2][2] = -forward.z;  // Negate forward for LH system
        return mat;
    }


    static VDFrame fromRotationMatrix(VDMatrix mat)
    {
        VDFrame frame(VDVector3(mat.m[0][0], mat.m[0][1], mat.m[0][2]),
            VDVector3(mat.m[1][0], mat.m[1][1], mat.m[1][2]),
            VDVector3(mat.m[2][0], mat.m[2][1], mat.m[2][2]));
    }

    void normalizeAxes()
    {
        right.normalize();
        up.normalize();
        forward.normalize();
    }
};

float VDMod(float x, float y)
{
    return x - y * (float)((int)(x / y));
}

struct VDQuaternion
{
	float w, x, y, z;

	VDQuaternion()
	{
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	VDQuaternion(float _w, float _x, float _y, float _z)
	{
		w = _w;
		x = _x;
		y = _y;
		z = _z;
	}

	VDQuaternion(const VDQuaternion& other)
	{
		w = other.w;
		x = other.x;
		y = other.y;
		z = other.z;
	}

	VDQuaternion& operator=(const VDQuaternion& other)
	{
		if (this != &other)
		{
			w = other.w;
			x = other.x;
			y = other.y;
			z = other.z;
		}

		return *this;
	}

	static VDQuaternion conjugate(const VDQuaternion& quat)
	{
		VDQuaternion retQuat = quat;
		retQuat.x = -quat.x;
		retQuat.y = -quat.y;
		retQuat.z = -quat.z;
		return retQuat;
	}

	void conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

    VDFrame toFrame()
    {
        VDFrame frame;

        // Calculate the squares of the components for easier reuse
        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        // Set the right vector (first column of the matrix)
        frame.right.x = 1.0f - 2.0f * (yy + zz);
        frame.right.y = 2.0f * (xy + wz);
        frame.right.z = 2.0f * (xz - wy);

        // Set the up vector (second column of the matrix)
        frame.up.x = 2.0f * (xy - wz);
        frame.up.y = 1.0f - 2.0f * (xx + zz);
        frame.up.z = 2.0f * (yz + wx);

        // Set the forward vector (third column of the matrix)
        frame.forward.x = 2.0f * (xz + wy);
        frame.forward.y = 2.0f * (yz - wx);
        frame.forward.z = 1.0f - 2.0f * (xx + yy);

        return frame;
    }

    VDMatrix toMatrix(bool rh = true) const
    {
        VDMatrix mr;

        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        if (rh)
        {
            // Invert the necessary signs to correct the handedness
            mr.m[0][0] = 1.0f - 2.0f * (yy + zz);
            mr.m[0][1] = 2.0f * (xy + wz);
            mr.m[0][2] = 2.0f * (xz - wy);
            mr.m[0][3] = 0.0f;

            mr.m[1][0] = 2.0f * (xy - wz);
            mr.m[1][1] = 1.0f - 2.0f * (xx + zz);
            mr.m[1][2] = 2.0f * (yz + wx);
            mr.m[1][3] = 0.0f;

            mr.m[2][0] = 2.0f * (xz + wy);
            mr.m[2][1] = 2.0f * (yz - wx);
            mr.m[2][2] = 1.0f - 2.0f * (xx + yy);
            mr.m[2][3] = 0.0f;

            mr.m[3][0] = 0.0f;
            mr.m[3][1] = 0.0f;
            mr.m[3][2] = 0.0f;
            mr.m[3][3] = 1.0f;
        }
        else
        {

            mr.m[0][0] = 1.0f - 2.0f * (yy + zz);
            mr.m[0][1] = 2.0f * (xy - wz);
            mr.m[0][2] = 2.0f * (xz + wy);
            mr.m[0][3] = 0.0f;

            mr.m[1][0] = 2.0f * (xy + wz);
            mr.m[1][1] = 1.0f - 2.0f * (xx + zz);
            mr.m[1][2] = 2.0f * (yz - wx);
            mr.m[1][3] = 0.0f;

            mr.m[2][0] = 2.0f * (xz - wy);
            mr.m[2][1] = 2.0f * (yz + wx);
            mr.m[2][2] = 1.0f - 2.0f * (xx + yy);
            mr.m[2][3] = 0.0f;

            mr.m[3][0] = 0.0f;
            mr.m[3][1] = 0.0f;
            mr.m[3][2] = 0.0f;
            mr.m[3][3] = 1.0f;
        }

        return mr;
    }


	static VDQuaternion fromAngleAxis(VDVector3 axis, float angle)
	{
		float sTheta = sinf(angle * 0.5f);
		float cTheta = cosf(angle * 0.5f);
		return VDQuaternion(cTheta, axis.x * sTheta, axis.y * sTheta, axis.z * sTheta);
	}

	static VDQuaternion fromEulerAngles(VDVector3 eulerAngles)
	{
		float cr = cos(eulerAngles.x * 0.5f);
		float sr = sin(eulerAngles.x * 0.5f);
		float cp = cos(eulerAngles.y * 0.5f);
		float sp = sin(eulerAngles.y * 0.5f);
		float cy = cos(eulerAngles.z * 0.5f);
		float sy = sin(eulerAngles.z * 0.5f);

		VDQuaternion qr;
		qr.w = cr * cp * cy + sr * sp * sy;
		qr.x = sr * cp * cy - cr * sp * sy;
		qr.y = cr * sp * cy + sr * cp * sy;
		qr.z = cr * cp * sy - sr * sp * cy;

		return qr;
	}

	void normalize()
	{
		float mag = sqrtf(w * w + x * x + y * y + z * z);
		w = w / mag;
		x = x / mag;
		y = y / mag;
		z = z / mag;
	}

	VDQuaternion operator*(const VDQuaternion& other)
	{
		VDQuaternion qr;
		qr.w = w * other.w - x * other.x - y * other.y - z * other.z;
		qr.x = w * other.x + x * other.w + y * other.z - z * other.y;
		qr.y = w * other.y - x * other.z + y * other.w + z * other.x;
		qr.z = w * other.z + x * other.y - y * other.x + z * other.w;
		return qr;
	}

	VDQuaternion operator+(const VDQuaternion& other)
	{
		VDQuaternion qr;
		qr.w = w + other.w;
		qr.x = x + other.x;
		qr.y = y + other.y;
		qr.z = z + other.z;
		return qr;
	}

	VDQuaternion operator-(const VDQuaternion& other)
	{
		VDQuaternion qr;
		qr.w = w - other.w;
		qr.x = x - other.x;
		qr.y = y - other.y;
		qr.z = z - other.z;
		return qr;
	}

	VDQuaternion operator*(const float s)
	{
		VDQuaternion qr;
		qr.w = w * s;
		qr.x = x * s;
		qr.y = y * s;
		qr.z = z * s;
		return qr;
	}

	void rotate(VDQuaternion rotation)
	{
		*this = rotation * (*this);
		normalize();
	}

	VDVector3 rotatePoint(VDVector3 point)
	{
		VDQuaternion rotatedQuat = (*this * VDQuaternion(0.0f, point.x, point.y, point.z)) * VDQuaternion::conjugate(*this);
		return VDVector3(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
	}

	static VDQuaternion fromFrame(VDFrame frame)
	{
		VDQuaternion quat;

		float trace = frame.right.x + frame.up.y + frame.forward.z;

		if (trace > 0)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			quat.w = 0.25f / s;
			quat.x = (frame.up.z - frame.forward.y) * s;
			quat.y = (frame.forward.x - frame.right.z) * s;
			quat.z = (frame.right.y - frame.up.x) * s;
		}
		else
		{
			if (frame.right.x > frame.up.y && frame.right.x > frame.forward.z)
			{
				float s = 2.0f * sqrtf(1.0f + frame.right.x - frame.up.y - frame.forward.z);
				quat.w = (frame.up.z - frame.forward.y) / s;
				quat.x = 0.25f * s;
				quat.y = (frame.up.x + frame.right.y) / s;
				quat.z = (frame.forward.x + frame.right.z) / s;
			}
			else if (frame.up.y > frame.forward.z)
			{
				float s = 2.0f * sqrtf(1.0f + frame.up.y - frame.right.x - frame.forward.z);
				quat.w = (frame.forward.x - frame.right.z) / s;
				quat.x = (frame.up.x + frame.right.y) / s;
				quat.y = 0.25f * s;
				quat.z = (frame.forward.y + frame.up.z) / s;
			}
			else
			{
				float s = 2.0f * sqrtf(1.0f + frame.forward.z - frame.right.x - frame.up.y);
				quat.w = (frame.right.y - frame.up.x) / s;
				quat.x = (frame.forward.x + frame.right.z) / s;
				quat.y = (frame.forward.y + frame.up.z) / s;
				quat.z = 0.25f * s;
			}
		}

		return quat;
	}

    static VDQuaternion lookAt(VDVector3 start, VDVector3 target, VDVector3 upDirection = VDVector3(0.0f, 1.0f, 0.0f))
    {
        // Calculate forward vector
        VDVector3 forward = VDNormalize(target-start);

            // Calculate right vector
        VDVector3 right = VDNormalize(VDCross(upDirection, forward));

        // Recalculate up vector to ensure orthogonality
        VDVector3 up = VDCross(forward,right);

        // Create a VDFrame using the calculated right, up, and forward vectors
        VDFrame frame;
        frame.right = right;
        frame.up = up;
        frame.forward = forward;

        // Convert the frame to a quaternion using the fromFrame method
        return VDQuaternion::fromFrame(frame);
    }
};

#endif