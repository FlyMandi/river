#pragma once

#include "device.h"

#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <filesystem>
#include <string>

typedef struct Debugger{

Debugger() = delete;
Debugger(VkInstance &instance, Device &device) : instance(instance), device(device){};
Debugger(const Debugger&) = delete;
Debugger operator=(const Debugger&) = delete;

VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation" 
};


VkInstance &instance;
Device &device;

bool firstOpen = true;

void setupDebugMessenger();
void DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator);
void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);

std::filesystem::path debugLog;

}Debugger;
