#pragma once

#include "common.hpp"

namespace democollection::vk
{
	class PhysicalDevice
	{
	private:
		VkPhysicalDevice m_device;
		uint32_t m_graphicsQueueIndex;
		uint32_t m_presentQueueIndex;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkFormat m_depthFormat;
		VkPresentModeKHR m_presentMode;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkSampleCountFlagBits m_msaaSampleCount;

	public:
		static const char* sDeviceExtensions[1];

	private:
		bool CheckDeviceExtensionSupport() const;
		uint32_t ScorePhysicalDeviceQueues(VkSurfaceKHR surface);
		uint32_t ScoreSurfaceFormat(VkSurfaceKHR surface);
		uint32_t ScoreDepthFormat();
		uint32_t ScorePresentMode(VkSurfaceKHR surface);
		uint32_t ScoreSwapchainSupport(VkSurfaceKHR surface);
		uint32_t ScoreMultiSampling();
		uint32_t ScoreGpuType();

	public:
		PhysicalDevice(VkPhysicalDevice device = VK_NULL_HANDLE);
		uint32_t ScoreDevice(VkSurfaceKHR surface);
		void UpdateSurfaceDetails(VkSurfaceKHR surface);

		inline VkPhysicalDevice Device() const { return m_device; }
		inline uint32_t GraphicsQueueIndex() const { return m_graphicsQueueIndex; }
		inline uint32_t PresentQueueIndex() const { return m_presentQueueIndex; }
		inline const VkPhysicalDeviceProperties& Properties() const { return m_properties; }
		inline const VkPhysicalDeviceFeatures& Features() const { return m_features; }
		inline const VkSurfaceCapabilitiesKHR& SurfaceCapabilities() const { return m_surfaceCapabilities; }
		inline const VkSurfaceFormatKHR& SurfaceFormat() const { return m_surfaceFormat; }
		inline VkFormat DepthFormat() const { return m_depthFormat; }
		inline VkPresentModeKHR PresentMode() const { return m_presentMode; }
		inline VkSampleCountFlagBits MsaaSampleCount() const { return m_msaaSampleCount; }
	};
}
