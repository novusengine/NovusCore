#include "Matrix.h"

Matrix::Matrix(bool identity)
    : right(identity ? Vector3::Right : Vector3::Zero)
    , pad1(0.0f)
    , up(identity ? Vector3::Up : Vector3::Zero)
    , pad2(0.0f)
    , at(identity ? Vector3::Forward : Vector3::Zero)
    , pad3(0.0f)
    , pos(Vector3::Zero)
    , pad4(identity ? 1.0f : 0.0f)
{

}

Matrix::Matrix(const Matrix& other)
{
    *this = other;
}

Matrix& Matrix::operator=(const Matrix& other)
{
    for (uint32_t i = 0; i < 4; ++i)
    {
        row[i] = other.row[i];
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    Matrix out = *this;
    out *= other;
    return out;
}

Matrix Matrix::operator*(const float scalar) const
{
    Matrix out = *this;
    out *= scalar;
    return out;
}

Matrix Matrix::operator+(const Matrix& other) const
{
    Matrix out = *this;
    out += other;
    return out;
}

Matrix Matrix::operator/(float scalar) const
{
    Matrix out = *this;
    for (uint32_t i = 0; i < 4; ++i)
    {
        out.row[i] /= scalar;
    }

    return out;
}

Matrix& Matrix::operator*=(const Matrix& other)
{
    Matrix before(*this);
    Matrix in_transposed = other.Transposed();

    row[0].x = before.row[0].Dot(in_transposed.row[0]);
    row[0].y = before.row[0].Dot(in_transposed.row[1]);
    row[0].z = before.row[0].Dot(in_transposed.row[2]);
    row[0].w = before.row[0].Dot(in_transposed.row[3]);

    row[1].x = before.row[1].Dot(in_transposed.row[0]);
    row[1].y = before.row[1].Dot(in_transposed.row[1]);
    row[1].z = before.row[1].Dot(in_transposed.row[2]);
    row[1].w = before.row[1].Dot(in_transposed.row[3]);

    row[2].x = before.row[2].Dot(in_transposed.row[0]);
    row[2].y = before.row[2].Dot(in_transposed.row[1]);
    row[2].z = before.row[2].Dot(in_transposed.row[2]);
    row[2].w = before.row[2].Dot(in_transposed.row[3]);

    row[3].x = before.row[3].Dot(in_transposed.row[0]);
    row[3].y = before.row[3].Dot(in_transposed.row[1]);
    row[3].z = before.row[3].Dot(in_transposed.row[2]);
    row[3].w = before.row[3].Dot(in_transposed.row[3]);

    return *this;
}

Matrix& Matrix::operator+=(const Matrix& other)
{
    for (uint32_t i = 0; i < 4; ++i)
    {
        row[i] += other.row[i];
    }

    return *this;
}

Matrix& Matrix::operator*=(const float scalar)
{
    for (uint32_t i = 0; i < 4; ++i)
    {
        row[i] *= scalar;
    }

    return *this;
}

Matrix& Matrix::Transpose()
{
    Matrix before(*this);
    row[0] = Vector4(before.row[0].x, before.row[1].x, before.row[2].x, before.row[3].x);
    row[1] = Vector4(before.row[0].y, before.row[1].y, before.row[2].y, before.row[3].y);
    row[2] = Vector4(before.row[0].z, before.row[1].z, before.row[2].z, before.row[3].z);
    row[3] = Vector4(before.row[0].w, before.row[1].w, before.row[2].w, before.row[3].w);
    return *this;
}

Matrix Matrix::Transposed() const
{
    Matrix out = *this;
    return out.Transpose();
}

Matrix& Matrix::Invert()
{
    *this = Inverted();
    return *this;
}


Matrix Matrix::Inverted() const
{
    Matrix out;

    float v0 = row[2].x * row[3].y - row[2].y * row[3].x;
    float v1 = row[2].x * row[3].z - row[2].z * row[3].x;
    float v2 = row[2].x * row[3].w - row[2].w * row[3].x;
    float v3 = row[2].y * row[3].z - row[2].z * row[3].y;
    float v4 = row[2].y * row[3].w - row[2].w * row[3].y;
    float v5 = row[2].z * row[3].w - row[2].w * row[3].z;

    Vector4 t;
    t.x = v5 * row[1].y - v4 * row[1].z + v3 * row[1].w;
    t.y = -(v5 * row[1].x - v2 * row[1].z + v1 * row[1].w);
    t.z = v4 * row[1].x - v2 * row[1].y + v0 * row[1].w;
    t.w = -(v3 * row[1].x - v1 * row[1].y + v0 * row[1].z);

    float inv_det = 1.0f / row[0].Dot(t);

    out.row[0].x = t.x * inv_det;
    out.row[1].x = t.y * inv_det;
    out.row[2].x = t.z * inv_det;
    out.row[3].x = t.w * inv_det;

    out.row[0].y = -(v5 * row[0].y - v4 * row[0].z + v3 * row[0].w) * inv_det;
    out.row[1].y = (v5 * row[0].x - v2 * row[0].z + v1 * row[0].w) * inv_det;
    out.row[2].y = -(v4 * row[0].x - v2 * row[0].y + v0 * row[0].w) * inv_det;
    out.row[3].y = (v3 * row[0].x - v1 * row[0].y + v0 * row[0].z) * inv_det;

    v0 = row[1].x * row[3].y - row[1].y * row[3].x;
    v1 = row[1].x * row[3].z - row[1].z * row[3].x;
    v2 = row[1].x * row[3].w - row[1].w * row[3].x;
    v3 = row[1].y * row[3].z - row[1].z * row[3].y;
    v4 = row[1].y * row[3].w - row[1].w * row[3].y;
    v5 = row[1].z * row[3].w - row[1].w * row[3].z;

    out.row[0].z = (v5 * row[0].y - v4 * row[0].z + v3 * row[0].w) * inv_det;
    out.row[1].z = -(v5 * row[0].x - v2 * row[0].z + v1 * row[0].w) * inv_det;
    out.row[2].z = (v4 * row[0].x - v2 * row[0].y + v0 * row[0].w) * inv_det;
    out.row[3].z = -(v3 * row[0].x - v1 * row[0].y + v0 * row[0].z) * inv_det;

    v0 = row[2].y * row[1].x - row[2].x * row[1].y;
    v1 = row[2].z * row[1].x - row[2].x * row[1].z;
    v2 = row[2].w * row[1].x - row[2].x * row[1].w;
    v3 = row[2].z * row[1].y - row[2].y * row[1].z;
    v4 = row[2].w * row[1].y - row[2].y * row[1].w;
    v5 = row[2].w * row[1].z - row[2].z * row[1].w;

    out.row[0].w = -(v5 * row[0].y - v4 * row[0].z + v3 * row[0].w) * inv_det;
    out.row[1].w = (v5 * row[0].x - v2 * row[0].z + v1 * row[0].w) * inv_det;
    out.row[2].w = -(v4 * row[0].x - v2 * row[0].y + v0 * row[0].w) * inv_det;
    out.row[3].w = (v3 * row[0].x - v1 * row[0].y + v0 * row[0].z) * inv_det;

    return out;
}

Matrix& Matrix::RotateX(float angle, MultiplicationType mul_type)
{
    Matrix rotation_matrix;
    angle = Math::DegToRad(angle);
    // Left unchanged
    rotation_matrix.up = Vector3(0.0f, Math::Cos(angle), -Math::Sin(angle));
    rotation_matrix.at = Vector3(0.0f, Math::Sin(angle), Math::Cos(angle));

    return Transform(rotation_matrix, mul_type);
}

Matrix& Matrix::RotateY(float angle, MultiplicationType mul_type)
{
    Matrix rotation_matrix;
    angle = Math::DegToRad(angle);
    rotation_matrix.right = Vector3(Math::Cos(angle), 0.0f, Math::Sin(angle));
    // Up unchanged
    rotation_matrix.at = Vector3(-Math::Sin(angle), 0.0f, Math::Cos(angle));

    return Transform(rotation_matrix, mul_type);
}

Matrix& Matrix::RotateZ(float angle, MultiplicationType mul_type)
{
    Matrix rotation_matrix;
    angle = Math::DegToRad(angle);
    rotation_matrix.right = Vector3(Math::Cos(angle), -Math::Sin(angle), 0.0f);
    rotation_matrix.up = Vector3(Math::Sin(angle), Math::Cos(angle), 0.0f);
    // At unchanged

    return Transform(rotation_matrix, mul_type);
}

Matrix& Matrix::Scale(Vector3 scale, MultiplicationType mul_type)
{
    Matrix scale_matrix;
    scale_matrix.right.x = scale.x;
    scale_matrix.up.y = scale.y;
    scale_matrix.at.z = scale.z;

    return Transform(scale_matrix, mul_type);
}

Matrix& Matrix::Scale(float scale, MultiplicationType mul_type)
{
    Matrix scale_matrix;
    scale_matrix.right.x = scale;
    scale_matrix.up.y = scale;
    scale_matrix.at.z = scale;

    return Transform(scale_matrix, mul_type);
}

Matrix& Matrix::Transform(const Matrix& transformation, MultiplicationType mul_type)
{
    switch (mul_type)
    {
    case PRE:
    {
        *this = transformation * *this;
    }
    break;
    case POST:
    {
        *this = *this * transformation;
    }
    break;
    case REPLACE:
    {
        *this = transformation;
    }
    break;
    }

    return *this;
}

void Matrix::TransformVector(Vector3& vector, bool translation) const
{
    Vector3 out;
    float w = static_cast<float>(translation);
    Matrix transpose = Transposed();
    out.x = transpose.row[0].Dot(Vector4(vector, w));
    out.y = transpose.row[1].Dot(Vector4(vector, w));
    out.z = transpose.row[2].Dot(Vector4(vector, w));
    vector = out;
}