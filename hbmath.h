// hburd's personal math library.
// Version 0.1
// MIT License.
// See end of file for license text.

#ifndef HBMATH_INCLUDED
#define HBMATH_INCLUDED

#include <cstddef>

namespace hbmath
{

/////////////
// 2d math //
/////////////

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y);

    // Takes an array of two floats
    Vec2(float* data);

    float* array();
    const float* array() const;

    float square_magnitude() const;
    float magnitude() const;
    Vec2 normalize() const;

    Vec2& operator+=(const Vec2& rhs);
    Vec2& operator-=(const Vec2& rhs);
    Vec2& operator*=(float rhs);
    Vec2& operator/=(float rhs);
};

// This ensures that the memory layout of a Vec2 is the same as an array of its components.
// This is a requirement of various parts of the code, e.g. imgui.
static_assert(sizeof(Vec2) == 2 * sizeof(float), "The Vec2 type cannot be padded.");

struct Mat2
{
    float data[4] = {1.0f, 0.0f, 0.0f, 1.0f};

    Mat2() = default;
    Mat2(Vec2 left_column, Vec2 right_column);
    Mat2(float m00, float m01, float m10, float m11);

    static Mat2 Diagonal(const Vec2& diag);
    static Mat2 Rotation(float angle);

    Mat2 transpose() const;

    Mat2& operator*=(float rhs);
    Mat2& operator*=(const Mat2& rhs);
    Mat2& operator+=(const Mat2& rhs);
    Mat2& operator-=(const Mat2& rhs);
};

// Vector operations
float dot(const Vec2& lhs, const Vec2& rhs);

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(float lhs, const Vec2& rhs);
Vec2 operator*(const Vec2& lhs, float rhs);
Vec2 operator/(const Vec2& lhs, float rhs);
Vec2 operator-(const Vec2& operand);

// Matrix operations
Vec2 operator*(const Mat2& lhs, const Vec2& rhs);
Mat2 operator*(const Mat2& lhs, const Mat2& rhs);
Mat2 operator*(float lhs, const Mat2& rhs);
Mat2 operator*(const Mat2& lhs, float rhs);
Mat2 operator+(const Mat2& lhs, const Mat2& rhs);
Mat2 operator-(const Mat2& lhs, const Mat2& rhs);
Mat2 operator-(const Mat2& operand);


/////////////
// 3d Math //
/////////////

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_);

    // Takes an array of three floats
    Vec3(float* data);

    static Vec3 X(float x_);
    static Vec3 Y(float y_);
    static Vec3 Z(float z_);

    float* array();
    const float* array() const;

    float square_magnitude() const;
    float magnitude() const;
    Vec3 normalize() const;

    Vec3& operator+=(const Vec3& rhs);
    Vec3& operator-=(const Vec3& rhs);
    Vec3& operator*=(float rhs);
    Vec3& operator/=(float rhs);
};

// This ensures that the memory layout of a Vec3 is the same as an array of its components.
// This is a requirement of various parts of the code, e.g. imgui.
static_assert(sizeof(Vec3) == 3 * sizeof(float), "The Vec3 type cannot be padded.");

struct Mat3
{
    float data[9] = {1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f};

    Mat3() = default;
    Mat3(float m00, float m01, float m02,
         float m10, float m11, float m12,
         float m20, float m21, float m22);

    static Mat3 FromColumns(const Vec3& c0, const Vec3& c1, const Vec3& c2);
    static Mat3 FromRows(const Vec3& r0, const Vec3& r1, const Vec3& r2);
    static Mat3 Diagonal(const Vec3& diag);

    static Mat3 RotateX(float angle);
    static Mat3 RotateY(float angle);
    static Mat3 RotateZ(float angle);

    Mat3 transpose() const;

    Vec3 column(size_t index) const;
    Vec3 row(size_t index) const;

    Mat3& operator*=(float rhs);
    Mat3& operator*=(const Mat3& rhs);
    Mat3& operator+=(const Mat3& rhs);
    Mat3& operator-=(const Mat3& rhs);
};

struct Quaternion
{
    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Quaternion() = default;
    Quaternion(float w_, float x_, float y_, float z_);

    Quaternion normalize() const;

    Quaternion inverse() const;

    // TODO: use the Mat4 type
    void to_matrix(float* matrix);

    Vec3 apply_rotation(const Vec3& x) const;

    static Quaternion RotateX(float angle);
    static Quaternion RotateY(float angle);
    static Quaternion RotateZ(float angle);
};

// Vector operations
float dot(const Vec3& lhs, const Vec3& rhs);
Vec3 cross(const Vec3& lhs, const Vec3& rhs);

Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
Vec3 operator*(float lhs, const Vec3& rhs);
Vec3 operator*(const Vec3& lhs, float rhs);
Vec3 operator/(const Vec3& lhs, float rhs);
Vec3 operator-(const Vec3& operand);

// Matrix operations
Vec3 operator*(const Mat3& lhs, const Vec3& rhs);
Mat3 operator*(const Mat3& lhs, const Mat3& rhs);
Mat3 operator*(float lhs, const Mat3& rhs);
Mat3 operator*(const Mat3& lhs, float rhs);
Mat3 operator+(const Mat3& lhs, const Mat3& rhs);
Mat3 operator-(const Mat3& lhs, const Mat3& rhs);
Mat3 operator-(const Mat3& operand);

// Quaternion operations
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);


/////////////
// 4d Math //
/////////////

struct Vec4
{
    float data[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    Vec4() = default;
    Vec4(float d0, float d1, float d2, float d3);

    // Takes an array of four floats
    Vec4(float* data_);

    // Sets the fourth coordinate to 1.0;
    Vec4(const Vec3& vec3);

    float& operator[](size_t index);
    const float& operator[](size_t index) const;

    float square_magnitude() const;
    float magnitude() const;
    Vec4 normalize() const;

    Vec4& operator+=(const Vec4& rhs);
    Vec4& operator-=(const Vec4& rhs);
    Vec4& operator*=(float rhs);
    Vec4& operator/=(float rhs);
};

struct Mat4
{
    float data[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};

    Mat4() = default;
    Mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33);

    Mat4(const Mat3& mat3);

    static Mat4 FromColumns(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3);
    static Mat4 FromRows(const Vec4& r0, const Vec4& r1, const Vec4& r2, const Vec4& r3);
    static Mat4 Diagonal(const Vec4& diag);

    static Mat4 Perspective(float near, float far, float fov, float aspect_ratio);
    static Mat4 Orthographic(float near, float far, float width, float height);
    static Mat4 Translate(Vec3 translation);

    Mat4 transpose() const;

    Vec4 column(size_t index) const;
    Vec4 row(size_t index) const;

    Mat4& operator*=(float rhs);
    Mat4& operator*=(const Mat4& rhs);
    Mat4& operator+=(const Mat4& rhs);
    Mat4& operator-=(const Mat4& rhs);
};

// Vector operations
float dot(const Vec4& lhs, const Vec4& rhs);

Vec4 operator+(const Vec4& lhs, const Vec4& rhs);
Vec4 operator-(const Vec4& lhs, const Vec4& rhs);
Vec4 operator*(float lhs, const Vec4& rhs);
Vec4 operator*(const Vec4& lhs, float rhs);
Vec4 operator/(const Vec4& lhs, float rhs);
Vec4 operator-(const Vec4& operand);

// Matrix operations
Vec4 operator*(const Mat4& lhs, const Vec4& rhs);
Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
Mat4 operator*(float lhs, const Mat4& rhs);
Mat4 operator*(const Mat4& lhs, float rhs);
Mat4 operator+(const Mat4& lhs, const Mat4& rhs);
Mat4 operator-(const Mat4& lhs, const Mat4& rhs);
Mat4 operator-(const Mat4& operand);

}

#endif


#ifdef HBMATH_IMPLEMENTATION

#include <cmath>
#include <cassert>

namespace hbmath
{

Vec2::Vec2(float x_, float y_)
    : x(x_), y(y_)
{}

Vec2::Vec2(float* data)
    : x(data[0]), y(data[1])
{}

float Vec2::square_magnitude() const
{
    return x * x + y * y;
}

// A static_assert in the header file ensures that the components of a Vec2
// can be accessed as an array
float* Vec2::array()
{
    return &x;
}

const float* Vec2::array() const
{
    return &x;
}

float Vec2::magnitude() const
{
    return sqrtf(square_magnitude());
}

Vec2 Vec2::normalize() const
{
    return (*this) / magnitude();
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
    return *this = *this + rhs;
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
    return *this = *this - rhs;
}

Vec2& Vec2::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Vec2& Vec2::operator/=(float rhs)
{
    return *this = *this / rhs;
}

float dot(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
{
    return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
{
    return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

Vec2 operator*(float lhs, const Vec2& rhs)
{
    return Vec2(lhs * rhs.x, lhs * rhs.y);
}

Vec2 operator*(const Vec2& lhs, float rhs)
{
    return rhs * lhs;
}

Vec2 operator/(const Vec2& lhs, float rhs)
{
    return (1.0f / rhs) * lhs;
}

Vec2 operator-(const Vec2& operand)
{
    return Vec2(-operand.x, -operand.y);
}

Mat2::Mat2(Vec2 left_column, Vec2 right_column)
    : data{left_column.x, right_column.x, left_column.y, right_column.y}
{}

Mat2::Mat2(float m00, float m01, float m10, float m11)
    : data{m00, m01, m10, m11}
{}

Mat2 Mat2::Diagonal(const Vec2& diag)
{
    return Mat2(diag.x, 0.0f, 0.0f, diag.y);
}

Mat2 Mat2::Rotation(float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);
    return Mat2(cosine, -sine, sine, cosine);
}

Mat2 Mat2::transpose() const
{
    Mat2 result;

    // Set diagonal
    for (int i = 0; i < 2; ++i)
    {
        result.data[3 * i] = data[3 * i];
    }

    // For each row
    for (int i = 0; i < 2; ++i)
    {
        // For each element of row, starting 1 past diagonal
        for (int j = i + 1; j < 2; ++j)
        {
            result.data[2*i + j] = data[2*j + i];
            result.data[2*j + i] = data[2*i + j];
        }
    }

    return result;
}

Mat2& Mat2::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Mat2& Mat2::operator*=(const Mat2& rhs)
{
    return *this = *this * rhs;
}

Mat2& Mat2::operator+=(const Mat2& rhs)
{
    return *this = *this + rhs;
}

Mat2& Mat2::operator-=(const Mat2& rhs)
{
    return *this = *this - rhs;
}

Vec2 operator*(const Mat2& lhs, const Vec2& rhs)
{
    return Vec2(lhs.data[0] * rhs.x + lhs.data[1] * rhs.y, lhs.data[2] * rhs.x + lhs.data[3] * rhs.y);
}

Mat2 operator*(const Mat2& lhs, const Mat2& rhs)
{
    Vec2 left_column  = lhs * Vec2(rhs.data[0], rhs.data[2]);
    Vec2 right_column = lhs * Vec2(rhs.data[1], rhs.data[3]);

    return Mat2(left_column, right_column);
}

Mat2 operator*(float lhs, const Mat2& rhs)
{
    return Mat2(lhs * rhs.data[0], lhs * rhs.data[1],
                lhs * rhs.data[2], lhs * rhs.data[3]);
}

Mat2 operator*(const Mat2& lhs, float rhs)
{
    return rhs * lhs;
}

Mat2 operator+(const Mat2& lhs, const Mat2& rhs)
{
    return Mat2(lhs.data[0] + rhs.data[0], lhs.data[1] + rhs.data[1],
                lhs.data[0] + rhs.data[0], lhs.data[1] + rhs.data[1]);
}

Mat2 operator-(const Mat2& lhs, const Mat2& rhs)
{
    return lhs + (-rhs);
}

Mat2 operator-(const Mat2& operand)
{
    return Mat2(-operand.data[0], -operand.data[1],
                -operand.data[2], -operand.data[3]);
}

Vec3::Vec3(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_)
{}

// Takes an array of three floats
Vec3::Vec3(float* data)
    : x(data[0]), y(data[1]), z(data[2])
{}

Vec3 Vec3::X(float x_)
{
    return Vec3(x_, 0.0f, 0.0f);
}

Vec3 Vec3::Y(float y_)
{
    return Vec3(0.0f, y_, 0.0f);
}

Vec3 Vec3::Z(float z_)
{
    return Vec3(0.0f, 0.0f, z_);
}

float Vec3::square_magnitude() const
{
    return x*x + y*y + z*z;
}

float Vec3::magnitude() const
{
    return sqrtf(square_magnitude());
}

Vec3 Vec3::normalize() const
{
    return *this / magnitude();
}

Vec3& Vec3::operator+=(const Vec3& rhs)
{
    return *this = *this + rhs;
}

Vec3& Vec3::operator-=(const Vec3& rhs)
{
    return *this = *this - rhs;
}

Vec3& Vec3::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Vec3& Vec3::operator/=(float rhs)
{
    return *this = *this / rhs;
}

float* Vec3::array()
{
    return &x;
}

const float* Vec3::array() const
{
    return &x;
}

float dot(const Vec3& lhs, const Vec3& rhs)
{
    return lhs.x * rhs.x
         + lhs.y * rhs.y
         + lhs.z * rhs.z;
}

Vec3 cross(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.z * rhs.x - lhs.x * rhs.z,
                lhs.x * rhs.y - lhs.y * rhs.x);
}

Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(lhs.x + rhs.x,
                lhs.y + rhs.y,
                lhs.z + rhs.z);
}

Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(lhs.x - rhs.x,
                lhs.y - rhs.y,
                lhs.z - rhs.z);
}

Vec3 operator*(float lhs, const Vec3& rhs)
{
    return Vec3(lhs * rhs.x,
                lhs * rhs.y,
                lhs * rhs.z);
}

Vec3 operator*(const Vec3& lhs, float rhs)
{
    return rhs * lhs;
}

Vec3 operator/(const Vec3& lhs, float rhs)
{
    return (1.0f / rhs) * lhs;
}

Vec3 operator-(const Vec3& operand)
{
    return Vec3(-operand.x,
                -operand.y,
                -operand.z);
}

Mat3::Mat3(float m00, float m01, float m02,
           float m10, float m11, float m12,
           float m20, float m21, float m22)
    : data{m00, m01, m02,
           m10, m11, m12,
           m20, m21, m22}
{}

Mat3 Mat3::FromColumns(const Vec3& c0, const Vec3& c1, const Vec3& c2)
{
    return Mat3(
        c0.x, c1.x, c2.x,
        c0.y, c1.y, c2.y,
        c0.z, c1.z, c2.z
    );
}

Mat3 Mat3::FromRows(const Vec3& r0, const Vec3& r1, const Vec3& r2)
{
    return Mat3(
        r0.x, r0.y, r0.z,
        r1.x, r1.y, r1.z,
        r2.x, r2.y, r2.z
    );
}

Mat3 Mat3::Diagonal(const Vec3& diag)
{
    return Mat3(
        diag.x, 0.0f,   0.0f,
        0.0f,   diag.y, 0.0f,
        0.0f,   0.0f,   diag.z);
}

Mat3 Mat3::RotateX(float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);

    return Mat3(
        1.0f,  0.0f,   0.0f,
        0.0f,  cosine, -sine,
        0.0f, sine,   cosine
    );
}

Mat3 Mat3::RotateY(float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);

    return Mat3(
         cosine, 0.0f, sine,
         0.0f,   1.0f, 0.0f,
        -sine,   0.0f, cosine
    );
}

Mat3 Mat3::RotateZ(float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);

    return Mat3(
        cosine, -sine,   0.0f,
        sine,    cosine, 0.0f,
        0.0f,    0.0f,   1.0f
    );
}

Vec3 Mat3::column(size_t index) const
{
    assert(index < 3);
    return Vec3(data[index], data[index + 3], data[index + 6]);
}

Vec3 Mat3::row(size_t index) const
{
    assert(index < 3);
    return Vec3(data[3 * index], data[3 * index + 1], data[3 * index + 2]);
}

Mat3 Mat3::transpose() const
{
    Mat3 result;

    // Set diagonal
    for (int i = 0; i < 3; ++i)
    {
        result.data[4 * i] = data[4 * i];
    }

    // For each row
    for (int i = 0; i < 3; ++i)
    {
        // For each element of row, starting 1 past diagonal
        for (int j = i + 1; j < 3; ++j)
        {
            result.data[3*i + j] = data[3*j + i];
            result.data[3*j + i] = data[3*i + j];
        }
    }

    return result;
}

Mat3& Mat3::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Mat3& Mat3::operator*=(const Mat3& rhs)
{
    return *this = *this * rhs;
}

Mat3& Mat3::operator+=(const Mat3& rhs)
{
    return *this = *this + rhs;
}

Mat3& Mat3::operator-=(const Mat3& rhs)
{
    return *this = *this - rhs;
}

Vec3 operator*(const Mat3& lhs, const Vec3& rhs)
{
    return Vec3(
        dot(lhs.row(0), rhs),
        dot(lhs.row(1), rhs),
        dot(lhs.row(2), rhs));
}

Mat3 operator*(const Mat3& lhs, const Mat3& rhs)
{
    return Mat3::FromColumns(
        lhs * rhs.column(0),
        lhs * rhs.column(1),
        lhs * rhs.column(2));
}

Mat3 operator*(float lhs, const Mat3& rhs)
{
    Mat3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.data[i] = lhs * rhs.data[i];
    }

    return result;
}

Mat3 operator*(const Mat3& lhs, float rhs)
{
    return rhs * lhs;
}

Mat3 operator+(const Mat3& lhs, const Mat3& rhs)
{
    Mat3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.data[i] = lhs.data[i] + rhs.data[i];
    }

    return result;
}

Mat3 operator-(const Mat3& lhs, const Mat3& rhs)
{
    Mat3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.data[i] = lhs.data[i] - rhs.data[i];
    }

    return result;
}

Mat3 operator-(const Mat3& operand)
{
    Mat3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.data[i] = operand.data[i];
    }

    return result;
}

Quaternion::Quaternion(float w_, float x_, float y_, float z_)
    : w(w_), x(x_), y(y_), z(z_)
{}

Quaternion Quaternion::normalize() const
{
    Quaternion result;

    float magnitude = sqrtf(w*w + x*x + y*y + z*z);
    if (magnitude != 0.0f)
    {
        result.w = w / magnitude;
        result.x = x / magnitude;
        result.y = y / magnitude;
        result.z = z / magnitude;
    }
    else
    {
        result.w = 1.0f;
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }

    return result;
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion result;
    result.w = lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z;
    result.x = lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y;
    result.y = lhs.w*rhs.y - lhs.x*rhs.z + lhs.y*rhs.w + lhs.z*rhs.x;
    result.z = lhs.w*rhs.z + lhs.x*rhs.y - lhs.y*rhs.x + lhs.z*rhs.w;

    return result;
}

Quaternion Quaternion::inverse() const
{
    Quaternion result;
    result.w = w;
    result.x = -x;
    result.y = -y;
    result.z = -z;

    return result;
}

void Quaternion::to_matrix(float* matrix)
{
    // assuming magnitude 1
    matrix[0] = 1 - 2 * (y*y + z*z);
    matrix[1] = 2 * (x*y - z*w);
    matrix[2] = 2 * (x*z + y*w);
    matrix[3] = 2 * (x*y + z*w);
    matrix[4] = 1 - 2 * (x*x + z*z);
    matrix[5] = 2 * (y*z - x*w);
    matrix[6] = 2 * (x*z - y*w);
    matrix[7] = 2 * (y*z + x*w);
    matrix[8] = 1 - 2 * (x*x + y*y);
}

Vec3 Quaternion::apply_rotation(const Vec3& x) const
{
    Quaternion x_quat;
    x_quat.w = 0.0f;
    x_quat.x = x.x;
    x_quat.y = x.y;
    x_quat.z = x.z;

    Quaternion result = (*this) * x_quat * (*this).inverse();
    return Vec3(result.x, result.y, result.z);
}

Quaternion Quaternion::RotateX(float angle)
{
    float cosine = cosf(0.5f * angle);
    float sine   = sinf(0.5f * angle);

    Quaternion result;
    result.w = cosine;
    result.x = sine;

    return result;
}

Quaternion Quaternion::RotateY(float angle)
{
    float cosine = cosf(0.5f * angle);
    float sine   = sinf(0.5f * angle);

    Quaternion result;
    result.w = cosine;
    result.y = sine;

    return result;
}

Quaternion Quaternion::RotateZ(float angle)
{
    float cosine = cosf(0.5f * angle);
    float sine   = sinf(0.5f * angle);

    Quaternion result;
    result.w = cosine;
    result.z = sine;

    return result;
}

Vec4::Vec4(float d0, float d1, float d2, float d3)
    : data{d0, d1, d2, d3}
{}

// Sets the fourth coordinate to 1.0
Vec4::Vec4(const Vec3& vec3)
    : data{vec3.x, vec3.y, vec3.z, 1.0f}
{}

// Takes an array of four floats
Vec4::Vec4(float* data_)
    : data{data_[0], data_[1], data_[2], data_[3]}
{}

float& Vec4::operator[](size_t index)
{
    return data[index];
}

const float& Vec4::operator[](size_t index) const
{
    return data[index];
}

float Vec4::square_magnitude() const
{
    return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] + data[3]*data[3];
}

float Vec4::magnitude() const
{
    return sqrtf(square_magnitude());
}

Vec4 Vec4::normalize() const
{
    return *this / magnitude();
}

Vec4& Vec4::operator+=(const Vec4& rhs)
{
    return *this = *this + rhs;
}

Vec4& Vec4::operator-=(const Vec4& rhs)
{
    return *this = *this - rhs;
}

Vec4& Vec4::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Vec4& Vec4::operator/=(float rhs)
{
    return *this = *this / rhs;
}

float dot(const Vec4& lhs, const Vec4& rhs)
{
    return lhs[0] * rhs[0]
         + lhs[1] * rhs[1]
         + lhs[2] * rhs[2]
         + lhs[3] * rhs[3];
}

Vec4 operator+(const Vec4& lhs, const Vec4& rhs)
{
    return Vec4(lhs[0] + rhs[0],
                lhs[1] + rhs[1],
                lhs[2] + rhs[2],
                lhs[3] + rhs[3]);
}

Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
{
    return Vec4(lhs[0] - rhs[0],
                lhs[1] - rhs[1],
                lhs[2] - rhs[2],
                lhs[3] - rhs[3]);
}

Vec4 operator*(float lhs, const Vec4& rhs)
{
    return Vec4(lhs * rhs[0],
                lhs * rhs[1],
                lhs * rhs[2],
                lhs * rhs[3]);
}

Vec4 operator*(const Vec4& lhs, float rhs)
{
    return rhs * lhs;
}

Vec4 operator/(const Vec4& lhs, float rhs)
{
    return (1.0f / rhs) * lhs;
}

Vec4 operator-(const Vec4& operand)
{
    return Vec4(-operand[0],
                -operand[1],
                -operand[2],
                -operand[3]);
}

Mat4::Mat4(float m00, float m01, float m02, float m03,
     float m10, float m11, float m12, float m13,
     float m20, float m21, float m22, float m23,
     float m30, float m31, float m32, float m33)
    : data{m00, m01, m02, m03,
           m10, m11, m12, m13,
           m20, m21, m22, m23,
           m30, m31, m32, m33}
{}

Mat4::Mat4(const Mat3& mat3)
    : data{mat3.data[0], mat3.data[1], mat3.data[2], 0.0f,
           mat3.data[3], mat3.data[4], mat3.data[5], 0.0f,
           mat3.data[6], mat3.data[7], mat3.data[8], 0.0f,
           0.0f,         0.0f,         0.0f,         1.0f}
{}

Mat4 Mat4::FromColumns(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3)
{
    return Mat4(
        c0[0], c1[0], c2[0], c3[0],
        c0[1], c1[1], c2[1], c3[1],
        c0[2], c1[2], c2[2], c3[2],
        c0[3], c1[3], c2[3], c3[3]);
}

Mat4 Mat4::FromRows(const Vec4& r0, const Vec4& r1, const Vec4& r2, const Vec4& r3)
{
    return Mat4(
        r0[0], r0[1], r0[2], r0[3],
        r1[0], r1[1], r1[2], r1[3],
        r2[0], r2[1], r2[2], r2[3],
        r3[0], r3[1], r3[2], r3[3]);
}
Mat4 Mat4::Diagonal(const Vec4& diag)
{
    return Mat4(
        diag[0], 0.0f,    0.0f,    0.0f,
        0.0f,    diag[1], 0.0f,    0.0f,
        0.0f,    0.0f,    diag[2], 0.0f,
        0.0f,    0.0f,    0.0f,    diag[3]);
}

Mat4 Mat4::Perspective(float near, float far, float fov, float aspect_ratio)
{
    float cot_fov = 1.0f / tanf(fov * 0.5f);

    return Mat4(
        cot_fov, 0.0f, 0.0f, 0.0f,
        0.0f, aspect_ratio * cot_fov, 0.0f, 0.0f,
        0.0f, 0.0f, -(near + far) / (far - near), -2 * near * far / (far - near),
        0.0f, 0.0f, -1.0f, 0.0f);
}

Mat4 Mat4::Orthographic(float near, float far, float width, float height)
{
    return Mat4(
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (far - near), -(far + near) / (far - near),
        0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::Translate(Vec3 translation)
{
    return Mat4(
        1.0f, 0.0f, 0.0f, translation.x,
        0.0f, 1.0f, 0.0f, translation.y,
        0.0f, 0.0f, 1.0f, translation.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

Vec4 Mat4::column(size_t index) const
{
    assert(index < 4);
    return Vec4(data[index], data[index + 4], data[index + 8], data[index + 12]);
}

Vec4 Mat4::row(size_t index) const
{
    assert(index < 4);
    return Vec4(data[4 * index], data[4 * index + 1], data[4 * index + 2], data[4 * index + 3]);
}

Mat4 Mat4::transpose() const
{
    Mat4 result;

    // Set diagonal
    for (int i = 0; i < 4; ++i)
    {
        result.data[5 * i] = data[5 * i];
    }

    // For each row
    for (int i = 0; i < 4; ++i)
    {
        // For each element of row, starting 1 past diagonal
        for (int j = i + 1; j < 4; ++j)
        {
            result.data[4*i + j] = data[4*j + i];
            result.data[4*j + i] = data[4*i + j];
        }
    }

    return result;
}

Mat4& Mat4::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Mat4& Mat4::operator*=(const Mat4& rhs)
{
    return *this = *this * rhs;
}

Mat4& Mat4::operator+=(const Mat4& rhs)
{
    return *this = *this + rhs;
}

Mat4& Mat4::operator-=(const Mat4& rhs)
{
    return *this = *this - rhs;
}

Vec4 operator*(const Mat4& lhs, const Vec4& rhs)
{
    return Vec4(
        dot(lhs.row(0), rhs),
        dot(lhs.row(1), rhs),
        dot(lhs.row(2), rhs),
        dot(lhs.row(3), rhs));
}

Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    return Mat4::FromColumns(
        lhs * rhs.column(0),
        lhs * rhs.column(1), 
        lhs * rhs.column(2),
        lhs * rhs.column(3));
}

Mat4 operator*(float lhs, const Mat4& rhs)
{
    Mat4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.data[i] = lhs * rhs.data[i];
    }

    return result;
}

Mat4 operator*(const Mat4& lhs, float rhs)
{
    return rhs * lhs;
}

Mat4 operator+(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.data[i] = lhs.data[i] + rhs.data[i];
    }

    return result;
}

Mat4 operator-(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.data[i] = lhs.data[i] - rhs.data[i];
    }

    return result;
}

Mat4 operator-(const Mat4& operand)
{
    Mat4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.data[i] = -operand.data[i];
    }

    return result;
}

}

#endif
//Copyright (c) 2022 Hugo Burd
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
