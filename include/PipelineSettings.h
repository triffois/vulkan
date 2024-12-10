#pragma once

#include "IAttachment.h"
#include <cstring>
#include <string>
#include <vector>

class PipelineSettings {
  public:
    PipelineSettings(std::string vertexShaderPath,
                     std::string fragmentShaderPath)
        : vertexShaderPath(vertexShaderPath),
          fragmentShaderPath(fragmentShaderPath) {}

    void bind(IAttachment &attachment) { attachments.push_back(attachment); }

    std::vector<std::reference_wrapper<IAttachment>> &getAttachments() {
        return attachments;
    }
    std::string getVertexShaderPath() { return vertexShaderPath; }
    std::string getFragmentShaderPath() { return fragmentShaderPath; }

  private:
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    std::vector<std::reference_wrapper<IAttachment>> attachments;
};
