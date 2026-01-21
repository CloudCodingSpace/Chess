#pragma once

#include <stdalign.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265358979323846f
#define HALF_PI (0.5f * PI)
#define QUARTER_PI (0.25f * PI)
#define ONE_OVER_PI (1.0f / PI)

#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735026918962576450f

#define DEG2RAD_MULTIPLIER (PI / 180.0f)
#define RAD2DEG_MULTIPLIER (180.0f / PI)

#define SEC_TO_US_MULTIPLIER (1000.0f * 1000.0f)
#define SEC_TO_MS_MULTIPLIER 1000.0f
#define MS_TO_SEC_MULTIPLIER 0.001f

#define INF (1e30f * 1e30f)
#define FLOAT_EPSILON 1.192092896e-07f
#define FLOAT_MIN -3.40282e+38F
#define FLOAT_MAX 3.40282e+38F

typedef union {
    struct { float x, y; };
    struct { float r, g; };
    struct { float s, t; };
    struct { float u, v; };
} Vec2;

typedef union {
    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float s, t, p; };
    struct { float u, v, w; };
} Vec3;

typedef union {
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
    struct { float s, t, width, height; };
} Vec4;

typedef struct Mat2_s {
    alignas(16) float data[4];
} Mat2;

typedef struct Mat3_s {
    alignas(16) float data[9];
} Mat3;

typedef struct Mat4_s {
    alignas(16) float data[16];
} Mat4;

/*     Vec2      */

static inline Vec2 vec2Create(float x, float y) {
    return (Vec2){x, y};
}

static inline Vec2 vec2Add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}

static inline Vec2 vec2Sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}

static inline Vec2 vec2Mul(Vec2 a, Vec2 b) {
    return (Vec2){a.x * b.x, a.y * b.y};
}

static inline Vec2 vec2MulScalar(Vec2 a, float x) {
    return (Vec2){a.x * x, a.y * x};
}

static inline Vec2 vec2Div(Vec2 a, Vec2 b) {
    return (Vec2){a.x / b.x, a.y / b.y};
}

static inline float vec2LengthSqr(Vec2 vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

static inline float vec2Length(Vec2 vec) {
    return sqrtf(vec2LengthSqr(vec));
}

static inline float vec2Dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline Vec2 vec2Normalize(Vec2 a) {
    float len = vec2Length(a);
    if(len == 0.0f)
        return vec2Create(0, 0);
    return vec2Div(a, vec2Create(len, len));
}

/*     Vec3      */

static inline Vec3 vec3Create(float x, float y, float z) {
    return (Vec3){x, y, z};
}

static inline Vec3 vec3Add(Vec3 a, Vec3 b) {
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline Vec3 vec3Sub(Vec3 a, Vec3 b) {
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vec3 vec3Mul(Vec3 a, Vec3 b) {
    return (Vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline Vec3 vec3MulScalar(Vec3 a, float x) {
    return (Vec3){a.x * x, a.y * x, a.z * x};
}

static inline Vec3 vec3Div(Vec3 a, Vec3 b) {
    return (Vec3){a.x / b.x, a.y / b.y, a.z / b.z};
}

static inline float vec3LengthSqr(Vec3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

static inline float vec3Length(Vec3 vec) {
    return sqrtf(vec3LengthSqr(vec));
}

static inline float vec3Dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vec3Cross(Vec3 a, Vec3 b) {
    return (Vec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline Vec3 vec3Normalize(Vec3 a) {
    float len = vec3Length(a);
    if(len == 0.0f)
        return vec3Create(0, 0, 0);
    return vec3Div(a, vec3Create(len, len, len));
}

static inline Vec3 vec3Reflect(Vec3 I, Vec3 N) {
    return vec3Sub(I, vec3MulScalar(N, 2.0f * vec3Dot(I, N)));
}

/*     Vec4      */

static inline Vec4 vec4Create(float x, float y, float z, float w) {
    return (Vec4){x, y, z, w};
}

static inline Vec4 vec4Add(Vec4 a, Vec4 b) {
    return (Vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

static inline Vec4 vec4Sub(Vec4 a, Vec4 b) {
    return (Vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

static inline Vec4 vec4Mul(Vec4 a, Vec4 b) {
    return (Vec4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

static inline Vec4 vec4MulScalar(Vec4 a, float x) {
    return (Vec4){a.x * x, a.y * x, a.z * x, a.w * x};
}

static inline Vec4 vec4Div(Vec4 a, Vec4 b) {
    return (Vec4){a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

static inline float vec4LengthSqr(Vec4 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

static inline float vec4Length(Vec4 vec) {
    return sqrtf(vec4LengthSqr(vec));
}

static inline float vec4Dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline Vec4 vec4Normalize(Vec4 a) {
    float len = vec4Length(a);
    if(len == 0.0f)
        return vec4Create(0, 0, 0, 0);
    return vec4Div(a, vec4Create(len, len, len, len));
}

/*  Mat2  */

static inline Mat2 mat2Identity() {
    return (Mat2) {
        .data = {
            1, 0,
            0, 1
        }
    };
}

static inline Mat2 mat2Add(Mat2 a, Mat2 b) {
    Mat2 result;

    for(int i = 0; i < 4; i++) {
        result.data[i] = a.data[i] + b.data[i];
    }

    return result;
}

static inline Mat2 mat2Sub(Mat2 a, Mat2 b) {
    Mat2 result;

    for(int i = 0; i < 4; i++) {
        result.data[i] = a.data[i] - b.data[i];
    }

    return result;
}

static inline Mat2 mat2Mul(Mat2 a, Mat2 b) {
    Mat2 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[2] * b.data[1];
    result.data[1] = a.data[1] * b.data[0] + a.data[3] * b.data[1];

    result.data[2] = a.data[0] * b.data[2] + a.data[2] * b.data[3];
    result.data[3] = a.data[1] * b.data[2] + a.data[3] * b.data[3];

    return result;
}

static inline Mat2 mat2MulScalar(Mat2 a, float x) {
    Mat2 result;

    for (int i = 0; i < 4; i++) {
        result.data[i] = a.data[i] * x;
    } 

    return result;
}

static inline Vec2 mat2MulVec2(Mat2 a, Vec2 x) {
    Vec2 result;

    result.x = a.data[0] * x.x + a.data[1] * x.y;
    result.y = a.data[2] * x.x + a.data[3] * x.y;

    return result;
}

static inline void mat2Rotate(Mat2* mat, float theta_rad) {
    float c = cosf(theta_rad);
    float s = sinf(theta_rad);
    
    Mat2 rotation = {
        .data = {
            c, -s,
            s,  c
        }
    };

    *mat = mat2Mul(rotation, *mat);
}

static inline void mat2Scale(Mat2* mat, float x, float y) {
    mat->data[0] *= x;
    mat->data[3] *= y;
}

static inline Mat2 mat2Transpose(Mat2 mat) {
    Mat2 result;

    result.data[0] = mat.data[0];
    result.data[3] = mat.data[3];

    result.data[1] = mat.data[2];
    result.data[2] = mat.data[1];

    return result;
}

static inline Mat2 mat2Inverse(Mat2 mat) {
    Mat2 result;

    result.data[0] = mat.data[3];
    result.data[1] = -mat.data[1];
    result.data[2] = -mat.data[2];
    result.data[3] = mat.data[0];

    float d = mat.data[0] * mat.data[3] - mat.data[1] * mat.data[2];

    if(fabsf(d) < 1e-6f) {
        fprintf(stderr, "ERROR: The inverse determinant is undefined! Aborting!");
        exit(1);
    }

    d = 1.0f / d;

    result = mat2MulScalar(result, d);

    return result;
}

/*  Mat3  */
static inline Mat3 mat3Identity() {
    return (Mat3) {
        .data = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        }
    };
}

static inline Mat3 mat3Add(Mat3 a, Mat3 b) {
    Mat3 result;

    for(int i = 0; i < 9; i++) {
        result.data[i] = a.data[i] + b.data[i];
    }

    return result;
}

static inline Mat3 mat3Sub(Mat3 a, Mat3 b) {
    Mat3 result;

    for(int i = 0; i < 9; i++) {
        result.data[i] = a.data[i] - b.data[i];
    }

    return result;
}

static inline Mat3 mat3Mul(Mat3 a, Mat3 b) {
    Mat3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[3] * b.data[1] + a.data[6] * b.data[2];
    result.data[3] = a.data[0] * b.data[3] + a.data[3] * b.data[4] + a.data[6] * b.data[5];
    result.data[6] = a.data[0] * b.data[6] + a.data[3] * b.data[7] + a.data[6] * b.data[8];

    result.data[1] = a.data[1] * b.data[0] + a.data[4] * b.data[1] + a.data[7] * b.data[2];
    result.data[4] = a.data[1] * b.data[3] + a.data[4] * b.data[4] + a.data[7] * b.data[5];
    result.data[7] = a.data[1] * b.data[6] + a.data[4] * b.data[7] + a.data[7] * b.data[8];
    
    result.data[2] = a.data[2] * b.data[0] + a.data[5] * b.data[1] + a.data[8] * b.data[2];
    result.data[5] = a.data[2] * b.data[3] + a.data[5] * b.data[4] + a.data[8] * b.data[5];
    result.data[8] = a.data[2] * b.data[6] + a.data[5] * b.data[7] + a.data[8] * b.data[8];

    return result;
}

static inline Mat3 mat3MulScalar(Mat3 a, float x) {
    Mat3 result;

    for (int i = 0; i < 9; i++) {
        result.data[i] = a.data[i] * x;
    } 

    return result;
}

static inline Vec3 mat3MulVec3(Mat3 a, Vec3 x) {
    Vec3 result;

    result.x = a.data[0] * x.x + a.data[1] * x.y + a.data[2] * x.z;
    result.y = a.data[3] * x.x + a.data[4] * x.y + a.data[5] * x.z;
    result.z = a.data[6] * x.x + a.data[7] * x.y + a.data[8] * x.z;

    return result;
}

static inline void mat3Rotate(Mat3* mat, float theta_rad) {
    float c = cosf(theta_rad);
    float s = sinf(theta_rad);

    Mat3 rotation = {
        .data = {
            c, -s, 0,
            s,  c, 0,
            0,  0, 1
        }
    };

    *mat = mat3Mul(rotation, *mat);
}

static inline void mat3Scale(Mat3* mat, float x, float y, float z) {
    mat->data[0] *= x;
    mat->data[4] *= y;
    mat->data[8] *= z;
}

static inline Mat3 mat3Transpose(Mat3 mat) {
    Mat3 result;

    result.data[0] = mat.data[0];
    result.data[4] = mat.data[4];
    result.data[8] = mat.data[8];

    result.data[1] = mat.data[3];
    result.data[2] = mat.data[6];
    result.data[3] = mat.data[1];

    result.data[5] = mat.data[7];
    result.data[6] = mat.data[2];
    result.data[7] = mat.data[5];

    return result;
}

static inline Mat3 mat3Inverse(Mat3 m) {
    float a = m.data[0], b = m.data[1], tx = m.data[2];
    float c = m.data[3], d = m.data[4], ty = m.data[5];

    float det = a * d - b * c;
    if (fabsf(det) < 1e-6f) {
        fprintf(stderr, "ERROR: The inverse determinant is undefined! Aborting!");
        exit(1);
    }

    float invDet = 1.0f / det;

    float ia =  d * invDet;
    float ib = -b * invDet;
    float ic = -c * invDet;
    float id =  a * invDet;

    float itx = -(ia * tx + ib * ty);
    float ity = -(ic * tx + id * ty);

    Mat3 inv = {
        .data = {
            ia, ib, itx,
            ic, id, ity,
            0,  0,  1
        }
    };

    return inv;
}
/*   Mat4   */

static inline Mat4 mat4Identity(void) {
    return (Mat4){
        .data = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }
    };
}

static inline Mat4 mat4Mul(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (uint32_t col = 0; col < 4; ++col) {
        for (uint32_t row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (uint32_t i = 0; i < 4; ++i) {
                sum += a.data[i * 4 + row] * b.data[col * 4 + i];
            }
            result.data[col * 4 + row] = sum;
        }
    }
    return result;
}

static inline Mat4 mat4MulScalar(Mat4 mat, float scalar) {
    for (uint32_t i = 0; i < 16; ++i) {
        mat.data[i] *= scalar;
    }
    return mat;
}

static inline Mat4 mat4Translate(float x, float y, float z) {
    Mat4 mat = mat4Identity();
    mat.data[12] = x;
    mat.data[13] = y;
    mat.data[14] = z;
    return mat;
}

static inline void mat4Scale(Mat4* mat, float x, float y, float z) {
    mat->data[0] *= x;
    mat->data[5] *= y;
    mat->data[10] *= z;
}

static inline Mat4 mat4RotateX(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    Mat4 mat = mat4Identity();
    mat.data[5] = c;
    mat.data[6] = s;
    mat.data[9] = -s;
    mat.data[10] = c;
    return mat;
}

static inline Mat4 mat4RotateY(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    Mat4 mat = mat4Identity();
    mat.data[0] = c;
    mat.data[2] = -s;
    mat.data[8] = s;
    mat.data[10] = c;
    return mat;
}

static inline Mat4 mat4RotateZ(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    Mat4 mat = mat4Identity();
    mat.data[0] = c;
    mat.data[1] = s;
    mat.data[4] = -s;
    mat.data[5] = c;
    return mat;
}

static inline Mat4 mat4RotateXYZ(float radiansX, float radiansY, float radiansZ) {
    Mat4 x = mat4RotateX(radiansX);
    Mat4 y = mat4RotateY(radiansY);
    Mat4 z = mat4RotateZ(radiansZ);
    return mat4Mul(mat4Mul(x, y), z);
}

static inline Mat4 mat4Perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
    float f = 1.0f / tanf(fovYRadians / 2.0f);
    float nf = 1.0f / (nearZ - farZ);
    Mat4 mat = {0};
    mat.data[0] = f / aspect;
    mat.data[5] = f;
    mat.data[10] = (farZ + nearZ) * nf;
    mat.data[11] = -1.0f;
    mat.data[14] = (2.0f * farZ * nearZ) * nf;
    return mat;
}

static inline Mat4 mat4Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Mat4 mat = {0};
    mat.data[0] = 2.0f / (right - left);
    mat.data[5] = 2.0f / (top - bottom);
    mat.data[10] = -2.0f / (farZ - nearZ);
    mat.data[12] = -(right + left) / (right - left);
    mat.data[13] = -(top + bottom) / (top - bottom);
    mat.data[14] = -(farZ + nearZ) / (farZ - nearZ);
    mat.data[15] = 1.0f;
    return mat;
}

static inline Mat4 mat4LookAt(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = vec3Normalize(vec3Sub(center, eye));
    Vec3 s = vec3Normalize(vec3Cross(f, up));
    Vec3 u = vec3Cross(s, f);

    Mat4 mat = mat4Identity();

    mat.data[0] = s.x;
    mat.data[1] = u.x;
    mat.data[2] = -f.x;
    mat.data[3] = 0.0f;

    mat.data[4] = s.y;
    mat.data[5] = u.y;
    mat.data[6] = -f.y;
    mat.data[7] = 0.0f;

    mat.data[8] = s.z;
    mat.data[9] = u.z;
    mat.data[10] = -f.z;
    mat.data[11] = 0.0f;

    mat.data[12] = -vec3Dot(s, eye);
    mat.data[13] = -vec3Dot(u, eye);
    mat.data[14] = vec3Dot(f, eye);
    mat.data[15] = 1.0f;

    return mat;
}

static inline Mat4 mat4Inverse(Mat4 m) {
    float* a = m.data;
    Mat4 inv;
    float* o = inv.data;

    o[0] =  a[5]*a[10]*a[15] - a[5]*a[11]*a[14] - a[9]*a[6]*a[15]  + a[9]*a[7]*a[14] + a[13]*a[6]*a[11] - a[13]*a[7]*a[10];

    o[4] = -a[4]*a[10]*a[15] + a[4]*a[11]*a[14] + a[8]*a[6]*a[15]  - a[8]*a[7]*a[14] - a[12]*a[6]*a[11] + a[12]*a[7]*a[10];

    o[8] =  a[4]*a[9]*a[15] - a[4]*a[11]*a[13] - a[8]*a[5]*a[15]  + a[8]*a[7]*a[13] + a[12]*a[5]*a[11] - a[12]*a[7]*a[9];

    o[12] = -a[4]*a[9]*a[14] + a[4]*a[10]*a[13] + a[8]*a[5]*a[14]  - a[8]*a[6]*a[13] - a[12]*a[5]*a[10] + a[12]*a[6]*a[9];

    o[1] = -a[1]*a[10]*a[15] + a[1]*a[11]*a[14] + a[9]*a[2]*a[15]  - a[9]*a[3]*a[14] - a[13]*a[2]*a[11] + a[13]*a[3]*a[10];

    o[5] =  a[0]*a[10]*a[15] - a[0]*a[11]*a[14] - a[8]*a[2]*a[15]  + a[8]*a[3]*a[14] + a[12]*a[2]*a[11] - a[12]*a[3]*a[10];

    o[9] = -a[0]*a[9]*a[15] + a[0]*a[11]*a[13] + a[8]*a[1]*a[15] - a[8]*a[3]*a[13] - a[12]*a[1]*a[11] + a[12]*a[3]*a[9];

    o[13] =  a[0]*a[9]*a[14] - a[0]*a[10]*a[13] - a[8]*a[1]*a[14] + a[8]*a[2]*a[13] + a[12]*a[1]*a[10] - a[12]*a[2]*a[9];

    o[2] =  a[1]*a[6]*a[15] - a[1]*a[7]*a[14] - a[5]*a[2]*a[15]  + a[5]*a[3]*a[14] + a[13]*a[2]*a[7] - a[13]*a[3]*a[6];

    o[6] = -a[0]*a[6]*a[15] + a[0]*a[7]*a[14] + a[4]*a[2]*a[15] - a[4]*a[3]*a[14] - a[12]*a[2]*a[7] + a[12]*a[3]*a[6];

    o[10] = a[0]*a[5]*a[15] - a[0]*a[7]*a[13] - a[4]*a[1]*a[15]  + a[4]*a[3]*a[13] + a[12]*a[1]*a[7] - a[12]*a[3]*a[5];

    o[14] = -a[0]*a[5]*a[14] + a[0]*a[6]*a[13] + a[4]*a[1]*a[14] - a[4]*a[2]*a[13] - a[12]*a[1]*a[6] + a[12]*a[2]*a[5];

    o[3] = -a[1]*a[6]*a[11] + a[1]*a[7]*a[10] + a[5]*a[2]*a[11] - a[5]*a[3]*a[10] - a[9]*a[2]*a[7] + a[9]*a[3]*a[6];

    o[7] =  a[0]*a[6]*a[11] - a[0]*a[7]*a[10] - a[4]*a[2]*a[11] + a[4]*a[3]*a[10] + a[8]*a[2]*a[7] - a[8]*a[3]*a[6];

    o[11] = -a[0]*a[5]*a[11] + a[0]*a[7]*a[9] + a[4]*a[1]*a[11]  - a[4]*a[3]*a[9] - a[8]*a[1]*a[7] + a[8]*a[3]*a[5];

    o[15] = a[0]*a[5]*a[10] - a[0]*a[6]*a[9] - a[4]*a[1]*a[10] + a[4]*a[2]*a[9] + a[8]*a[1]*a[6] - a[8]*a[2]*a[5];

    float det = a[0]*o[0] + a[1]*o[4] + a[2]*o[8] + a[3]*o[12];
    if (det == 0.0f) {
        fprintf(stderr, "ERROR: The inverse determinant is undefined! Aborting!");
        exit(1);
    }

    float inv_det = 1.0f / det;
    for (int i = 0; i < 16; ++i) 
        o[i] *= inv_det;
    return inv;
}

static inline Mat4 mat4Transpose(Mat4 mat) {
    Mat4 result;

    result.data[0]  = mat.data[0];
    result.data[1]  = mat.data[4];
    result.data[2]  = mat.data[8];
    result.data[3]  = mat.data[12];

    result.data[4]  = mat.data[1];
    result.data[5]  = mat.data[5];
    result.data[6]  = mat.data[9];
    result.data[7]  = mat.data[13];

    result.data[8]  = mat.data[2];
    result.data[9]  = mat.data[6];
    result.data[10] = mat.data[10];
    result.data[11] = mat.data[14];

    result.data[12] = mat.data[3];
    result.data[13] = mat.data[7];
    result.data[14] = mat.data[11];
    result.data[15] = mat.data[15];

    return result;
}
