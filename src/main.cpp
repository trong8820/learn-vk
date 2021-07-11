#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
                        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                        void* pUserData);


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* pWindow = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

    uint32_t instanceExtensionCount{};
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::cout << "Instance Extensions: " << instanceExtensionCount << "\n";
    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionProperties.data());
    for (auto instanceExtensionProperty : instanceExtensionProperties)
    {
        std::cout << "\t" << instanceExtensionProperty.extensionName 
                    << " " << VK_VERSION_MAJOR(instanceExtensionProperty.specVersion) 
                    << "." << VK_VERSION_MINOR(instanceExtensionProperty.specVersion) 
                    << "." << VK_VERSION_PATCH(instanceExtensionProperty.specVersion)
                    << "\n";
    }

    uint32_t glfwRequiredExtensionCount{};
    const char** glfwRequiredExtensions;
    glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
    std::vector<const char*> requiredExensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);
    bool isNotFoundDebugExtension = true;
    for (auto requiredExension : requiredExensions)
    {
        if (std::strcmp(requiredExension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
        {
            isNotFoundDebugExtension = false;
            break;
        }
    }
    if (isNotFoundDebugExtension)
    {
        requiredExensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "Required Instance Extensions: " << requiredExensions.size() << "\n";
    std::vector<const char*> notFoundRequiredExensions;
    for (auto requiredExension : requiredExensions)
    {
        std::cout << "\t" << requiredExension << "\n";
        bool isNotFoundInstanceExtension = true;
        for (auto instanceExtensionProperty : instanceExtensionProperties)
        {
            if (std::strcmp(instanceExtensionProperty.extensionName, requiredExension) == 0)
            {
                isNotFoundInstanceExtension = false;
                break;
            }
        }
        if (isNotFoundInstanceExtension)
        {
            notFoundRequiredExensions.push_back(requiredExension);
        }
    }

    if (notFoundRequiredExensions.size() > 0)
    {
        std::cout << "Not found extensions: "  << notFoundRequiredExensions.size() << "\n";
        for (auto notFoundRequiredExension : notFoundRequiredExensions)
        {
            std::cout << "\t" << notFoundRequiredExension << "\n";
        }
        
        std::cout << std::endl;
        glfwTerminate();
        return EXIT_SUCCESS;
    }

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Learn Vulkan";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExensions.data();

    VkInstance instance;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
    {
        std::cout << "Failed to create instance!\n";
    }
    
    uint32_t physicalDeviceCount{};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::cout << "Physical Devices: " << physicalDeviceCount << "\n";

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    for (auto physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceVulkan11Properties physicalDeviceVulkan11Properties{};
        physicalDeviceVulkan11Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;

        VkPhysicalDeviceVulkan12Properties physicalDeviceVulkan12Properties{};
        physicalDeviceVulkan12Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        physicalDeviceVulkan12Properties.pNext = &physicalDeviceVulkan11Properties;

        VkPhysicalDeviceDriverProperties physicalDeviceDriverProperties{};
		physicalDeviceDriverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        physicalDeviceDriverProperties.pNext = &physicalDeviceVulkan12Properties;

        VkPhysicalDeviceProperties2 physicalDeviceProperties;
        physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        physicalDeviceProperties.pNext = &physicalDeviceDriverProperties;
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties);
        std::cout << "\t" << physicalDeviceProperties.properties.deviceName << "\n";

        /*VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        std::cout << "\t" << physicalDeviceProperties.deviceName << "\n";*/
    }

    uint32_t instanceLayerCount{};
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

    std::cout << "Instance Layers: " << instanceLayerCount << "\n";

    std::vector<VkLayerProperties> layerProperties(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());

    for (auto layerProperty : layerProperties)
    {
        std::cout << "\t" << layerProperty.layerName << " " << layerProperty.description << std::endl;

        uint32_t instanceExtensionCount{};
        vkEnumerateInstanceExtensionProperties(layerProperty.layerName, &instanceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionCount);
        vkEnumerateInstanceExtensionProperties(layerProperty.layerName, &instanceExtensionCount, instanceExtensionProperties.data());
        for (auto instanceExtensionProperty : instanceExtensionProperties)
        {
            std::cout << "\t\t-> Instance Ext: " << instanceExtensionProperty.extensionName 
                        << " " << VK_VERSION_MAJOR(instanceExtensionProperty.specVersion) 
                        << "." << VK_VERSION_MINOR(instanceExtensionProperty.specVersion) 
                        << "." << VK_VERSION_PATCH(instanceExtensionProperty.specVersion)
                        << "\n";
        }

        for (auto physicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            uint32_t deviceExtensionCount{};
            vkEnumerateDeviceExtensionProperties(physicalDevice, layerProperty.layerName, &deviceExtensionCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, layerProperty.layerName, &deviceExtensionCount, deviceExtensionProperties.data());
            for (auto deviceExtensionProperty : deviceExtensionProperties)
            {
                std::cout << "\t\t-> Device (" << physicalDeviceProperties.deviceName << ") Ext: " 
                            << deviceExtensionProperty.extensionName 
                            << " " << VK_VERSION_MAJOR(deviceExtensionProperty.specVersion) 
                            << "." << VK_VERSION_MINOR(deviceExtensionProperty.specVersion) 
                            << "." << VK_VERSION_PATCH(deviceExtensionProperty.specVersion)
                            << "\n";
            }
        }
    }

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
    debugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.pfnUserCallback = debugUtilsMessengerCallback;
    debugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

    auto pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerEXT debugUtilsMessengerEXT;
    pfnCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfoEXT, nullptr, &debugUtilsMessengerEXT);

    std::cout << std::endl;
    while(!glfwWindowShouldClose(pWindow)) 
    {
        glfwPollEvents();
    }

    auto pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    pfnDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessengerEXT, nullptr);

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(pWindow);
    glfwTerminate();
    return EXIT_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
                        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                        void* pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}