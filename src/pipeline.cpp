#include "river.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include <filesystem>
#include <fstream>
#include <vector>

static VkShaderModule createShaderModule(const std::vector<char> &code){
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device::logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
        river::printDebugLog("failed to create shader module!", 0, 2);
        throw std::runtime_error("failed to create shader module!");
    }else{
        river::printDebugLog("Successfully created shader module.", 0, 2);
    }

    return shaderModule;
}

static std::vector<char> readFile(const std::string &filename){
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if(!file.is_open()){
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    if(buffer.size() != fileSize){
        river::printDebugLog("failed to correctly read from file!", 0, 2);
        throw std::runtime_error("failed to correctly read from buffer!");
    }

    file.close();
    return buffer;
}

void pipeline::createGraphicsPipeline(){
    using namespace river;
    printDebugLog("current directory: ", 0, 0);
    printDebugLog(std::filesystem::current_path().string(), 0, 2);
    
    const std::filesystem::path vertPath = appRoot / "river\\bin\\vertTest.vert.spv";
    const std::filesystem::path fragPath = appRoot / "river\\bin\\fragTest.frag.spv";

    auto vertShaderCode = readFile(vertPath.string());
    auto fragShaderCode = readFile(fragPath.string());

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain::swapChainExtent.width;
    viewport.height = (float)swapchain::swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain::swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.sampleShadingEnable = VK_FALSE;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample.minSampleShading = 1.0f;
    multisample.pSampleMask = nullptr;
    multisample.alphaToCoverageEnable = VK_FALSE;
    multisample.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = VK_FALSE;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if(vkCreatePipelineLayout(device::logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create pipeline layout!", 2, 1);
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisample;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlend;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    if((vkCreateGraphicsPipelines(device::logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create graphics pipeline!", 2, 1);
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device::logicalDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(device::logicalDevice, fragShaderModule, nullptr);
}

void pipeline::createFramebuffers(){
    swapchain::swapChainFramebuffers.resize(swapchain::swapChainImageViews.size());

    for(size_t i = 0; i < swapchain::swapChainImageViews.size(); ++i){
        VkImageView attachments[] = { swapchain::swapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline::renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain::swapChainExtent.width;
        framebufferInfo.height = swapchain::swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(device::logicalDevice, &framebufferInfo, nullptr, &swapchain::swapChainFramebuffers[i]) != VK_SUCCESS){
            river::printDebugLog("\nERROR: failed to create framebuffer!", 2, 1);
            throw std::runtime_error("failed to create framebuffer!");
        }else{
            river::printDebugLog("Successfully created framebuffer.", 0, 1);
        }
    }
}

void pipeline::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if((vkBeginCommandBuffer(commandBuffer, &beginInfo)) != VK_SUCCESS){
        river::printDebugLog("\nERROR: failed to begin recording command buffer!", 2, 1);
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline::renderPass;
    renderPassInfo.framebuffer = swapchain::swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain::swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline::graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain::swapChainExtent.width);
    viewport.height = static_cast<float>(swapchain::swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain::swapChainExtent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //NOTE: I'm specifyin the num of verts... lmfao
    vkCmdDraw(commandBuffer, 3, 2, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        river::printDebugLog("\nERROR: failed to record command buffer!", 2, 1);
        throw std::runtime_error("failed to record command buffer!");
    }
}

void pipeline::createCommandPool(){
    QueueFamilyIndices queueFamilyIndices = device::findQueueFamilies(device::physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if((vkCreateCommandPool(device::logicalDevice, &poolInfo, nullptr, &pipeline::commandPool)) != VK_SUCCESS){
        river::printDebugLog("\nERROR: failed to create command pool!", 2, 1);
        throw std::runtime_error("failed to create command pool!");
    }else{
        river::printDebugLog("Successfully created command pool.", 0, 1);
    }
}

void pipeline::createCommandBuffer(){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pipeline::commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(device::logicalDevice, &allocInfo, &pipeline::commandBuffer) != VK_SUCCESS){
        river::printDebugLog("\nERROR: failed to allocate command buffers!", 2, 1);
        throw std::runtime_error("failed to allocate command buffers!");
    }else{
        river::printDebugLog("Successfully allocated command buffer.", 0, 1);
    }
}

void pipeline::createSyncObjects(){
    using namespace river;
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if((vkCreateSemaphore(device::logicalDevice, &semaphoreInfo, nullptr, &pipeline::imageAvailableSemaphore)) != VK_SUCCESS || (vkCreateSemaphore(device::logicalDevice, &semaphoreInfo, nullptr, &pipeline::renderFinishedSemaphore)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create semaphores!", 2, 1);
        throw std::runtime_error("failed to create semaphores!");
    }else{
        printDebugLog("Successfully created semaphores.", 0, 1);
    }
    if((vkCreateFence(device::logicalDevice, &fenceInfo, nullptr, &pipeline::inFlightFence)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create fence!", 2, 1);
        throw std::runtime_error("failed to create fence!");
    }else{
        printDebugLog("Successfully created fence.", 0, 1);
    }
}

