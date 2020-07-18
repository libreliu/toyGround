#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <set>

class SillyDispatcher {
public:
    static VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) noexcept {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) noexcept {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

};

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *window;
    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;
    vk::DispatchLoaderDynamic dldi;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;
    SillyDispatcher dldis;

    constexpr static bool enableValidationLayers = true;
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        //dldi.init(instance);
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR *>(&surface)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void pickPhysicalDevice() {
        // enumerate devices
        auto devices = instance.enumeratePhysicalDevices();

        // print all devices
        std::cout << "Available devices:" << std::endl;
        for (const auto& device: devices) {
            std::cout << " - " << device.getProperties().deviceName << std::endl;
            std::cout << "   Type: " << vk::to_string(device.getProperties().deviceType) << std::endl;
            std::cout << "   Geometry Shader: " << device.getFeatures().geometryShader << std::endl;
        }

        for (const auto& device: devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                std::cout << "Selected " << physicalDevice.getProperties().deviceName << "." << std::endl;
            }
        }
    }

    // returns the queue with at least one of the given capability
    std::optional<uint32_t> findPhysQueueIndice(enum class vk::QueueFlagBits flag) {
        std::optional<uint32_t> index;
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        int i = 0;
        for (const auto& queueFamily: queueFamilies) {
            if (queueFamily.queueFlags & flag) {
                index = i;
                break;
            }
            i++;
        }
        return index;
    }

    std::optional<uint32_t> findPhysQueueIndicePresent() {
        std::optional<uint32_t> index;
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        int i = 0;
        for (const auto& queueFamily: queueFamilies) {
            if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
                index = i;
                break;
            }
            i++;
        }
        return index;
    }

    void createLogicalDevice() {
        // find the appropriate queue with graphics support
        auto graphicsIndex = findPhysQueueIndice(vk::QueueFlagBits::eGraphics);
        auto presentIndex = findPhysQueueIndicePresent();
        assert(graphicsIndex.has_value() && presentIndex.has_value());

        std::set<uint32_t> uniqueQueueFamilies = {graphicsIndex.value(), presentIndex.value()};

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamily: uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo(
                {},
                queueFamily,
                1,
                std::array<float, 1>{1.0f}.data()    // queue priorities
            );
            queueCreateInfos.push_back(queueCreateInfo);
        }

        if constexpr (enableValidationLayers) {
            vk::DeviceCreateInfo createInfo(
                {},
                static_cast<uint32_t>(queueCreateInfos.size()),
                queueCreateInfos.data(),
                static_cast<uint32_t>(validationLayers.size()),
                validationLayers.data(),
                0,
                nullptr,
                &vk::PhysicalDeviceFeatures{}     // no feature to be enabled
            );

            device = physicalDevice.createDevice(createInfo);
        } else {
            vk::DeviceCreateInfo createInfo(
                {},
                static_cast<uint32_t>(queueCreateInfos.size()),
                queueCreateInfos.data(),
                0,
                nullptr,
                0,
                nullptr,
                &vk::PhysicalDeviceFeatures{}     // no feature to be enabled
            );

            device = physicalDevice.createDevice(createInfo);
        }

        graphicsQueue = device.getQueue(graphicsIndex.value(), 0);
        presentQueue = device.getQueue(presentIndex.value(), 0);
    }

    bool isDeviceSuitable(vk::PhysicalDevice device) {
        auto prop = device.getProperties();
        auto feat = device.getFeatures();

        bool haveGeometryShader = feat.geometryShader;
        bool haveGraphicsQueue = [&]() -> bool {
            bool haveRes = false;
            auto queueFamilies = device.getQueueFamilyProperties();
            int i = 0;
            for (const auto& queueFamily: queueFamilies) {
                if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                    haveRes = true;
                    break;
                }
                i++;
            }

            return haveRes;
        }();

        bool havePresentQueue = [&]() -> bool {
            bool haveRes = false;
            auto queueFamilies = device.getQueueFamilyProperties();
            int i = 0;
            for (const auto& queueFamily: queueFamilies) {
                if (device.getSurfaceSupportKHR(i, surface)) {
                    haveRes = true;
                    break;
                }
                i++;
            }
            
            return haveRes;
        }();

        return haveGeometryShader && haveGraphicsQueue && havePresentQueue;
    }

    void createInstance() {
        vk::ApplicationInfo appInfo(
            "Hello Triangle", 
            VK_MAKE_VERSION(1, 0, 0),
            "No Engine", 
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_0
        );
        
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        auto extensions = vk::enumerateInstanceExtensionProperties(nullptr);
        std::cout << "Available extensions:" << std::endl;
        for (const auto &extension: extensions) {
            std::cout << " - " << extension.extensionName << std::endl;
        }

        auto layers = vk::enumerateInstanceLayerProperties();
        std::cout << "Available layers:" << std::endl;
        for (const auto &layer: layers) {
            std::cout << " - " << layer.layerName << std::endl;
        }

        if constexpr (enableValidationLayers) {
            for (auto &name: validationLayers) {
                bool found = false;
                for (const auto& layerProp: layers) {
                    if (!strcmp(name, layerProp.layerName)) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    throw std::runtime_error("Validation layers requested, but not available");
                }
            }

            std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            vk::InstanceCreateInfo createInfo(
                {},
                &appInfo,
                static_cast<uint32_t>(validationLayers.size()),
                validationLayers.data(),
                requiredExtensions.size(),
                requiredExtensions.data()
            );

            auto debugCreateInfo = populateDebugMessengerCreateInfo();
            createInfo.setPNext(&debugCreateInfo);

            instance = vk::createInstance(createInfo, nullptr);
        } else {
            vk::InstanceCreateInfo createInfo(
                {},
                &appInfo,
                0,
                nullptr,
                glfwExtensionCount,
                glfwExtensions
            );

            instance = vk::createInstance(createInfo, nullptr);            
        }
    }

    vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() {
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError 
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback,
            nullptr
        );

        return debugCreateInfo;
    }

    void setupDebugMessenger() {
        if constexpr (!enableValidationLayers)
            return;
        
        auto createInfo = populateDebugMessengerCreateInfo();
        debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldis);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        device.destroy();

        if constexpr (enableValidationLayers) {
            instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldis);
        }

        instance.destroySurfaceKHR(surface, nullptr);
        instance.destroy(nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}