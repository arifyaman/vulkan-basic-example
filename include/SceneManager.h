#pragma once

#include "Model.h"
#include "ModelInstance.h"
#include <map>
#include <memory>
#include <vector>

// Manages a collection of model instances (the scene)
class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    // Add a new instance to the scene
    void addInstance(const std::shared_ptr<ModelInstance> &instance)
    {
        instances.push_back(instance);
    }

    // Remove an instance by index
    void removeInstance(size_t index)
    {
        if (index < instances.size())
        {
            instances.erase(instances.begin() + index);
        }
    }

    // Get instance by index
    std::shared_ptr<ModelInstance> getInstance(size_t index) const
    {
        if (index < instances.size())
        {
            return instances[index];
        }
        return nullptr;
    }

    // Get all instances
    const std::vector<std::shared_ptr<ModelInstance>> &getInstances() const
    {
        return instances;
    }

    // Get number of instances
    size_t getInstanceCount() const { return instances.size(); }

    // Clear all instances
    void clear() { instances.clear(); }

    // Load or get a model
    void setModel(const std::string &modelName, const std::shared_ptr<Model> &model)
    {
        models[modelName] = model;
    }

    // Get a model by name
    std::shared_ptr<Model> getModel(const std::string &modelName) const
    {
        auto it = models.find(modelName);
        if (it != models.end())
        {
            return it->second;
        }
        return nullptr;
    }

    // Get the default/primary model
    std::shared_ptr<Model> getPrimaryModel() const
    {
        if (!models.empty())
        {
            return models.begin()->second;
        }
        return nullptr;
    }

    // Cleanup all models
    void cleanupModels(VkDevice device)
    {
        for (auto &pair : models)
        {
            if (pair.second)
            {
                pair.second->cleanup(device);
            }
        }
        models.clear();
    }

private:
    std::vector<std::shared_ptr<ModelInstance>> instances;
    std::map<std::string, std::shared_ptr<Model>> models;
};
