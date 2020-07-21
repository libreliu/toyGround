#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <set>
#include <cstdint>

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
    vk::SwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;

    vk::Format swapChainImageFormat;
    // To use VkImage in render pipeline we need VkImageView
    std::vector<vk::ImageView> swapChainImageViews;

    SillyDispatcher dldis;

    constexpr static bool enableValidationLayers = true;
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb 
                // -> Use SRGB non linear colorSpace
                && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    // the swap chain resolution
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            // apply current resolution
            return capabilities.currentExtent;
        } else { // window manager reports to be no current extent available
            vk::Extent2D actualExtent = {800, 600};

            actualExtent.width = 
                std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = 
                std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

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
        createSwapChain();
        createImageViews();
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vk::ImageViewCreateInfo createInfo(
                {},
                swapChainImages[i],
                // treat image as 2D texture
                vk::ImageViewType::e2D,
                swapChainImageFormat,

                // component swizzling
                vk::ComponentMapping(
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity
                ),
                // The subresourceRange field describes what the image's purpose
                // is and which part of the image should be accessed.
                vk::ImageSubresourceRange(
                    vk::ImageAspectFlagBits::eColor,    // aspectMask
                    0,                                  // baseMipLevel
                    1,                                  // levelCount
                    0,                                  // baseArrayLayer
                    1                                   // layerCount
                )
            );

            swapChainImageViews[i] = device.createImageView(createInfo);
        }

    }

    void createSwapChain() {
        // std::tuple<vk::SurfaceCapabilitiesKHR, 
        //          std::vector<vk::SurfaceFormatKHR>,
        //          std::vector<vk::PresentModeKHR>>

        auto res = querySwapChainSupport(physicalDevice);

        vk::Extent2D extent = chooseSwapExtent(std::get<0>(res));
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(std::get<1>(res));
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(std::get<2>(res));

        uint32_t imageCount = std::get<0>(res).minImageCount + 1;

        if (std::get<0>(res).maxImageCount > 0 
            && imageCount > std::get<0>(res).maxImageCount) {
            imageCount = std::get<0>(res).maxImageCount;
        }

        // I'm skipping the assertion for now, TODO: add
        uint32_t queueFamilyIndices[] = {
            findPhysQueueIndice(vk::QueueFlagBits::eGraphics).value(),
            findPhysQueueIndicePresent().value()
        };

        // If the graphics queue family and presentation queue family are the same, 
        // which will be the case on most hardware, then we should stick to exclusive
        // mode, because concurrent mode requires you to specify at least two distinct
        // queue families.
        bool use_exclusive = (queueFamilyIndices[0] == queueFamilyIndices[1]);

        vk::SwapchainCreateInfoKHR createInfo(
            {},                                            // flags
            surface,                                       // surface
            // minImageCount is the minimum number of presentable images that the 
            // application needs. The implementation will either create the 
            // swapchain with at least that many images, or it will fail to create
            // the swapchain.
            imageCount,                                    // minImageCount
            surfaceFormat.format,                          // imageFormat
            surfaceFormat.colorSpace,                      // imageColorSpace
            extent,                                        // imageExtent
            // The imageArrayLayers specifies the amount of layers each image 
            // consists of. This is always 1 unless you are developing a 
            // stereoscopic 3D application.
            1,                                             // imageArrayLayers
            // The imageUsage bit field specifies what kind of operations
            // we'll use the images in the swap chain for. In this tutorial 
            // we're going to render directly to them, which means that 
            // they're used as color attachment. It is also possible that 
            // you'll render images to a separate image first to perform 
            // operations like post-processing.
            vk::ImageUsageFlagBits::eColorAttachment,      // imageUsage
            // Image Sharing mode, how to handle swap chain images that will be 
            // used across multiple queue families
            use_exclusive ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
            use_exclusive ? 0 : 2,                         // queueFamilyIndexCount
            use_exclusive ? nullptr : queueFamilyIndices,  // pQueueFamilyIndices

            // whether we need to transform the images in swap chain
            std::get<0>(res).currentTransform,             // preTransform
            
            // The compositeAlpha field specifies if the alpha channel should be
            // used for blending with other windows in the window system.
            vk::CompositeAlphaFlagBitsKHR::eOpaque,        // compositeAlpha

            presentMode,                                   // presentMode

            // clipped
            //  If the clipped member is set to VK_TRUE then that means that we 
            // don't care about the color of pixels that are obscured, for example 
            // because another window is in front of them. Unless you really need 
            // to be able to read these pixels back and get predictable results, 
            // you'll get the best performance by enabling clipping.
            VK_TRUE,                                       // clipped

            // oldSwapchain
            // With Vulkan it's possible that your swap chain becomes invalid or
            // unoptimized while your application is running, for example because 
            // the window was resized. In that case the swap chain actually needs
            // to be recreated from scratch and a reference to the old one must be
            // specified in this field.
            {}                                             // oldSwapChain
        );

        swapChain = device.createSwapchainKHR(createInfo);

        swapChainImages = device.getSwapchainImagesKHR(swapChain);
        swapChainImageFormat = surfaceFormat.format;
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

    std::tuple<vk::SurfaceCapabilitiesKHR, 
             std::vector<vk::SurfaceFormatKHR>,
             std::vector<vk::PresentModeKHR>> 
    querySwapChainSupport(vk::PhysicalDevice device) {
        auto capabilities = device.getSurfaceCapabilitiesKHR(surface);
        auto formats = device.getSurfaceFormatsKHR(surface);
        auto presentModes = device.getSurfacePresentModesKHR(surface);
        return std::tie(capabilities, formats, presentModes);
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
                static_cast<uint32_t>(deviceExtensions.size()),
                deviceExtensions.data(),
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
                static_cast<uint32_t>(deviceExtensions.size()),
                deviceExtensions.data(),
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

        bool haveDeviceExtensions = [&]() -> bool {
            std::set<std::string> requiredExtensions(this->deviceExtensions.begin(), this->deviceExtensions.end());

            auto availableExtensions = device.enumerateDeviceExtensionProperties(nullptr);

            for (const auto& extension: availableExtensions) {
                requiredExtensions.erase(std::string(extension.extensionName));
            }

            return requiredExtensions.empty();
        }();

        bool haveAdequateSwapChainSupport = [&]() -> bool {
            // std::tuple<vk::SurfaceCapabilitiesKHR, 
            //         std::vector<vk::SurfaceFormatKHR>,
            //         std::vector<vk::PresentModeKHR>> 
            auto res = querySwapChainSupport(device);
            return !std::get<1>(res).empty() && !std::get<2>(res).empty();
        }();

        return haveGeometryShader && haveGraphicsQueue 
            && havePresentQueue && haveDeviceExtensions
            && haveAdequateSwapChainSupport;
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
                static_cast<uint32_t>(requiredExtensions.size()),
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
        for (auto imageView: swapChainImageViews) {
            device.destroyImageView(imageView);
        }
        
        device.destroySwapchainKHR(swapChain);
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