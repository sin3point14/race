#include "renderer.h"
#include <iostream>
#include <set>

void Renderer::createInstance(const char** extensions, uint32_t extensionsCount) {
    if (s_EnableValidationLayers) {
        if (!checkValidationLayerSupport()) {
            std::cerr << "Validation layers requested, but not available!" << std::endl;
        }
        else {
            std::cout << "Validation layers enabled" << std::endl;
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionsCount;
    createInfo.ppEnabledExtensionNames = extensions;

    if (s_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!";
        exit(-1);
    }
}


bool Renderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : s_ValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::optional<uint32_t> Renderer::findQueueFamilies(const VkPhysicalDevice& device) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    std::optional<uint32_t> retVal;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
        if (presentSupport && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            retVal = i;
            break;
        }
        i++;
    }
    return retVal;
}

bool Renderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

// Check if required queue families and extensions are supported
// then rate them by capabilities
int Renderer::rateDeviceSuitability(const VkPhysicalDevice& device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    bool found = false;

    if (!checkDeviceExtensionSupport(device)) {
        return 0;
    }

    if (!findQueueFamilies(device).has_value()) {
        return 0;
    }

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

void Renderer::createSurface(const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator) {
    if (!surfaceCreator(m_Instance, m_Surface)) {
        std::cerr << "Failed to create window surface!" << std::endl;
        exit(-1);
    }
}

void Renderer::pickPhysicalDevice() {
    m_PhysicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!";
        exit(-1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    int bestScore = 0;

    for (const auto& device : devices) {
        if (int currScore = rateDeviceSuitability(device)) {
            m_PhysicalDevice = currScore > bestScore ? device : VK_NULL_HANDLE;
            bestScore = currScore > bestScore ? currScore : bestScore;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        std::cerr << "Failed to find a suitable GPU!";
        exit(-1);
    }

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &deviceFeatures);

    std::cout << "Selected Vulkan device\t" << std::endl;
    std::cout << "Name:\t\t\t" << deviceProperties.deviceName << std::endl;
    std::cout << "Device Type:\t\t";
    switch (deviceProperties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        std::cout << "Other";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        std::cout << "Integrated";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        std::cout << "Discrete";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        std::cout << "Virtual";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        std::cout << "CPU";
        break;
    default:
        std::cout << "Unknown";
        break;
    }
    std::cout << std::endl;
}

void Renderer::createLogicalDevice() {
    uint32_t index = findQueueFamilies(m_PhysicalDevice).value();

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = index;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = s_DeviceExtensions.size();
    createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

    if (s_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        std::cerr << "Failed to create logical device!" << std::endl;
    }

    vkGetDeviceQueue(m_Device, index, 0, &m_GraphicsPresentQueue);
}

Renderer::Renderer(const char** extensions, uint32_t extensionsCount, const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator) {
    createInstance(extensions, extensionsCount);
    createSurface(surfaceCreator);
    pickPhysicalDevice();
    createLogicalDevice();
}

void Renderer::render(float time) {

}

Renderer::~Renderer() {
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}
