/**
 * 								 MIT License
 *
 *						  Copyright (c) 2023 kbz_8
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 *
 *
 *	Do this:
 *	#define KVF_IMPLEMENTATION
 *	before you include this file in *one* C or C++ file to create the implementation.
 *
 *	// i.e. it should look like this:
 *	#include ...
 *	#include ...
 *	#include ...
 *	#define KVF_IMPLEMENTATION
 *	#include "kvf.h"
 *	
 *	You can #define KVF_ASSERT(x) before the #include to avoid using assert.h.
 *	And #define KVF_MALLOC, KVF_REALLOC, and KVF_FREE to avoid using malloc, realloc, free.
 *
 *	If you are using Volk or any other meta loader you must define KVF_IMPL_VK_NO_PROTOTYPES
 *	or VK_NO_PROTOTYPES before including this file to avoid conflicts with Vulkan prototypes.
 *
 *	You can also #define KVF_ENABLE_VALIDATION_LAYERS to enable validation layers.
 */

#ifndef KBZ_8_VULKAN_FRAMEWORK_H
#define KBZ_8_VULKAN_FRAMEWORK_H

#ifdef KVF_IMPL_VK_NO_PROTOTYPES
	#define VK_NO_PROTOTYPES
#endif // KVF_IMPL_VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>

#include <stdint.h>
#include <stdbool.h>

/* ============================================= Prototypes ============================================= */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	KVF_GRAPHICS_QUEUE = 0,
	KVF_PRESENT_QUEUE = 1
} KvfQueueType;

VkInstance kvfCreateInstance(const char** extensionsEnabled, uint32_t extensionsCount);
void kvfDestroyInstance(VkInstance instance);

VkPhysicalDevice kvfPickFirstPhysicalDevice(VkInstance instance);
VkPhysicalDevice kvfPickGoodDefaultPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
VkPhysicalDevice kvfPickGoodPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const char** deviceExtensions, uint32_t deviceExtensionsCount);

void kvfFindQueueFamilies(VkPhysicalDevice physical, VkSurfaceKHR surface);
VkQueue kvfGetDeviceQueue(VkDevice device, KvfQueueType queue);

VkDevice kvfCreateDefaultDevice(VkPhysicalDevice physical);
VkDevice kvfCreateDevice(VkPhysicalDevice physical, const char** extensions, uint32_t extensions_count);
void kvfDestroyDevice(VkDevice device);

VkFence kvfCreateFence(VkDevice device);
void kvfDestroyFence(VkDevice device, VkFence fence);

VkSemaphore kvfCreateSemaphore(VkDevice device);
void kvfDestroySemaphore(VkDevice device, VkSemaphore semaphore);

VkSwapchainKHR kvfCreateSwapchainKHR(VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface, VkExtent2D extent, bool tryVsync);
void kvfDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain);

VkImageView* kvfCreateSwapChainImageViewsKHR(VkDevice device, VkSwapchainKHR swapChain, size_t* size);
void kvfDestroySwapChainImageViewsKHR(VkDevice device, VkImageView* imageView, size_t size);

VkFramebuffer kvfCreateFrameBuffer(VkDevice device, VkRenderPass renderPass, VkImageView swapChainImageView, VkExtent2D extent);
void kvfDestroyFrameBuffer(VkDevice device, VkFramebuffer frameBuffer);

VkCommandBuffer kvfCreateCommandBuffer(VkDevice device);
VkCommandBuffer kvfCreateCommandBufferLeveled(VkDevice device, VkCommandBufferLevel level);

VkRenderPass kvfCreateRenderPass(VkDevice device);
void kvfDestroyRenderPass(VkDevice device, VkRenderPass renderPass);

VkShaderModule kvfCreateShaderModule(VkDevice device, uint32_t* code, size_t size);
void kvfDestroyShaderModule(VkDevice device, VkShaderModule shader);

const char* kvfVerbaliseResultVk(VkResult result);

#ifdef __cplusplus
}
#endif

/* ========================================== Implementation =========================================== */

#ifdef KVF_IMPLEMENTATION

#ifndef KVF_MALLOC
	#define KVF_MALLOC(x) malloc(x)
#endif
#ifndef KVF_REALLOC
	#define KVF_REALLOC(x, s) realloc(x, s)
#endif
#ifndef KVF_FREE
	#define KVF_FREE(x) free(x)
#endif
#ifndef KVF_ASSERT
	#include <assert.h>
	#define KVF_ASSERT(x) assert(x)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	VkDevice device;
	VkCommandPool pool;
} __KvfCommandPool;

typedef struct
{
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR* formats;
	VkPresentModeKHR* presentModes;
	uint32_t formatsCount;
	uint32_t presentModesCount;
} __KvfSwapchainSupportInternal;


// Dynamic array
__KvfCommandPool* __kvf_internal_command_pools = NULL;
size_t __kvf_internal_command_pools_size = 0;
size_t __kvf_internal_command_pools_capacity = 0;

int32_t __kvf_graphics_queue_family = -1;
int32_t __kvf_present_queue_family = -1;

VkFormat __kvf_internal_swap_chain_image_format = VK_FORMAT_UNDEFINED;
__KvfSwapchainSupportInternal __kvf_internal_swap_chain_support = {};

#ifdef KVF_ENABLE_VALIDATION_LAYERS
	VkDebugUtilsMessengerEXT __kvf_debug_messenger;
#endif

VkCommandPool __kvfAddCommandPoolToArray(VkDevice device, VkCommandPool pool)
{
	KVF_ASSERT(device != NULL);
	KVF_ASSERT(pool != NULL);
	if(__kvf_internal_command_pools_size == __kvf_internal_command_pools_capacity)
	{
        // Resize the dynamic array if necessary
		__kvf_internal_command_pools_capacity += 2;
		__kvf_internal_command_pools = (__KvfCommandPool*)KVF_REALLOC(__kvf_internal_command_pools, __kvf_internal_command_pools_capacity * sizeof(__KvfCommandPool));
	}
	__kvf_internal_command_pools[__kvf_internal_command_pools_size].device = device;
	__kvf_internal_command_pools[__kvf_internal_command_pools_size].pool = pool;
	__kvf_internal_command_pools_size++;
	return pool;
}

void __kvfRemoveCommandPoolFromArray(VkDevice device)
{
	KVF_ASSERT(device != NULL);
	for(size_t i = 0; i < __kvf_internal_command_pools_size; i++)
	{
		if(__kvf_internal_command_pools[i].device == device)
		{
			vkDestroyCommandPool(device, __kvf_internal_command_pools[i].pool, NULL);
			// Shift the elements to fill the gap
			for(size_t j = i; j < __kvf_internal_command_pools_size - 1; j++)
				__kvf_internal_command_pools[j] = __kvf_internal_command_pools[j + 1];
			__kvf_internal_command_pools_size--;
			return;
		}
    }
}

VkCommandPool __kvfGetCommandPoolForDevice(VkDevice device)
{
	KVF_ASSERT(device != NULL);
	for(size_t i = 0; i < __kvf_internal_command_pools_size; i++)
	{
		if(__kvf_internal_command_pools[i].device == device)
			return __kvf_internal_command_pools[i].pool;
	}
	return VK_NULL_HANDLE;
}

const char* kvfVerbaliseResultVk(VkResult result)
{
	switch(result)
	{
		case VK_SUCCESS: return "Success";
		case VK_NOT_READY: return "A fence or query has not yet completed";
		case VK_TIMEOUT: return "A wait operation has not completed in the specified time";
		case VK_EVENT_SET: return "An event is signaled";
		case VK_EVENT_RESET: return "An event is unsignaled";
		case VK_INCOMPLETE: return "A return array was too small for the result";
		case VK_ERROR_OUT_OF_HOST_MEMORY: return "A host memory allocation has failed";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "A device memory allocation has failed";
		case VK_ERROR_INITIALIZATION_FAILED: return "Initialization of an object could not be completed for implementation-specific reasons";
		case VK_ERROR_DEVICE_LOST: return "The logical or physical device has been lost";
		case VK_ERROR_MEMORY_MAP_FAILED: return "Mapping of a memory object has failed";
		case VK_ERROR_LAYER_NOT_PRESENT: return "A requested layer is not present or could not be loaded";
		case VK_ERROR_EXTENSION_NOT_PRESENT: return "A requested extension is not supported";
		case VK_ERROR_FEATURE_NOT_PRESENT: return "A requested feature is not supported";
		case VK_ERROR_INCOMPATIBLE_DRIVER: return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
		case VK_ERROR_TOO_MANY_OBJECTS: return "To``````o many objects of the type have already been created";
		case VK_ERROR_FORMAT_NOT_SUPPORTED: return "A requested format is not supported on this device";
		case VK_ERROR_SURFACE_LOST_KHR: return "A surface is no longer available";
		case VK_SUBOPTIMAL_KHR: return "A swapchain no longer matches the surface properties exactly, but can still be used";
		case VK_ERROR_OUT_OF_DATE_KHR: return "A surface has changed in such a way that it is no longer compatible with the swapchain";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "The display used by a swapchain does not use the same presentable image layout";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
		case VK_ERROR_VALIDATION_FAILED_EXT: return "A validation layer found an error";

		default: return "Unknown Vulkan error";
	}
	return NULL; // just to avoid warnings
}

void __kvfCheckVk(VkResult result, const char* function)
{
	if(result != VK_SUCCESS)
		printf("KVF Vulkan error in '%s': %s\n", function, kvfVerbaliseResultVk(result));
}

#undef __kvfCheckVk
#define __kvfCheckVk(res) __kvfCheckVk(res, __FUNCTION__)

#ifdef KVF_ENABLE_VALIDATION_LAYERS
	bool __kvfCheckValidationLayerSupport()
	{
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, NULL);
		VkLayerProperties* available_layers = (VkLayerProperties*)KVF_MALLOC(sizeof(VkLayerProperties) * layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

		for(int i = 0; i < layer_count; i++)
		{
			if(strcmp(available_layers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
				return true;
		}
		return false;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL __kvfDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			printf("\nKVF Vulkan validation layer error : %s\n", pCallbackData->pMessage);
		else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			printf("\nKVF Vulkan validation layer warning : %s\n", pCallbackData->pMessage);
		return VK_FALSE;
	}

	void __kvfPopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
	{
		createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo->pfnUserCallback = __kvfDebugCallback;
	}
#endif // KVF_ENABLE_VALIDATION_LAYERS

VkInstance kvfCreateInstance(const char** extensionsEnabled, uint32_t extensionsCount)
{
	VkInstance vk_instance = VK_NULL_HANDLE;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = NULL;
	createInfo.flags = 0;
	createInfo.enabledExtensionCount = extensionsCount;
	createInfo.ppEnabledExtensionNames = extensionsEnabled;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = NULL;
	createInfo.pNext = NULL;

#ifdef KVF_ENABLE_VALIDATION_LAYERS
	const char** new_extension_set = NULL;
	if(__kvfCheckValidationLayerSupport())
	{
		const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		__kvfPopulateDebugMessengerCreateInfo(&debugCreateInfo);
		new_extension_set = (const char**)KVF_MALLOC(sizeof(char*) * (extensionsCount + 1));
		memcpy(new_extension_set, extensionsEnabled, sizeof(char*) * extensionsCount);
		new_extension_set[extensionsCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

		createInfo.enabledExtensionCount = extensionsCount + 1;
		createInfo.ppEnabledExtensionNames = new_extension_set;
		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = layers;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
#endif

	__kvfCheckVk(vkCreateInstance(&createInfo, NULL, &vk_instance));
#ifdef KVF_ENABLE_VALIDATION_LAYERS
	KVF_FREE(new_extension_set);
#endif
	return vk_instance;
}

void kvfDestroyInstance(VkInstance instance)
{
	if(instance == VK_NULL_HANDLE)
		return;
#ifdef KVF_ENABLE_VALIDATION_LAYERS
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func)
		func(instance, __kvf_debug_messenger, NULL);
#endif
	vkDestroyInstance(instance, NULL);
}

VkPhysicalDevice kvfPickFirstPhysicalDevice(VkInstance instance)
{
	uint32_t device_count;
	VkPhysicalDevice* devices = NULL;
	VkPhysicalDevice chosen_one = VK_NULL_HANDLE;

	KVF_ASSERT(instance != VK_NULL_HANDLE);
	
	vkEnumeratePhysicalDevices(instance, &device_count, NULL);
	devices = (VkPhysicalDevice*)KVF_MALLOC(sizeof(VkPhysicalDevice) * device_count + 1);
	vkEnumeratePhysicalDevices(instance, &device_count, devices);
	chosen_one = devices[0];
	KVF_FREE(devices);
	return chosen_one;
}

void kvfFindQueueFamilies(VkPhysicalDevice physical, VkSurfaceKHR surface)
{
	if(__kvf_graphics_queue_family != -1 && __kvf_present_queue_family != -1)
		return;
	uint32_t queue_family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &queue_family_count, NULL);
	VkQueueFamilyProperties* queue_families = (VkQueueFamilyProperties*)KVF_MALLOC(sizeof(VkQueueFamilyProperties) * queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &queue_family_count, queue_families);
	
	for(int i = 0; i < queue_family_count; i++)
	{
		if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			__kvf_graphics_queue_family = i;
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &present_support);
		if(present_support)
			__kvf_present_queue_family = i;

		if(__kvf_graphics_queue_family != -1 && __kvf_present_queue_family != -1)
			break;
	}
	KVF_FREE(queue_families);
}

VkPhysicalDevice kvfPickGoodDefaultPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	return kvfPickGoodPhysicalDevice(instance, surface, extensions, sizeof(extensions) / sizeof(extensions[0]));
}

int32_t __kvfScorePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, const char** deviceExtensions, uint32_t deviceExtensionsCount)
{
	/* Check Extensions Support */
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
	VkExtensionProperties* props = (VkExtensionProperties*)KVF_MALLOC(sizeof(VkExtensionProperties) * extension_count + 1);
	vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, props);

	bool are_there_required_device_extensions = true;
	for(int j = 0; j < deviceExtensionsCount; j++)
	{
		bool is_there_extension = false;
		for(int k = 0; k < extension_count; k++)
		{
			if(strcmp(deviceExtensions[j], props[k].extensionName) == 0)
			{
				is_there_extension = true;
				break;
			}
		}
		if(is_there_extension == false)
		{
			are_there_required_device_extensions = false;
			break;
		}
	}
	KVF_FREE(props);
	if(are_there_required_device_extensions == false)
		return -1;

	/* Check Queue Families Support */
	kvfFindQueueFamilies(device, surface);
	if(__kvf_graphics_queue_family == -1 || __kvf_present_queue_family == -1)
		return -1;

	/* Check Surface Formats Counts */
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
	if(format_count == 0)
		return -1;

	VkPhysicalDeviceProperties device_props;
	vkGetPhysicalDeviceProperties(device, &device_props);

	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures(device, &device_features);

	int32_t score = -1;
	if(device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	if(!device_features.geometryShader)
		return -1;

	score += device_props.limits.maxImageDimension2D;
	score += device_props.limits.maxBoundDescriptorSets;

	return score;
}

VkPhysicalDevice kvfPickGoodPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const char** deviceExtensions, uint32_t deviceExtensionsCount)
{
	VkPhysicalDevice* devices = NULL;
	VkPhysicalDevice chosen_one = VK_NULL_HANDLE;
	uint32_t device_count;
	int32_t best_device_score = -1;

	KVF_ASSERT(instance != VK_NULL_HANDLE);
	KVF_ASSERT(surface != VK_NULL_HANDLE);
	
	vkEnumeratePhysicalDevices(instance, &device_count, NULL);
	devices = (VkPhysicalDevice*)KVF_MALLOC(sizeof(VkPhysicalDevice) * device_count + 1);
	vkEnumeratePhysicalDevices(instance, &device_count, devices);

	for(int i = 0; i < device_count; i++)
	{
		__kvf_graphics_queue_family = -1;
		__kvf_present_queue_family = -1;
		int32_t current_device_score = __kvfScorePhysicalDevice(devices[i], surface, deviceExtensions, deviceExtensionsCount);
		if(current_device_score > best_device_score)
		{
			best_device_score = current_device_score;
			chosen_one = devices[i];
		}
	}
	KVF_FREE(devices);
	if(chosen_one != VK_NULL_HANDLE)
	{
		kvfFindQueueFamilies(chosen_one, surface);
		return chosen_one;
	}
	return VK_NULL_HANDLE;
}

VkDevice kvfCreateDefaultDevice(VkPhysicalDevice physical)
{
	const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	return kvfCreateDevice(physical, extensions, sizeof(extensions) / sizeof(extensions[0]));
}

VkDevice kvfCreateDevice(VkPhysicalDevice physical, const char** extensions, uint32_t extensions_count)
{
	const float queue_priority = 1.0f;

	KVF_ASSERT(__kvf_graphics_queue_family != -1);
	KVF_ASSERT(__kvf_present_queue_family != -1);

	VkDeviceQueueCreateInfo queue_create_info[2];
	queue_create_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info[0].queueFamilyIndex = __kvf_graphics_queue_family;
	queue_create_info[0].queueCount = 1;
	queue_create_info[0].pQueuePriorities = &queue_priority;
	queue_create_info[0].flags = 0;
	queue_create_info[0].pNext = NULL;
	queue_create_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info[1].queueFamilyIndex = __kvf_present_queue_family;
	queue_create_info[1].queueCount = 1;
	queue_create_info[1].pQueuePriorities = &queue_priority;
	queue_create_info[1].flags = 0;
	queue_create_info[1].pNext = NULL;

	VkPhysicalDeviceFeatures device_features = { VK_FALSE };

	VkDeviceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = (__kvf_graphics_queue_family == __kvf_present_queue_family ? 1 : 2);
	createInfo.pQueueCreateInfos = queue_create_info;
	createInfo.pEnabledFeatures = &device_features;
	createInfo.enabledExtensionCount = extensions_count;
	createInfo.ppEnabledExtensionNames = extensions;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = NULL;
	createInfo.flags = 0;
	createInfo.pNext = NULL;

	VkDevice device = VK_NULL_HANDLE;
	__kvfCheckVk(vkCreateDevice(physical, &createInfo, NULL, &device));
	return device;
}

void kvfDestroyDevice(VkDevice device)
{
	if(device == VK_NULL_HANDLE)
		return;
	if(__kvf_internal_command_pools != NULL && __kvf_internal_command_pools_size != 0)
		__kvfRemoveCommandPoolFromArray(device);
	vkDestroyDevice(device, NULL);
}

VkQueue kvfGetDeviceQueue(VkDevice device, KvfQueueType queue)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	KVF_ASSERT(__kvf_graphics_queue_family != -1);
	KVF_ASSERT(__kvf_present_queue_family != -1);

	VkQueue vk_queue = VK_NULL_HANDLE;

	if(queue == KVF_GRAPHICS_QUEUE)
		vkGetDeviceQueue(device, __kvf_graphics_queue_family, 0, &vk_queue);
	else if(queue == KVF_GRAPHICS_QUEUE)
		vkGetDeviceQueue(device, __kvf_present_queue_family, 0, &vk_queue);
	return vk_queue;
}

VkFence kvfCreateFence(VkDevice device)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	VkFence fence = VK_NULL_HANDLE;
	__kvfCheckVk(vkCreateFence(device, &fenceInfo, NULL, &fence));
	return fence;
}

void kvfDestroyFence(VkDevice device, VkFence fence)
{
	if(fence == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroyFence(device, fence, NULL);
}

VkSemaphore kvfCreateSemaphore(VkDevice device)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkSemaphore semaphore = VK_NULL_HANDLE;
	__kvfCheckVk(vkCreateSemaphore(device, &semaphoreInfo, NULL, &semaphore));
	return semaphore;
}

void kvfDestroySemaphore(VkDevice device, VkSemaphore semaphore)
{
	if(semaphore == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroySemaphore(device, semaphore, NULL);
}

__KvfSwapchainSupportInternal __kvfQuerySwapchainSupport(VkPhysicalDevice physical, VkSurfaceKHR surface)
{
	__KvfSwapchainSupportInternal support;

	__kvfCheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &support.capabilities));

	vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &support.formatsCount, NULL);
	if(support.formatsCount != 0)
	{
		support.formats = (VkSurfaceFormatKHR*)KVF_MALLOC(sizeof(VkSurfaceFormatKHR) * support.formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &support.formatsCount, support.formats);
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &support.presentModesCount, NULL);
	if(support.presentModesCount != 0)
	{
		support.presentModes = (VkPresentModeKHR*)KVF_MALLOC(sizeof(VkPresentModeKHR) * support.presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &support.presentModesCount, support.presentModes);
	}
	__kvf_internal_swap_chain_support = support;
	return support;
}

VkSurfaceFormatKHR __kvfChooseSwapSurfaceFormat(__KvfSwapchainSupportInternal* support)
{
	for(int i = 0; i < support->formatsCount; i++)
	{
		if(support->formats[i].format == VK_FORMAT_R8G8B8A8_SRGB && support->formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return support->formats[i];
	}
	return support->formats[0];
}

VkPresentModeKHR __kvfChooseSwapPresentMode(__KvfSwapchainSupportInternal* support, bool tryVsync)
{
	if(tryVsync == false)
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	for(int i = 0; i < support->presentModesCount; i++)
	{
		if(support->presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return support->presentModes[i];
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t __kvfClamp(uint32_t i, uint32_t min, uint32_t max)
{
	const uint32_t t = i < min ? min : i;
	return t > max ? max : t;
}

VkSwapchainKHR kvfCreateSwapchainKHR(VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface, VkExtent2D extent, bool tryVsync)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	__KvfSwapchainSupportInternal support = __kvfQuerySwapchainSupport(physical, surface);

	VkSurfaceFormatKHR surfaceFormat = __kvfChooseSwapSurfaceFormat(&support);
	VkPresentModeKHR presentMode = __kvfChooseSwapPresentMode(&support, tryVsync);

	uint32_t imageCount = support.capabilities.minImageCount + 1;
	if(support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
		imageCount = support.capabilities.maxImageCount;

	uint32_t queueFamilyIndices[] = { (uint32_t)__kvf_graphics_queue_family, (uint32_t)__kvf_present_queue_family };

	if(support.capabilities.currentExtent.width != UINT32_MAX)
		extent = support.capabilities.currentExtent;
	else
	{
		extent.width = __kvfClamp(extent.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
		extent.height = __kvfClamp(extent.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if(__kvf_graphics_queue_family != __kvf_present_queue_family)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	__kvf_internal_swap_chain_image_format = surfaceFormat.format;

	__kvfCheckVk(vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain));
	return swapchain;
}

void kvfDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain)
{
	if(swapchain == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroySwapchainKHR(device, swapchain, NULL);
}

VkImageView* kvfCreateSwapChainImageViewsKHR(VkDevice device, VkSwapchainKHR swapChain, size_t* size)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkGetSwapchainImagesKHR(device, swapChain, (uint32_t*)size, NULL);
	VkImage* images = (VkImage*)KVF_MALLOC(sizeof(VkImage) * (*size));
	VkImageView* views = (VkImageView*)KVF_MALLOC(sizeof(VkImageView) * (*size));
	vkGetSwapchainImagesKHR(device, swapChain, (uint32_t*)size, images);

	for(int i = 0; i < *size; i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = __kvf_internal_swap_chain_image_format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		__kvfCheckVk(vkCreateImageView(device, &createInfo, NULL, &views[i]));
	}
	KVF_FREE(images);
	return views;
}

void kvfDestroySwapChainImageViewsKHR(VkDevice device, VkImageView* imageView, size_t size)
{
	if(imageView == NULL)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	for(int i = 0; i < size; i++)
		vkDestroyImageView(device, imageView[i], NULL);
}

VkFramebuffer kvfCreateFrameBuffer(VkDevice device, VkRenderPass renderPass, VkImageView swapChainImageView, VkExtent2D extent)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;
	VkImageView attachments[] = { swapChainImageView };

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	__kvfCheckVk(vkCreateFramebuffer(device, &framebufferInfo, NULL, &frameBuffer));
	return frameBuffer;
}

void kvfDestroyFrameBuffer(VkDevice device, VkFramebuffer frameBuffer)
{
	if(frameBuffer == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroyFramebuffer(device, frameBuffer, NULL);
}

VkCommandPool __kvfCreateCommandPoolForDevice(VkDevice device)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkCommandPool pool;
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = __kvf_graphics_queue_family;
	__kvfCheckVk(vkCreateCommandPool(device, &pool_info, NULL, &pool));
	__kvfAddCommandPoolToArray(device, pool);
	return pool;
}

VkCommandBuffer kvfCreateCommandBuffer(VkDevice device)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	return kvfCreateCommandBufferLeveled(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

VkCommandBuffer kvfCreateCommandBufferLeveled(VkDevice device, VkCommandBufferLevel level)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkCommandPool pool = __kvfGetCommandPoolForDevice(device);
	if(pool == VK_NULL_HANDLE)
		pool = __kvfAddCommandPoolToArray(device, __kvfCreateCommandPoolForDevice(device));

	VkCommandBuffer buffer;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = pool;
	alloc_info.level = level;
	alloc_info.commandBufferCount = 1;
	__kvfCheckVk(vkAllocateCommandBuffers(device, &alloc_info, &buffer));
	return buffer;
}

VkRenderPass kvfCreateRenderPass(VkDevice device)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkRenderPass renderPass = VK_NULL_HANDLE;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = __kvf_internal_swap_chain_image_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	__kvfCheckVk(vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass));
	return renderPass;
}

void kvfDestroyRenderPass(VkDevice device, VkRenderPass renderPass)
{
	if(renderPass == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroyRenderPass(device, renderPass, NULL);
}

VkShaderModule kvfCreateShaderModule(VkDevice device, uint32_t* code, size_t size)
{
	KVF_ASSERT(device != VK_NULL_HANDLE);
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size * sizeof(uint32_t);
	createInfo.pCode = code;
	VkShaderModule shader = VK_NULL_HANDLE;
	__kvfCheckVk(vkCreateShaderModule(device, &createInfo, NULL, &shader));
	return shader;
}

void kvfDestroyShaderModule(VkDevice device, VkShaderModule shader)
{
	if(shader == VK_NULL_HANDLE)
		return;
	KVF_ASSERT(device != VK_NULL_HANDLE);
	vkDestroyShaderModule(device, shader, NULL);
}

#endif // KVF_IMPLEMENTATION

#endif // KBZ_8_VULKAN_FRAMEWORK_H