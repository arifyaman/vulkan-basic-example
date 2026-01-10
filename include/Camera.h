#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera for 3D rendering with configurable parameters
class Camera
{
public:
    Camera()
        : position(2.0f, 2.0f, 2.0f),
          target(0.0f, 0.0f, 0.0f),
          up(0.0f, 0.0f, 1.0f),
          fov(45.0f),
          aspectRatio(16.0f / 9.0f),
          nearPlane(0.1f),
          farPlane(100.0f)
    {
    }

    // Position and orientation
    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setTarget(const glm::vec3 &tgt) { target = tgt; }
    void setUp(const glm::vec3 &u) { up = u; }

    const glm::vec3 &getPosition() const { return position; }
    const glm::vec3 &getTarget() const { return target; }
    const glm::vec3 &getUp() const { return up; }

    // Projection parameters
    void setFov(float fovDegrees) { fov = fovDegrees; }
    void setAspectRatio(float aspect) { aspectRatio = aspect; }
    void setNearPlane(float near) { nearPlane = near; }
    void setFarPlane(float far) { farPlane = far; }

    float getFov() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }

    // Get focal length (in mm, assuming 35mm sensor)
    float getFocalLength() const
    {
        // Focal length = sensor_width / (2 * tan(fov/2))
        // For 35mm sensor width
        return 36.0f / (2.0f * tanf(glm::radians(fov) / 2.0f));
    }

    // Set focal length (in mm, assuming 35mm sensor)
    void setFocalLength(float focalLengthMm)
    {
        // fov = 2 * atan(sensor_width / (2 * focal_length))
        fov = glm::degrees(2.0f * atanf(36.0f / (2.0f * focalLengthMm)));
    }

    // Get view matrix
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(position, target, up);
    }

    // Get projection matrix (Vulkan convention: Y-inverted)
    glm::mat4 getProjectionMatrix() const
    {
        glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        proj[1][1] *= -1; // Flip Y for Vulkan
        return proj;
    }

    // Camera movement helpers
    void lookAt(const glm::vec3 &pos, const glm::vec3 &tgt, const glm::vec3 &u)
    {
        position = pos;
        target = tgt;
        up = u;
    }

    // Orbit around target
    void orbit(float angleRadians, const glm::vec3 &axis)
    {
        glm::vec3 dir = position - target;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
        dir = glm::vec3(rotation * glm::vec4(dir, 0.0f));
        position = target + dir;
    }

    // Move camera (e.g., for panning)
    void translate(const glm::vec3 &offset)
    {
        position += offset;
        target += offset;
    }

    // Zoom (move closer/farther from target)
    void zoom(float factor)
    {
        glm::vec3 dir = position - target;
        float distance = glm::length(dir);
        distance *= factor;
        position = target + glm::normalize(dir) * distance;
    }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float fov;         // Field of view in degrees
    float aspectRatio; // Width / Height
    float nearPlane;
    float farPlane;
};
