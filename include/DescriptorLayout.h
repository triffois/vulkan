#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

// Forward declaration
class IAttachment;

class DescriptorLayout {
  public:
    void init(VkDevice device,
              std::vector<std::reference_wrapper<IAttachment>> attachments);
    void cleanup();
    VkDescriptorSetLayout getLayout() const { return layout; }

  private:
    VkDevice device;
    VkDescriptorSetLayout layout;
};
