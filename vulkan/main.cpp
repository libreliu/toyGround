#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *window;
    vk::Instance instance;

    constexpr static bool enableDebug = true;

    void initVulkan() {
        createInstance();

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
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

        auto layers = vk::enumerateInstanceLayerProperties(nullptr);
        std::cout << "Available layers:" << std::endl;
        for (const auto &layer: layers) {
            std::cout << " - " << layer.layerName << std::endl;
        }

        if constexpr (enableDebug) {
            const std::vector<std::string> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };

            for (auto &name: validationLayers) {
                bool found = false;
                for (const auto& layerProp: layers) {
                    if (name == layerProp.layerName) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    throw std::runtime_error("Validation layers requested, but not available");
                }
            }

            

            vk::InstanceCreateInfo createInfo(
                {},
                &appInfo,
                0,
                nullptr,
                glfwExtensionCount,
                glfwExtensions
            );

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

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
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