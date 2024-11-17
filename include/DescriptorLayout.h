#pragma once

#include <vulkan/vulkan.h>

class DescriptorLayout {
  public:
    void init(VkDevice device);
    void cleanup();
    VkDescriptorSetLayout getLayout() const { return layout; }

  private:
    VkDevice device;
    VkDescriptorSetLayout layout;
};
