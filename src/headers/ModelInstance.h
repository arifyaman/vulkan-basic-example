#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Material.h"

// Represents an instance of a model with its own transform
class ModelInstance
{
public:
    explicit ModelInstance(const std::string &modelName = "default")
        : modelName(modelName),
          instanceName(modelName + "_instance"),
          position(0.0f),
          scale(1.0f),
          rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), // Identity quaternion
          material(Material())
    {
    }

    // Constructor with separate model and instance names
    ModelInstance(const std::string &model, const std::string &instance)
        : modelName(model),
          instanceName(instance),
          position(0.0f),
          scale(1.0f),
          rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), // Identity quaternion
          material(Material())
    {
    }

    // Model reference getters
    const std::string &getModelName() const { return modelName; }
    const std::string &getInstanceName() const { return instanceName; }

    // Transform getters
    const glm::vec3 &getPosition() const { return position; }
    const glm::vec3 &getScale() const { return scale; }
    const glm::quat &getRotation() const { return rotation; }
    const Material &getMaterial() const { return material; }
    Material &getMaterial() { return material; }

    // Model reference setters
    void setModelName(const std::string &model) { modelName = model; }
    void setInstanceName(const std::string &instance) { instanceName = instance; }

    // Transform setters
    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setScale(const glm::vec3 &s) { scale = s; }
    void setRotation(const glm::quat &rot) { rotation = glm::normalize(rot); }
    void setMaterial(const Material &mat) { material = mat; }

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
    std::string modelName;     // Name of the model this instance uses
    std::string instanceName;  // Unique name for this instance
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
    Material material;         // Material properties (colors, shininess)
};
