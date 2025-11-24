#include "tun/tmath.h"
#include "comp/ccore.h"
#include "glm/common.hpp"
#include "glm/gtc/matrix_access.hpp"

Matrix tun::LookAt(const Vec& position, const Quat& rotation) {
    const Vec look = rotation * tun::back;
    const Vec right = rotation * tun::right;
    const Vec up = rotation * tun::up;
    Matrix m(1);
    m[0][0] = right.x;
    m[1][0] = right.y;
    m[2][0] = right.z;
    m[0][1] = up.x;
    m[1][1] = up.y;
    m[2][1] = up.z;
    m[0][2] = look.x;
    m[1][2] = look.y;
    m[2][2] = look.z;
    m[3][0] = -dot(right, position);
    m[3][1] = -dot(up, position);
    m[3][2] = dot(-look, position);
    return m;
}

Vec2 tun::LookAtPitchYaw(const Vec& position, const Vec& target) {
    Vec direction = target - position;
    float len = glm::length(direction);
    float pitch = len < 0.001f ? 0.f : -glm::asin(direction.y / len);
    float yaw = glm::atan(direction.x, direction.z);
    return Vec2(pitch, yaw);
}

float tun::Lerp(float a, float b, float t) {
    return a * (1.f - t) + b * t;
}

float tun::Clamp(float x, float min, float max) {
    return glm::clamp(x, min, max);
}

Vec4 tun::Lerp(const Vec4& a, const Vec4& b, float t) {
    return a * (1.f - t) + b * t;
}

Color tun::Lerp(const Color& a, const Color& b, float t) {
    return a * (1.f - t) + b * t;
}

Quat tun::Lerp(const Quat& a, const Quat& b, float t) {
    return a * (1.f - t) + b * t;
}

Matrix tun::Lerp(const Matrix& a, const Matrix& b, float t) {
    return a * (1.f - t) + b * t;
}

Color tun::LerpThrough(const Color& a, const Color& c, const Color& b, float t) {
    if (t < 0.5f) {
        return tun::Lerp(a, c, t * 2.f);
    } else {
        return tun::Lerp(c, b, (t - 0.5f) * 2.f);
    }
}

Frustum tun::ExtractFrustumPlanes(float fovy, float aspect, float znear, float zfar, const Matrix& projView, const Matrix& view) {
    Frustum frustum {};
    
    Vec4 rowX = glm::row(projView, 0);
    Vec4 rowY = glm::row(projView, 1);
    Vec4 rowZ = glm::row(projView, 2);
    Vec4 rowW = glm::row(projView, 3);

    // Gribb-Hartmann plane extraction
    frustum.planes.push_back({Vec(rowW + rowX), rowW.w + rowX.w}); // Left
    frustum.planes.push_back({Vec(rowW - rowX), rowW.w - rowX.w}); // Right
    frustum.planes.push_back({Vec(rowW + rowY), rowW.w + rowY.w}); // Bottom
    frustum.planes.push_back({Vec(rowW - rowY), rowW.w - rowY.w}); // Top
    frustum.planes.push_back({Vec(rowW + rowZ), rowW.w + rowZ.w}); // Near
    frustum.planes.push_back({Vec(rowW - rowZ), rowW.w - rowZ.w}); // Far

    // Normalize planes for accurate distance calculations
    for (auto& plane : frustum.planes) {
        float length = glm::length(plane.normal);
        if (length > 1e-6f) {
            plane.normal /= length;
            plane.distance /= length;
        } else {
            plane.normal = glm::vec3(0.0f);
            plane.distance = 0.0f;
        }
    }

    float tanHalfFovy = std::tan(fovy / 2.0f);
    float nearHeight = znear * tanHalfFovy;
    float nearWidth = nearHeight * aspect;
    float farHeight = zfar * tanHalfFovy;
    float farWidth = farHeight * aspect;

    // Compute 8 corners in view space (camera at origin, looking down -z)
    std::vector<glm::vec3> viewPoints = {
        // Near plane: z = -znear
        { -nearWidth, -nearHeight, -znear }, // Bottom-left
        {  nearWidth, -nearHeight, -znear }, // Bottom-right
        {  nearWidth,  nearHeight, -znear }, // Top-right
        { -nearWidth,  nearHeight, -znear }, // Top-left
        // Far plane: z = -zfar
        { -farWidth, -farHeight, -zfar },    // Bottom-left
        {  farWidth, -farHeight, -zfar },    // Bottom-right
        {  farWidth,  farHeight, -zfar },    // Top-right
        { -farWidth,  farHeight, -zfar }     // Top-left
    };

    // Transform to world space
    glm::mat4 invView = glm::inverse(view);
    for (size_t i = 0; i < 8; ++i) {
        glm::vec4 world = invView * glm::vec4(viewPoints[i], 1.0f);
        frustum.points.push_back(Vec(world) / world.w);
    }

    return frustum;
}

bool tun::IsBoundingBoxInSphere(const BoundingBox& box, const Vec& spherePos, float radius) {
    // Check for degenerate AABB
    if (glm::any(glm::greaterThan(box.min, box.max))) {
        return false; // Invalid AABB
    }

    // Find the closest point on the AABB to the sphere's center
    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(spherePos.x, box.min.x, box.max.x);
    closestPoint.y = glm::clamp(spherePos.y, box.min.y, box.max.y);
    closestPoint.z = glm::clamp(spherePos.z, box.min.z, box.max.z);

    // Calculate the distance between the sphere's center and the closest point
    float distanceSquared = glm::dot(spherePos - closestPoint, spherePos - closestPoint);

    // Check if the distance is less than or equal to the sphere's radius squared
    return distanceSquared <= radius * radius;
}

bool tun::IsBoundingBoxInFrustum(const BoundingBox& box, const Frustum& frustum) {
    // Check for degenerate AABB
    if (glm::any(glm::greaterThan(box.min, box.max))) {
        return true;
    }

    // Part 1: Check AABB vertices against frustum planes
    for (const auto& plane : frustum.planes) {
        int out = 0;
        // Test all 8 AABB vertices
        out += (glm::dot(plane.normal, glm::vec3(box.min.x, box.min.y, box.min.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.max.x, box.min.y, box.min.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.min.x, box.max.y, box.min.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.max.x, box.max.y, box.min.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.min.x, box.min.y, box.max.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.max.x, box.min.y, box.max.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.min.x, box.max.y, box.max.z)) + plane.distance < 0.0f) ? 1 : 0;
        out += (glm::dot(plane.normal, glm::vec3(box.max.x, box.max.y, box.max.z)) + plane.distance < 0.0f) ? 1 : 0;
        if (out == 8) {
            return false; // All vertices outside this plane
        }
    }

    // Part 2: Check frustum vertices against AABB bounds
    int out;

    out = 0; for (const auto& p : frustum.points) out += (p.x > box.max.x) ? 1 : 0; if (out == 8) return false;
    out = 0; for (const auto& p : frustum.points) out += (p.x < box.min.x) ? 1 : 0; if (out == 8) return false;
    out = 0; for (const auto& p : frustum.points) out += (p.y > box.max.y) ? 1 : 0; if (out == 8) return false;
    out = 0; for (const auto& p : frustum.points) out += (p.y < box.min.y) ? 1 : 0; if (out == 8) return false;
    out = 0; for (const auto& p : frustum.points) out += (p.z > box.max.z) ? 1 : 0; if (out == 8) return false;
    out = 0; for (const auto& p : frustum.points) out += (p.z < box.min.z) ? 1 : 0; if (out == 8) return false;

    return true; // AABB intersects or is inside frustum
}

BoundingBox tun::TransformAABB(const Matrix& transformMatrix, const BoundingBox& localAABB, const Vec& offset, const Vec& shapeSize) {
    glm::vec3 vertices[8] = {
        { localAABB.min.x, localAABB.min.y, localAABB.min.z },
        { localAABB.max.x, localAABB.min.y, localAABB.min.z },
        { localAABB.min.x, localAABB.max.y, localAABB.min.z },
        { localAABB.max.x, localAABB.max.y, localAABB.min.z },
        { localAABB.min.x, localAABB.min.y, localAABB.max.z },
        { localAABB.max.x, localAABB.min.y, localAABB.max.z },
        { localAABB.min.x, localAABB.max.y, localAABB.max.z },
        { localAABB.max.x, localAABB.max.y, localAABB.max.z }
    };

    glm::vec3 worldMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 worldMax = glm::vec3(-std::numeric_limits<float>::max());

    for (const auto& vertex : vertices) {
        glm::vec3 worldVertex = Vec(transformMatrix * glm::vec4(vertex, 1.0f));

        worldMin = glm::min(worldMin, worldVertex);
        worldMax = glm::max(worldMax, worldVertex);
    }

    return { worldMin, worldMax };
}

float tun::CubicBezier(float t, float x1, float y1, float x2, float y2) {
    t = tun::Clamp(t, 0.f, 1.f);
    
    float mt = 1.f - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    float t2 = t * t;
    float t3 = t2 * t;

    return 3.0f * mt2 * t * y1 + 3.0f * mt * t2 * y2 + t3;
}

float tun::CurveAuto(float t) {
    return tun::CubicBezier(t, 0.47f, 0.17f, 0.45f, 0.94f);
}

float tun::CurvePunch(float t) {
    return tun::CubicBezier(t, 0.22f, 1.04f, 0.68f, 0.14f);
}
