#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* pWindow = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

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

    VkInstance instance;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
    {
        std::cout << "Failed to create instance!\n";
    }

    uint32_t physicalDeviceCount{};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::cout << "Physical Device: " << physicalDeviceCount << "\n";

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

    std::cout << instanceLayerCount << std::endl;

    std::vector<VkLayerProperties> layerProperties(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());

    for (auto layerProperty : layerProperties)
    {
        std::cout << layerProperty.layerName << " " << layerProperty.description << std::endl;
    }

    std::cout << std::endl;
    while(!glfwWindowShouldClose(pWindow)) 
    {
        glfwPollEvents();
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(pWindow);
    glfwTerminate();
    return EXIT_SUCCESS;
}