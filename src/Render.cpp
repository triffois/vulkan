#include "Render.h"
#include <stdexcept>

Render::Render(GlobalResources *globalResources, VkCommandBuffer commandBuffer,
               uint32_t imageIndex, uint32_t currentFrame,
               VkSemaphore imageAvailableSemaphore,
               VkSemaphore renderFinishedSemaphore, VkFence inFlightFence,
               Camera &camera)
    : globalResources(globalResources), commandBuffer(commandBuffer),
      imageIndex(imageIndex), currentFrame(currentFrame),
      imageAvailableSemaphore(imageAvailableSemaphore),
      renderFinishedSemaphore(renderFinishedSemaphore),
      inFlightFence(inFlightFence), camera(camera) {
    auto &swapChain = globalResources->getSwapChain();

    // Setup rendering info
    VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachmentInfo.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.imageView = swapChain.getImageView(imageIndex);
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;

    VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depthAttachmentInfo.clearValue.depthStencil = {1.0f, 0};
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentInfo.imageView = swapChain.getDepthImageView();
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;

    VkRenderingInfoKHR renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;
    renderingInfo.layerCount = 1;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = swapChain.getExtent();

    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void Render::submit(Renderable &renderable) {
    if (isFinished) {
        throw std::runtime_error("Cannot submit to a finished render!");
    }

    recordRenderingCommands(renderable);
}

void Render::recordRenderingCommands(Renderable &renderable) {
    for (RenderPass &pass : renderable.getRenderPasses()) {
        recordRenderingCommands(pass);
    }
}

void Render::recordRenderingCommands(RenderPass &pass) {
    auto &swapChain = globalResources->getSwapChain();

    auto &pipeline =
        globalResources->getPipelineManager().getPipeline(pass.getPipelineId());
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline.getPipeline());

    pass.update(currentFrame);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain.getExtent().width);
    viewport.height = static_cast<float>(swapChain.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    auto mesh = globalResources->getMeshManager().getMesh(pass.getMeshId());
    VkBuffer vertexBuffers[] = {mesh->vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    VkBuffer instanceDataBuffers[] = {pass.getInstanceBuffer()};
    VkDeviceSize instanceOffsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 1, 1, instanceDataBuffers,
                           instanceOffsets);

    vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer->getBuffer(), 0,
                         VK_INDEX_TYPE_UINT16);

    auto currentDescriptorSet = pass.getDescriptorSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.getLayout(), 0, 1, &currentDescriptorSet,
                            0, nullptr);

    vkCmdDrawIndexed(commandBuffer, mesh->indexCount, pass.getInstanceCount(),
                     0, 0, 0);
}

void Render::submitCommandBuffer() {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (globalResources->getDevice()->submitToAvailableGraphicsQueue(
            &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }
}

bool Render::finish() {
    if (isFinished) {
        throw std::runtime_error("Render is already finished!");
    }
    auto &swapChain = globalResources->getSwapChain();

    vkCmdEndRendering(commandBuffer);

    // Use SwapChain's transition method
    swapChain.transitionImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, imageIndex,
                                    commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    isFinished = true;
    submitCommandBuffer();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

    VkSwapchainKHR swapChains[] = {swapChain.getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result =
        globalResources->getDevice()->submitToAvailablePresentQueue(
            &presentInfo);
    return result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
}
