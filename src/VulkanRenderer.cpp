#include "headers/VulkanRenderer.h"
#include "headers/VulkanSwapChain.h"
#include "headers/VulkanTexture.h"
#include "headers/VulkanBuffer.h"
#include "headers/VulkanPipeline.h"
#include "headers/VulkanCommandManager.h"
#include "headers/VulkanDescriptorManager.h"
#include "headers/ExampleApplication.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <unordered_map>

VulkanRenderer::VulkanRenderer(VkPhysicalDevice physicalDevice, VkDevice device,
                               VkQueue graphicsQueue, VkQueue presentQueue,
                               VkSurfaceKHR surface, GLFWwindow *window,
                               VkSampleCountFlagBits msaaSamples)
    : physicalDevice(physicalDevice), device(device), graphicsQueue(graphicsQueue), 
      presentQueue(presentQueue), surface(surface), window(window), msaaSamples(msaaSamples)
{
}

VulkanRenderer::~VulkanRenderer()
{
    cleanup();
}

void VulkanRenderer::initialize()
{
    startTime = glfwGetTime();

    try
    {
        std::cout << "Initializing Vulkan Renderer..." << std::endl;
        
        // Create helper classes
        std::cout << "Creating command manager..." << std::endl;
        commandManager = std::make_shared<VulkanCommandManager>(physicalDevice, device, graphicsQueue, surface);
        commandManager->createCommandPool();
        
        std::cout << "Creating swap chain..." << std::endl;
        swapChain = std::make_shared<VulkanSwapChain>(physicalDevice, device, surface, window);
        swapChain->create();
        swapChain->createImageViews();
        
        std::cout << "Creating buffer manager..." << std::endl;
        bufferManager = std::make_shared<VulkanBuffer>(physicalDevice, device, commandManager);
        
        std::cout << "Creating texture manager..." << std::endl;
        textureManager = std::make_shared<VulkanTexture>(physicalDevice, device, commandManager);
        textureManager->createTextureSampler();
        textureManager->createDefaultTexture();
        
        std::cout << "Creating descriptor manager..." << std::endl;
        descriptorManager = std::make_shared<VulkanDescriptorManager>(device, MAX_FRAMES_IN_FLIGHT);
        descriptorManager->createDescriptorSetLayout();
        
        std::cout << "Creating pipeline manager..." << std::endl;
        pipelineManager = std::make_shared<VulkanPipeline>(physicalDevice, device, msaaSamples);
        pipelineManager->createPipelineLayout(descriptorManager->getDescriptorSetLayout());
        pipelineManager->createGraphicsPipeline("shader", swapChain->getImageFormat(), swapChain->getExtent());
        
        std::cout << "Creating color resources..." << std::endl;
        createColorResources();
        std::cout << "Creating depth resources..." << std::endl;
        createDepthResources();
        
        std::cout << "Creating uniform buffers..." << std::endl;
        createUniformBuffers();
        std::cout << "Creating material uniform buffers..." << std::endl;
        createMaterialUniformBuffers();
        std::cout << "Creating indirect buffers..." << std::endl;
        createIndirectBuffers();
        
        std::cout << "Creating descriptor pool..." << std::endl;
        descriptorManager->createDescriptorPool();
        std::cout << "Creating descriptor sets..." << std::endl;
        descriptorManager->createDescriptorSets(uniformBuffers, materialUniformBuffers, 
                                               indirectInstanceBuffers,
                                               textureManager->getDefaultTextureView(),
                                               textureManager->getTextureSampler());
        
        std::cout << "Creating command buffers..." << std::endl;
        commandManager->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
        
        std::cout << "Creating sync objects..." << std::endl;
        createSyncObjects();
        
        std::cout << "Vulkan Renderer initialization complete!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR during Vulkan Renderer initialization: " << e.what() << std::endl;
        throw;
    }
}

void VulkanRenderer::cleanup()
{
    cleanupSwapChainResources();

    // Cleanup managers (in reverse order)
    if (descriptorManager)
        descriptorManager->cleanup();
    if (pipelineManager)
        pipelineManager->cleanup();
    if (textureManager)
        textureManager->cleanup();
    if (commandManager)
        commandManager->cleanup();
    if (swapChain)
        swapChain->cleanup();

    // Cleanup uniform buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (uniformBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (materialUniformBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, materialUniformBuffers[i], nullptr);
            vkFreeMemory(device, materialUniformBuffersMemory[i], nullptr);
        }
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (indirectCommandBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, indirectCommandBuffers[i], nullptr);
            vkFreeMemory(device, indirectCommandBuffersMemory[i], nullptr);
        }
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (indirectInstanceBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, indirectInstanceBuffers[i], nullptr);
            vkFreeMemory(device, indirectInstanceBuffersMemory[i], nullptr);
        }
    }

    // Cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (renderFinishedSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        if (imageAvailableSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        if (inFlightFences[i] != VK_NULL_HANDLE)
            vkDestroyFence(device, inFlightFences[i], nullptr);
    }
}

void VulkanRenderer::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanupSwapChainResources();

    swapChain->recreate();
    createColorResources();
    createDepthResources();
}

void VulkanRenderer::cleanupSwapChainResources()
{
    if (colorImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(device, colorImageView, nullptr);
        colorImageView = VK_NULL_HANDLE;
    }
    if (colorImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(device, colorImage, nullptr);
        vkFreeMemory(device, colorImageMemory, nullptr);
        colorImage = VK_NULL_HANDLE;
        colorImageMemory = VK_NULL_HANDLE;
    }

    if (depthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(device, depthImageView, nullptr);
        depthImageView = VK_NULL_HANDLE;
    }
    if (depthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);
        depthImage = VK_NULL_HANDLE;
        depthImageMemory = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::drawFrame(std::shared_ptr<SceneManager> sceneManager)
{
    try
    {
        // Calculate FPS
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - lastTime >= 1.0)
        {
            fps = frameCount / (currentTime - lastTime);
            std::string title = "Vulkan PBR Renderer - FPS: " + std::to_string(static_cast<int>(fps));
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }

        VkResult fenceResult = vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        if (fenceResult == VK_ERROR_DEVICE_LOST)
        {
            throw std::runtime_error("Device lost!");
        }
        else if (fenceResult != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for fences!");
        }

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, 
                                                imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(currentFrame, sceneManager);

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        vkResetCommandBuffer(commandManager->getCommandBuffer(currentFrame), 0);
        recordCommandBuffer(commandManager->getCommandBuffer(currentFrame), imageIndex, sceneManager);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkCommandBuffer cmdBuffer = commandManager->getCommandBuffer(currentFrame);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR in drawFrame: " << e.what() << std::endl;
        throw;
    }
}

void VulkanRenderer::createGraphicsPipelines(const std::vector<std::string>& shaderNames)
{
    for (const auto& shaderName : shaderNames)
    {
        try
        {
            pipelineManager->createGraphicsPipeline(shaderName, swapChain->getImageFormat(), swapChain->getExtent());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to create pipeline '" << shaderName << "': " << e.what() << std::endl;
        }
    }
}

void VulkanRenderer::createColorResources()
{
    VkFormat colorFormat = swapChain->getImageFormat();
    VkExtent2D extent = swapChain->getExtent();

    textureManager->createImage(extent.width, extent.height, 1, msaaSamples, colorFormat, 
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
    
    colorImageView = textureManager->createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

    textureManager->transitionImageLayout(colorImage, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
}

void VulkanRenderer::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();
    VkExtent2D extent = swapChain->getExtent();

    textureManager->createImage(extent.width, extent.height, 1, msaaSamples, depthFormat,
                               VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    
    depthImageView = textureManager->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    textureManager->transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

void VulkanRenderer::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        bufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void VulkanRenderer::createMaterialUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(MaterialUniformBufferObject);

    materialUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    materialUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    materialUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        bufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   materialUniformBuffers[i], materialUniformBuffersMemory[i]);

        vkMapMemory(device, materialUniformBuffersMemory[i], 0, bufferSize, 0, &materialUniformBuffersMapped[i]);
    }
}

void VulkanRenderer::createIndirectBuffers()
{
    const uint32_t maxInstancesPerFrame = 1024;

    indirectCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    indirectCommandBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    indirectCommandBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
    indirectInstanceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    indirectInstanceBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    indirectInstanceBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
    indirectDrawCounts.resize(MAX_FRAMES_IN_FLIGHT, 0);

    VkDeviceSize commandBufferSize = maxInstancesPerFrame * sizeof(VkDrawIndexedIndirectCommand);
    VkDeviceSize instanceBufferSize = maxInstancesPerFrame * sizeof(IndirectInstanceData);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        bufferManager->createBuffer(commandBufferSize, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   indirectCommandBuffers[i], indirectCommandBuffersMemory[i]);

        vkMapMemory(device, indirectCommandBuffersMemory[i], 0, commandBufferSize, 0, &indirectCommandBuffersMapped[i]);

        bufferManager->createBuffer(instanceBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   indirectInstanceBuffers[i], indirectInstanceBuffersMemory[i]);

        vkMapMemory(device, indirectInstanceBuffersMemory[i], 0, instanceBufferSize, 0, &indirectInstanceBuffersMapped[i]);
    }
}

void VulkanRenderer::createSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                                         std::shared_ptr<SceneManager> sceneManager)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Transition swap chain image to color attachment
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = swapChain->getImageLayout(imageIndex);
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.image = swapChain->getImages()[imageIndex];
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    swapChain->setImageLayout(imageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // Dynamic rendering
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    const bool useMsaa = (msaaSamples != VK_SAMPLE_COUNT_1_BIT);
    colorAttachment.imageView = useMsaa ? colorImageView : swapChain->getImageViews()[imageIndex];
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = useMsaa ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{sceneManager->getFogColor().r, sceneManager->getFogColor().g, 
                                        sceneManager->getFogColor().b, sceneManager->getFogColor().a}};

    if (useMsaa)
    {
        colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachment.resolveImageView = swapChain->getImageViews()[imageIndex];
        colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkRenderingAttachmentInfo depthAttachment{};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.imageView = depthImageView;
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue.depthStencil = {1.0f, 0};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = swapChain->getExtent();
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkPipeline pipeline = pipelineManager->getPipeline("shader");
    if (pipeline == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Pipeline 'shader' not found!");
    }
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain->getExtent().width;
    viewport.height = (float)swapChain->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDescriptorSet descriptorSet = descriptorManager->getDescriptorSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
                           pipelineManager->getPipelineLayout(), 0, 1, 
                           &descriptorSet, 0, nullptr);

    // Collect instances and prepare indirect data
    std::vector<IndirectInstanceData> instanceData;
    std::vector<VkDrawIndexedIndirectCommand> indirectCommands;
    std::unordered_map<std::string, uint32_t> modelIndexMap;

    uint32_t instanceIndex = 0;
    for (size_t i = 0; i < sceneManager->getInstanceCount(); ++i)
    {
        auto instance = sceneManager->getInstance(i);
        if (!instance) continue;

        auto model = sceneManager->getModel(instance->getModelName());
        if (!model) continue;

        std::string modelKey = instance->getModelName();
        if (modelIndexMap.find(modelKey) == modelIndexMap.end())
        {
            modelIndexMap[modelKey] = static_cast<uint32_t>(modelIndexMap.size());
        }

        IndirectInstanceData instData{};
        instData.modelMatrix = instance->getModelMatrix();
        
        const Material& material = instance->getMaterial();
        instData.baseColorFactor = material.baseColorFactor;
        instData.emissiveFactor = material.emissiveFactor;
        instData.metallic = material.metallic;
        instData.roughness = material.roughness;

        instData.baseColorTextureIndex = 0;
        instData.metallicRoughnessTextureIndex = 0;
        instData.emissiveTextureIndex = 0;
        instData.vertexBufferIndex = 0;
        instData.indexBufferIndex = 0;
        instData.firstIndex = 0;
        instData.indexCount = static_cast<uint32_t>(model->getIndices().size());
        instData.vertexOffset = 0;

        instanceData.push_back(instData);

        VkDrawIndexedIndirectCommand cmd{};
        cmd.indexCount = instData.indexCount;
        cmd.instanceCount = 1;
        cmd.firstIndex = instData.firstIndex;
        cmd.vertexOffset = instData.vertexOffset;
        cmd.firstInstance = instanceIndex;

        indirectCommands.push_back(cmd);
        instanceIndex++;
    }

    if (!instanceData.empty())
    {
        memcpy(indirectInstanceBuffersMapped[currentFrame], instanceData.data(),
               instanceData.size() * sizeof(IndirectInstanceData));
        memcpy(indirectCommandBuffersMapped[currentFrame], indirectCommands.data(),
               indirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand));

        indirectDrawCounts[currentFrame] = static_cast<uint32_t>(indirectCommands.size());

        for (const auto& pair : modelIndexMap)
        {
            // Find any instance with this model name
            for (size_t i = 0; i < sceneManager->getInstanceCount(); ++i)
            {
                auto inst = sceneManager->getInstance(i);
                if (!inst) continue;
                
                auto model = sceneManager->getModel(inst->getModelName());
                if (model)
                {
                    VkBuffer vertexBuffer = model->getVertexBuffer();
                    VkBuffer indexBuffer = model->getIndexBuffer();
                    VkDeviceSize offsets[] = {0};

                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
                    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

                    vkCmdDrawIndexedIndirect(commandBuffer, indirectCommandBuffers[currentFrame],
                                            0, indirectDrawCounts[currentFrame],
                                            sizeof(VkDrawIndexedIndirectCommand));
                    break;
                }
            }
            break;  // Only process first model for now
        }
    }

    vkCmdEndRendering(commandBuffer);

    // Transition to present layout
    VkImageMemoryBarrier presentBarrier{};
    presentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    presentBarrier.oldLayout = swapChain->getImageLayout(imageIndex);
    presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    presentBarrier.image = swapChain->getImages()[imageIndex];
    presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    presentBarrier.subresourceRange.baseMipLevel = 0;
    presentBarrier.subresourceRange.levelCount = 1;
    presentBarrier.subresourceRange.baseArrayLayer = 0;
    presentBarrier.subresourceRange.layerCount = 1;
    presentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    presentBarrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &presentBarrier);

    swapChain->setImageLayout(imageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void VulkanRenderer::updateUniformBuffer(uint32_t currentImage, std::shared_ptr<SceneManager> sceneManager)
{
    auto camera = sceneManager->getCamera();
    camera->setAspectRatio(swapChain->getExtent().width / (float)swapChain->getExtent().height);

    UniformBufferObject ubo{};
    ubo.view = camera->getViewMatrix();
    ubo.proj = camera->getProjectionMatrix();
    ubo.cameraPos = camera->getPosition();

    glm::vec4 dirLight = sceneManager->getDirectionalLight();
    ubo.lightDir = glm::normalize(-glm::vec3(dirLight));
    ubo.lightColor = glm::vec3(dirLight.w * 2.0f);

    ubo.fogColor = sceneManager->getFogColor();
    ubo.fogDensity = sceneManager->getFogDensity();
    ubo.fogStart = 5.0f;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void VulkanRenderer::loadTexture(const std::string& texturePath)
{
    textureManager->loadTexture(texturePath);
}

void VulkanRenderer::loadRequiredTextures(std::shared_ptr<SceneManager> sceneManager)
{
    for (size_t i = 0; i < sceneManager->getInstanceCount(); ++i)
    {
        auto instance = sceneManager->getInstance(i);
        if (instance)
        {
            const Material& material = instance->getMaterial();
            
            if (!material.baseColorTexture.empty() && !textureManager->hasTexture(material.baseColorTexture))
            {
                try
                {
                    textureManager->loadTexture(material.baseColorTexture);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Failed to load base color texture: " << e.what() << std::endl;
                }
            }

            if (!material.metallicRoughnessTexture.empty() && !textureManager->hasTexture(material.metallicRoughnessTexture))
            {
                try
                {
                    textureManager->loadTexture(material.metallicRoughnessTexture);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Failed to load metallic/roughness texture: " << e.what() << std::endl;
                }
            }

            if (!material.emissiveTexture.empty() && !textureManager->hasTexture(material.emissiveTexture))
            {
                try
                {
                    textureManager->loadTexture(material.emissiveTexture);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Failed to load emissive texture: " << e.what() << std::endl;
                }
            }
        }
    }
}

VkFormat VulkanRenderer::findDepthFormat()
{
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat VulkanRenderer::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

// Helper functions for Model class
VkCommandPool VulkanRenderer::getCommandPool() const
{
    return commandManager->getCommandPool();
}

void VulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                  VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    bufferManager->createBuffer(size, usage, properties, buffer, bufferMemory);
}

VkCommandBuffer VulkanRenderer::beginSingleTimeCommands()
{
    return commandManager->beginSingleTimeCommands();
}

void VulkanRenderer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    commandManager->endSingleTimeCommands(commandBuffer);
}

void VulkanRenderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    bufferManager->copyBuffer(srcBuffer, dstBuffer, size);
}

uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    return bufferManager->findMemoryType(typeFilter, properties);
}
