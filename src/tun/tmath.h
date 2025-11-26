#pragma once
#include "Jolt/Jolt.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "tun/tlog.h"
#include "tun/tlist.h"

using Quat = glm::quat;
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Matrix = glm::mat4;
using Color = glm::vec3;
using Vec = glm::vec3;

inline Quat Convert(const JPH::Quat& q) {
    return Quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
}

inline Vec3 Convert(const JPH::Vec3& v) {
    return Vec3(v.GetX(), v.GetY(), v.GetZ());
}

inline Vec4 Convert(const JPH::Vec4& v) {
    return Vec4(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
}

inline Matrix Convert(const JPH::Mat44& m) {
    return Matrix(Convert(m.GetColumn4(0)), Convert(m.GetColumn4(1)), Convert(m.GetColumn4(2)), Convert(m.GetColumn4(3)));
}

inline JPH::Quat Convert(const Quat& q) {
    return JPH::Quat(q.x, q.y, q.z, q.w);
}

inline JPH::Vec3 Convert(const Vec3& v) {
    return JPH::Vec3(v.x, v.y, v.z);
}

inline JPH::Vec4 Convert(const Vec4& v) {
    return JPH::Vec4(v.x, v.y, v.z, v.w);
}

inline JPH::Mat44 Convert(const Matrix& m) {
    return JPH::Mat44(Convert(m[0]), Convert(m[1]), Convert(m[2]), Convert(m[3]));
}

struct Plane {
    Vec3 normal {};
    float distance {};
};

struct Frustum {
    List<Plane> planes {};
    List<Vec> points {};
};

struct BoundingBox {
    Vec min {FLT_MAX};
    Vec max {FLT_MIN};

    void Stretch(const Vec& p) {
        if (p.x < min.x) {
            min.x = p.x;
        }
        if (p.y < min.y) {
            min.y = p.y;
        }
        if (p.z < min.z) {
            min.z = p.z;
        }
        if (p.x > max.x) {
            max.x = p.x;
        }
        if (p.y > max.y) {
            max.y = p.y;
        }
        if (p.z > max.z) {
            max.z = p.z;
        }
    }
};

template <>
struct std::formatter<Quat> : std::formatter<std::string> {
    auto format(Quat quat, format_context& ctx) const {
        Vec angles = glm::eulerAngles(quat);
        return formatter<string>::format(std::format("[pitch={:.0f}, yaw={:.0f}, roll={:.0f}]", glm::degrees(angles.x), glm::degrees(angles.y), glm::degrees(angles.z)), ctx);
    }
};

template <>
struct std::formatter<Vec2> : std::formatter<std::string> {
    auto format(Vec2 vec, format_context& ctx) const {
        return formatter<string>::format(std::format("[x={:.2f}, y={:.2f}]", vec.x, vec.y), ctx);
    }
};

template <>
struct std::formatter<Vec> : std::formatter<std::string> {
    auto format(Vec vec, format_context& ctx) const {
        return formatter<string>::format(std::format("[x={:.2f}, y={:.2f}, z={:.2f}]", vec.x, vec.y, vec.z), ctx);
    }
};

template <>
struct std::formatter<Vec4> : std::formatter<std::string> {
    auto format(Vec4 vec, format_context& ctx) const {
        return formatter<string>::format(std::format("[x={:.2f}, y={:.2f}, z={:.2f}, w={:.2f}]", vec.x, vec.y, vec.z, vec.w), ctx);
    }
};

template <>
struct std::formatter<Matrix> : std::formatter<std::string> {
    auto format(Matrix m, format_context& ctx) const {
        return formatter<string>::format(std::format("\n{:10.2f}{:10.2f}{:10.2f}{:10.2f}\n{:10.2f}{:10.2f}{:10.2f}{:10.2f}\n{:10.2f}{:10.2f}{:10.2f}{:10.2f}\n{:10.2f}{:10.2f}{:10.2f}{:10.2f}\n", 
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]
        ), ctx);
    }
};

namespace tun {

enum Anchor {
    begin,
    center,
    end,
};

struct Anchors {
    Anchor horizontal {center};
    Anchor vertical {center};
};

enum ProjectionType {
    perspective,
    ortho,
};

inline constexpr double pi = 3.14159265358979323846;
inline constexpr Vec up {0.f, 1.f, 0.f};
inline constexpr Vec down {0.f, -1.f, 0.f};
inline constexpr Vec right {1.f, 0.f, 0.f};
inline constexpr Vec left {-1.f, 0.f, 0.f};
inline constexpr Vec forward {0.f, 0.f, 1.f};
inline constexpr Vec back {0.f, 0.f, -1.f};
inline constexpr Vec vecOne {1.f, 1.f, 1.f};
inline constexpr Vec vecZero {0.f, 0.f, 0.f};
inline constexpr Quat quatIdentity {1.f, 0.f, 0.f, 0.f};

inline constexpr Color red {1.f, 0.f, 0.f};
inline constexpr Color darkRed {0.25f, 0.f, 0.f};
inline constexpr Color green {0.f, 1.f, 0.f};
inline constexpr Color darkGreen {0.f, 0.25f, 0.f};
inline constexpr Color gray {0.3f, 0.3f, 0.3f};
inline constexpr Color blue {0.f, 0.f, 1.f};
inline constexpr Color darkBlue {0.f, 0.0f, 0.25f};
inline constexpr Color darkPurple {0.25f, 0.0f, 0.25f};
inline constexpr Color darkYellow {0.25f, 0.25f, 0.f};
inline constexpr Color white {1.f, 1.f, 1.f};
inline constexpr Color black {0.f, 0.f, 0.f};
inline constexpr Color pink {1.f, 0.5f, 0.5f};
inline constexpr Color yellow {1.f, 1.0f, 0.f};
inline constexpr Color purple {1.f, 0.0f, 1.f};
inline constexpr Color orange {1.f, 0.5f, 0.f};
inline constexpr Color gold {1.f, 0.84f, 0.f};

inline constexpr Color lightGray {0.75f, 0.75f, 0.75f};
inline constexpr Color goldGray0 {0.75f*0.25f+0.95f*0.75f, 0.75f*0.25f+0.84f*0.75f, 0.75f*0.25f+0.6f*0.75f};
inline constexpr Color goldGray1 {0.75f*0.5f+0.95f*0.5f, 0.75f*0.5f+0.84f*0.5f, 0.75f*0.5f+0.6f*0.5f};
inline constexpr Color goldGray2 {0.75f*0.75f+0.95f*0.25f, 0.75f*0.75f+0.84f*0.25f, 0.75f*0.75f+0.6f*0.25f};
inline constexpr Color goldGray {0.95f, 0.84f, 0.6f};

float Clamp(float x, float min, float max);

Matrix LookAt(const Vec& position, const Quat& rotation);
Vec2 LookAtPitchYaw(const Vec& position, const Vec& target);

float Lerp(float a, float b, float t);
Vec4 Lerp(const Vec4& a, const Vec4& b, float t);
Color Lerp(const Color& a, const Color& b, float t);
Color LerpThrough(const Color& a, const Color& c, const Color& b, float t);
Quat Lerp(const Quat& a, const Quat& b, float t);
Matrix Lerp(const Matrix& a, const Matrix& b, float t);

float CubicBezier(float t, float x1, float y1, float x2, float y2);
float CurveAuto(float t);
float CurvePunch(float t);

Frustum ExtractFrustumPlanes(float fovy, float aspect, float znear, float zfar, const Matrix& projView, const Matrix& view);
bool IsBoundingBoxInFrustum(const BoundingBox& box, const Frustum& frustum);
bool IsBoundingBoxInSphere(const BoundingBox& box, const Vec& spherePos, float radius);
BoundingBox TransformAABB(const Matrix& transformMatrix, const BoundingBox& localAABB, const Vec& offset, const Vec& shapeSize);

}
