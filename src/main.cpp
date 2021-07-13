#include <iostream>
#include <iomanip>
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

    // Check vulkan supported
    if(glfwVulkanSupported() == GLFW_FALSE)
    {
        std::cout << "Vulkan is not supported!\n";
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* pWindow = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

    uint32_t instanceVersion{};
    vkEnumerateInstanceVersion(&instanceVersion);
    std::cout << "Vulkan Version:" << VK_VERSION_MAJOR(instanceVersion) << "." << VK_VERSION_MINOR(instanceVersion) << "." << VK_VERSION_PATCH(instanceVersion) << "\n";

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
        glfwDestroyWindow(pWindow);
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

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, pWindow, nullptr, &surface) != VK_SUCCESS)
    {
        std::cout << "Failed to window surface!\n";
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
        std::cout << "\t" << physicalDeviceProperties.properties.deviceName << " " << physicalDeviceProperties.properties.deviceType <<"\n";

        /*VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        std::cout << "\t" << physicalDeviceProperties.deviceName << "\n";*/

        uint32_t deviceExtensionCount{};
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
        std::cout << "\t\t> Extensions: " << deviceExtensionCount << "\n";
        std::vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensionProperties.data());
        for (auto deviceExtensionProperty : deviceExtensionProperties)
        {
            std::cout << "\t\t\t" << deviceExtensionProperty.extensionName 
                        << " " << VK_VERSION_MAJOR(deviceExtensionProperty.specVersion) 
                        << "." << VK_VERSION_MINOR(deviceExtensionProperty.specVersion) 
                        << "." << VK_VERSION_PATCH(deviceExtensionProperty.specVersion)
                        << "\n";
        }

        uint32_t queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
        std::cout << "\t\t> Queue Family Properties: " << queueFamilyPropertyCount << "\n";
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
        for (size_t i=0; i< queueFamilyProperties.size(); i++)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

            VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
            std::cout << "\t\t\tqueueFamilyProperties[" << i << "]\n";
            std::cout << "\t\t\t------------------------\n";
            std::cout << "\t\t\t" << std::setw(30) << std::left << "queueCount" << std::setw(0) << "= " << queueFamilyProperty.queueCount << "\n";
            std::cout << "\t\t\t" << std::setw(30) << std::left << "queueFlags" << std::setw(0) << "= " << queueFamilyProperty.queueFlags << "\n";
            std::cout << "\t\t\t" << std::setw(30) << std::left << "minImageTransferGranularity" << std::setw(0) << "= (" << queueFamilyProperty.minImageTransferGranularity.width << ", " << queueFamilyProperty.minImageTransferGranularity.height << ", " << queueFamilyProperty.minImageTransferGranularity.depth << ")\n";
            std::cout << "\t\t\t" << std::setw(30) << std::left << "presentSupport" << std::setw(0) << "= " << presentSupport << "\n";
            std::cout << "\n";
        }
        
        VkPhysicalDeviceMemoryBudgetPropertiesEXT physicalDeviceMemoryBudgetPropertiesEXT{};
        physicalDeviceMemoryBudgetPropertiesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

        VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties{};
        physicalDeviceMemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        physicalDeviceMemoryProperties.pNext = &physicalDeviceMemoryBudgetPropertiesEXT;

        vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &physicalDeviceMemoryProperties);
        std::cout << "\t\t> Memory Properties:\n";
        std::cout << "\t\t\tmemoryHeaps: " << physicalDeviceMemoryProperties.memoryProperties.memoryHeapCount << "\n";
        for (size_t i = 0; i < physicalDeviceMemoryProperties.memoryProperties.memoryHeapCount; i++)
        {
            VkMemoryHeap memoryHeap = physicalDeviceMemoryProperties.memoryProperties.memoryHeaps[i];
            std::cout << "\t\t\t\tmemoryHeaps[" << i << "]\n";
            std::cout << "\t\t\t\t--------------\n";
            std::cout << "\t\t\t\t" << std::setw(10) << std::left << "size" << std::setw(0) << "= " << memoryHeap.size << "\n";
            std::cout << "\t\t\t\t" << std::setw(10) << std::left << "budget" << std::setw(0) << "= " << physicalDeviceMemoryBudgetPropertiesEXT.heapBudget[i] << "\n";
            std::cout << "\t\t\t\t" << std::setw(10) << std::left << "usage" << std::setw(0) << "= " << physicalDeviceMemoryBudgetPropertiesEXT.heapUsage[i] << "\n";
            std::cout << "\n";
        }

        std::cout << "\t\t\tmemoryTypes: " << physicalDeviceMemoryProperties.memoryProperties.memoryTypeCount << "\n";
        for (size_t i = 0; i < physicalDeviceMemoryProperties.memoryProperties.memoryTypeCount; i++)
        {
            VkMemoryType memoryType = physicalDeviceMemoryProperties.memoryProperties.memoryTypes[i];
            std::cout << "\t\t\t\tmemoryTypes[" << i << "]\n";
            std::cout << "\t\t\t\t--------------\n";
            std::cout << "\t\t\t\t" << std::setw(15) << std::left << "heapIndex" << std::setw(0) << "= " << memoryType.heapIndex << "\n";
            std::cout << "\t\t\t\t" << std::setw(15) << std::left << "propertyFlags" << std::setw(0) << "= " << memoryType.propertyFlags << "\n";
            std::cout << "\n";
        }
        
        //VkPhysicalDeviceFeatures physicalDeviceFeatures;
        //vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
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
        std::cout << "\t\t> Layer Extensions: " << instanceExtensionProperties.size() << "\n";
        for (auto instanceExtensionProperty : instanceExtensionProperties)
        {
            std::cout << "\t\t\t" << instanceExtensionProperty.extensionName 
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
            std::cout << "\t\t> Device (" << physicalDeviceProperties.deviceName << ") Extensions: " << deviceExtensionProperties.size() << "\n";
            for (auto deviceExtensionProperty : deviceExtensionProperties)
            {
                std::cout << "\t\t\t" << deviceExtensionProperty.extensionName 
                            << " " << VK_VERSION_MAJOR(deviceExtensionProperty.specVersion) 
                            << "." << VK_VERSION_MINOR(deviceExtensionProperty.specVersion) 
                            << "." << VK_VERSION_PATCH(deviceExtensionProperty.specVersion)
                            << "\n";
            }
        }
    }

    // Choose physcal device
    VkPhysicalDevice choosePhysicalDevice = nullptr;
    VkPhysicalDeviceType physicalDeviceTypePriorities[] = 
    {
        VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
        VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
    };
    for (auto physicalDeviceTypePriority : physicalDeviceTypePriorities)
    {
        for (auto physicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            if (physicalDeviceProperties.deviceType == physicalDeviceTypePriority)
            {
                choosePhysicalDevice = physicalDevice;
                std::cout << "Choose Physical Device: " << physicalDeviceProperties.deviceName << "\n";
                break;
            }
        }

        if (choosePhysicalDevice != nullptr) break;
    }

    if (choosePhysicalDevice == nullptr)
    {
        std::cout << std::endl;
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(pWindow);
        glfwTerminate();
        return EXIT_SUCCESS;
    }

    // Choose queue
    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(choosePhysicalDevice, &queueFamilyPropertyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(choosePhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

    uint32_t graphicsQueueFamilyIndex{};
    for (size_t i=0; i<queueFamilyProperties.size(); i++)
    {
        VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }
    uint32_t presentQueueFamilyIndex{};
    for (size_t i=0; i<queueFamilyProperties.size(); i++)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(choosePhysicalDevice, i, surface, &presentSupport);
        if (presentSupport)
        {
            presentQueueFamilyIndex = i;
            break;
        }
    }

    float queuePriorities[1] = { 0.0 };
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(2);
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

        deviceQueueCreateInfos[0] = deviceQueueCreateInfo;
    }
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

        deviceQueueCreateInfos[1] = deviceQueueCreateInfo;
    }

    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkDevice device;
    if (vkCreateDevice(choosePhysicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
    {
        std::cout << "Failed to create logical device!\n";
    }

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    VkQueue presentQueue;
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
    
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
    debugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfoEXT.pfnUserCallback = debugUtilsMessengerCallback;
    debugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

    auto pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerEXT debugUtilsMessengerEXT;
    pfnCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfoEXT, nullptr, &debugUtilsMessengerEXT);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        std::cout << "Failed to create command pool!\n";
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    std::vector<VkCommandBuffer> commandBuffers(1);
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate command buffers!\n";
    }
    
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffers[0], &commandBufferBeginInfo);
    vkEndCommandBuffer(commandBuffers[0]);

    //VkSubmitInfo submitInfo{};
    //submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    //submitInfo.pCommandBuffers = commandBuffers.data();
    //vkQueueSubmit(graphicsQueue, 1, &submitInfo, ?);

    uint32_t surfaceFormatCount{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(choosePhysicalDevice, surface, &surfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(choosePhysicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());
    //VK_FORMAT_B8G8R8A8_UNORM
    //VK_COLOR_SPACE_SRGB_NONLINEAR_KHR

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(choosePhysicalDevice, surface, &surfaceCapabilities);
    //VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }
    VkExtent2D extent = surfaceCapabilities.currentExtent;
    if (extent.width == UINT32_MAX || extent.height == UINT32_MAX)
    {
        int width{};
        int height{};
        glfwGetFramebufferSize(pWindow, &width, &height);
        extent.width = std::clamp(static_cast<uint32_t>(width), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        extent.height = std::clamp(static_cast<uint32_t>(height), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(choosePhysicalDevice, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(choosePhysicalDevice, surface, &presentModeCount, presentModes.data());
    //VK_PRESENT_MODE_FIFO_KHR

    VkSwapchainCreateInfoKHR swapchainCreateInfoKHR{};
    swapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfoKHR.surface = surface;
    swapchainCreateInfoKHR.minImageCount = imageCount;
    swapchainCreateInfoKHR.imageExtent = extent;
    swapchainCreateInfoKHR.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainCreateInfoKHR.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfoKHR.imageArrayLayers = 1; // For non-stereoscopic-3D applications, this value is 1
    swapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfoKHR.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfoKHR.clipped = VK_TRUE;

    // VK_SHARING_MODE_CONCURRENT may result in lower performance access to the buffer or image than VK_SHARING_MODE_EXCLUSIVE.
    if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
    {
        swapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        swapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfoKHR.queueFamilyIndexCount = 2;
        uint32_t queueFamilyIndices[] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
        swapchainCreateInfoKHR.pQueueFamilyIndices = queueFamilyIndices;
    }

    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        swapchainCreateInfoKHR.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else 
    {
        swapchainCreateInfoKHR.preTransform = surfaceCapabilities.currentTransform;
    }
    
    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &swapchainCreateInfoKHR, nullptr, &swapchain) != VK_SUCCESS)
    {
        std::cout << "Failed to create swapchain!\n";
    }

    uint32_t swapchainImageCount{};
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    std::vector<VkImage> swapchainImages(swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());

    std::vector<VkImageView> swapchainImageViews(swapchainImageCount);
    for (size_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            std::cout << "Failed to create image view!\n";
        }
    }

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(choosePhysicalDevice, VK_FORMAT_D16_UNORM, &formatProperties);

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_D16_UNORM;
    imageCreateInfo.extent.width = extent.width;
    imageCreateInfo.extent.height = extent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    } else 
    {
        std::cout << "Unsupported depth format, try other depth formats!\n";
    }

    VkImage depthImage;
    if (vkCreateImage(device, &imageCreateInfo, nullptr, &depthImage) != VK_SUCCESS)
    {
        std::cout << "Failed to create depth image!\n";
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, depthImage, &memoryRequirements);

    std::cout << std::endl;
    while(!glfwWindowShouldClose(pWindow)) 
    {
        glfwPollEvents();
    }

    vkDestroyImage(device, depthImage, nullptr);

    for (auto swapchainImageView : swapchainImageViews) 
    {
        vkDestroyImageView(device, swapchainImageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapchain, nullptr);

    vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    auto pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    pfnDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessengerEXT, nullptr);

    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
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