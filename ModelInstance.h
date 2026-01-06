#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Represents an instance of a model with its own transform
class ModelInstance
{
public:
    explicit ModelInstance(const std::string &modelName = "default")
        : name(modelName),
          position(0.0f),
          scale(1.0f),
          rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) // Identity quaternion
    {
    }

    // Transform getters
    const glm::vec3 &getPosition() const { return position; }
    const glm::vec3 &getScale() const { return scale; }
    const glm::quat &getRotation() const { return rotation; }
    const std::string &getName() const { return name; }

    // Transform setters
    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setScale(const glm::vec3 &s) { scale = s; }
    void setRotation(const glm::quat &rot) { rotation = glm::normalize(rot); }

    // Transform modifiers
    void translate(const glm::vec3 &offset) { position += offset; }
    void scaleBy(const glm::vec3 &factor) { scale *= factor; }
    void rotate(const glm::quat &rot) { rotation = glm::normalize(rot * rotation); }

    // Get the model matrix from the transform
    glm::mat4 getModelMatrix() const
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotation_mat = glm::mat4_cast(rotation);
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

        return translation * rotation_mat * scaling;
    }

private:
    std::string name;
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};
