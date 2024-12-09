#include "DescriptorLayout.h"
#include "IAttachment.h"
#include <stdexcept>

DescriptorLayout::DescriptorLayout(
    VkDevice device,
    std::vector<std::reference_wrapper<IAttachment>> attachments)
    : device(device) {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(attachments.size());
    for (auto &attachment : attachments) {
        auto binding = attachment.get().layoutBinding();
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorLayout::cleanup() {
    if (layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
        layout = VK_NULL_HANDLE;
    }
}
