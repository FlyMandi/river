#include "image.h"
#include "vulkan/vulkan_core.h"

#include "river.h"
#include "device.h"
#include "swapchain.h"
#include "buffer.h"
#include "pipeline.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

internal VkShaderModule createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    riverAssertVkSuccess
    (
        vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule),
        "failed to create shader module!"
    );

    return shaderModule;
}

internal std::vector<char> readFile(const std::filesystem::path &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    riverAssert(file.is_open(), "failed to open file! check cwd.");

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    riverAssert(buffer.size() == fileSize, "failed to correctly read from file!");

    file.close();
    return buffer;
}

void createGraphicsPipeline
(
    const EngineData        &engine,
    const ProjectManifest   &manifest
){
    auto vertShaderCode = readFile(manifest.vertexShader);
    auto fragShaderCode = readFile(manifest.fragmentShader);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
    vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageCreateInfo.module = vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
    fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageCreateInfo.module = fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    auto bindingDescription = getVertexBindingDescription();
    auto attributeDescriptions = getVertexAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)engine.swapchainExtent.width;
    viewport.height = (float)engine.swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = engine.swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{};
    rasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerStateCreateInfo.lineWidth = 1.0f;
    rasterizerStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizerStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizerStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizerStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask =  VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT |
                                                VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = {};
    depthStencilStateCreateInfo.back = {};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    riverAssertVkSuccess
    (
        vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &graphicsPipelineLayout),
        "failed to create pipeline layout!"
    );

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = shaderStages;

    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

    graphicsPipelineCreateInfo.layout = graphicsPipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;

    riverAssertVkSuccess
    (
        vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline),
        "failed to create graphics pipeline!"
    );

    vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
}

void createFramebuffers
(
    EngineData &engine
){
    engine.swapchainFramebuffers.resize(engine.swapchainImageViews.size());

    for(size_t i = 0; i < engine.swapchainImageViews.size(); ++i)
    {
        std::array<VkImageView, 2> attachments =
        {
            engine.swapchainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = engine.swapchainExtent.width;
        framebufferCreateInfo.height = engine.swapchainExtent.height;
        framebufferCreateInfo.layers = 1;

        riverAssertVkSuccess
        (
            vkCreateFramebuffer
            (
                logicalDevice,
                &framebufferCreateInfo,
                nullptr,
                &engine.swapchainFramebuffers[i]
            ),
            "failed to create framebuffer!"
        );
    }
}

void recordCommandBuffer
(
    EngineData      &engine,
    VkCommandBuffer &commandBuffer,
    uint32_t        &imageIndex
){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    riverAssertVkSuccess
    (
        vkBeginCommandBuffer(commandBuffer, &beginInfo),
        "failed to begin recording command buffer!"
    );

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = engine.swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = engine.swapchainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, vertexBuffer, vertSize, VK_INDEX_TYPE_UINT32);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(engine.swapchainExtent.width);
    viewport.height = static_cast<float>(engine.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = engine.swapchainExtent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets
    (
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsPipelineLayout,
        0,
        1,
        &descriptorSets[currentFrame],
        0,
        nullptr
    );

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(vertexIndices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    riverAssertVkSuccess(vkEndCommandBuffer(commandBuffer), "failed to end recording command buffer!");
}

void createCommandPools()
{
    VkCommandPoolCreateInfo graphicsPoolInfo{};
    graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    graphicsPoolInfo.queueFamilyIndex = logicalQueueFamilies.graphicsIndex;

    riverAssertVkSuccess
    (
        vkCreateCommandPool(logicalDevice, &graphicsPoolInfo, nullptr, &graphicsCommandPool),
        "failed to create graphics command pool!"
    );

    VkCommandPoolCreateInfo transferPoolInfo{};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    transferPoolInfo.queueFamilyIndex = logicalQueueFamilies.transferIndex;

    riverAssertVkSuccess
    (
        vkCreateCommandPool(logicalDevice, &transferPoolInfo, nullptr, &transferCommandPool),
        "failed to create transfer command pool!"
    );
}

VkCommandBuffer setupCommandBuffer(VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo commandbufAllocInfo{};
    commandbufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandbufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandbufAllocInfo.commandPool = commandPool;
    commandbufAllocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    riverAssertVkSuccess
    (
        vkAllocateCommandBuffers(logicalDevice, &commandbufAllocInfo, &commandBuffer),
        "failed to allocate single time command buffer!"
    );

    VkCommandBufferBeginInfo commandbufBeginInfo{};
    commandbufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandbufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &commandbufBeginInfo);

    return commandBuffer;
}

void flushCommandBuffer
(
    VkCommandBuffer commandBuffer,
    VkCommandPool   commandPool,
    VkQueue         queue
){
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo singleTimeSubmitInfo{};
    singleTimeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    singleTimeSubmitInfo.commandBufferCount = 1;
    singleTimeSubmitInfo.pCommandBuffers = &commandBuffer;

    VkFence queueFence;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    riverAssertVkSuccess
    (
        vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &queueFence),
        "failed to create queue fence!"
    );

    vkQueueSubmit(queue, 1, &singleTimeSubmitInfo, queueFence);

    vkWaitForFences(logicalDevice, 1, &queueFence, VK_TRUE, UINT64_MAX);

    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    vkDestroyFence(logicalDevice, queueFence, nullptr);
}

void createCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphicsCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    riverAssertVkSuccess
    (
        vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()),
        "failed to allocate command buffers!"
    );
}

void cleanupSyncObjects
(
    const EngineData &engine
){
    for(size_t i = 0; i < engine.swapchainImages.size(); ++i)
    {
        vkDestroySemaphore(logicalDevice, imageReadyForWriteSemaphores[i], nullptr);
        imageReadyForWriteSemaphores[i] = VK_NULL_HANDLE;
        riverLog(std::format("destroyed imageReadyForWriteSemaphore No. {}.", i), RIV_LOG_LEVEL_TRACE);

        vkDestroySemaphore(logicalDevice, imageReadyForPresentSemaphores[i], nullptr);
        imageReadyForPresentSemaphores[i] = VK_NULL_HANDLE;
        riverLog(std::format("destroyed imageReadyForPresentSemaphore No. {}.", i), RIV_LOG_LEVEL_TRACE);

        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
        inFlightFences[i] = VK_NULL_HANDLE;
        riverLog(std::format("destroyed inFlightFence No. {}", i), RIV_LOG_LEVEL_TRACE);
    }
    vkDestroySemaphore(logicalDevice, acquireSemaphore, nullptr);
    acquireSemaphore = VK_NULL_HANDLE;
    riverLog("destroyed acquireSemaphore.", RIV_LOG_LEVEL_TRACE);
}

void createSyncObjects
(
    const EngineData &engine
){
    imageReadyForWriteSemaphores.resize(engine.swapchainImages.size(), VK_NULL_HANDLE);
    imageReadyForPresentSemaphores.resize(engine.swapchainImages.size(), VK_NULL_HANDLE);
    inFlightFences.resize(engine.swapchainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    riverAssertVkSuccess
    (
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &acquireSemaphore),
        "failed to create acquireSemaphore."
    );

    for(size_t i = 0; i < engine.swapchainImages.size(); ++i)
    {
        riverAssertVkSuccess
        (
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageReadyForPresentSemaphores[i]),
            std::format("failed to create imageReadyForPresentSemaphore No. {}", i)
        );

        if(imageReadyForWriteSemaphores[i] == VK_NULL_HANDLE)
        {
            riverAssertVkSuccess
            (
                vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageReadyForWriteSemaphores[i]),
                std::format("failed to create imageReadyForWriteSemaphore No. {}", i)
            );
        }

        if(inFlightFences[i] == VK_NULL_HANDLE)
        {
            riverAssertVkSuccess
            (
                vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]),
                std::format("failed to create inFlightFence No. {}", i)
            );
        }
    }
}

void createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
    uniformBufferLayoutBinding.binding = 0;
    uniformBufferLayoutBinding.descriptorCount = 1;
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.pImmutableSamplers = nullptr;
    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
    textureSamplerLayoutBinding.binding = 1;
    textureSamplerLayoutBinding.descriptorCount = 1;
    textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSamplerLayoutBinding.pImmutableSamplers = nullptr;
    textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings =
    {
        uniformBufferLayoutBinding,
        textureSamplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    riverAssertVkSuccess
    (
        vkCreateDescriptorSetLayout
        (
            logicalDevice,
            &descriptorSetLayoutCreateInfo,
            nullptr,
            &descriptorSetLayout
        ),
        "failed to create descriptor set layout!"
    );
}

void createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorPoolCreateInfo.flags = 0;

    riverAssertVkSuccess
    (
        vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool),
        "failed to create descriptor pool."
    );
}

void createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> setLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
    descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocInfo.descriptorPool = descriptorPool;
    descriptorSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorSetAllocInfo.pSetLayouts = setLayouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    riverAssertVkSuccess
    (
        vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocInfo, descriptorSets.data()),
        "failed to allocate descriptor sets!"
    );

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = uniformBuffers[i];
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = textureImageView;
        descriptorImageInfo.sampler = textureSampler;

        VkWriteDescriptorSet uniformWriteDescriptorSet{};
        uniformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformWriteDescriptorSet.dstSet = descriptorSets[i];
        uniformWriteDescriptorSet.dstBinding = 0;
        uniformWriteDescriptorSet.dstArrayElement = 0;
        uniformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformWriteDescriptorSet.descriptorCount = 1;
        uniformWriteDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        uniformWriteDescriptorSet.pImageInfo = nullptr;
        uniformWriteDescriptorSet.pTexelBufferView = nullptr;

        VkWriteDescriptorSet samplerWriteDescriptorSet{};
        samplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        samplerWriteDescriptorSet.dstSet = descriptorSets[i];
        samplerWriteDescriptorSet.dstBinding = 1;
        samplerWriteDescriptorSet.dstArrayElement = 0;
        samplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerWriteDescriptorSet.descriptorCount = 1;
        samplerWriteDescriptorSet.pBufferInfo = nullptr;
        samplerWriteDescriptorSet.pImageInfo = &descriptorImageInfo;
        samplerWriteDescriptorSet.pTexelBufferView = nullptr;

        std::array<VkWriteDescriptorSet, 2> descriptorSetWrites =
        {
            uniformWriteDescriptorSet,
            samplerWriteDescriptorSet
        };

        vkUpdateDescriptorSets
        (
            logicalDevice,
            static_cast<uint32_t>(descriptorSetWrites.size()),
            descriptorSetWrites.data(),
            0,
            nullptr
        );
    }
}
