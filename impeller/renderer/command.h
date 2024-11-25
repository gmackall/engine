// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_RENDERER_COMMAND_H_
#define FLUTTER_IMPELLER_RENDERER_COMMAND_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "impeller/core/buffer_view.h"
#include "impeller/core/formats.h"
#include "impeller/core/resource_binder.h"
#include "impeller/core/sampler.h"
#include "impeller/core/shader_types.h"
#include "impeller/core/texture.h"
#include "impeller/core/vertex_buffer.h"
#include "impeller/geometry/rect.h"
#include "impeller/renderer/pipeline.h"

namespace impeller {

#ifdef IMPELLER_DEBUG
#define DEBUG_COMMAND_INFO(obj, arg) obj.label = arg;
#else
#define DEBUG_COMMAND_INFO(obj, arg)
#endif  // IMPELLER_DEBUG

template <class T>
class Resource {
 public:
  using ResourceType = T;
  ResourceType resource;

  Resource() {}

  Resource(const ShaderMetadata* metadata, ResourceType p_resource)
      : resource(p_resource), metadata_(metadata) {}

  Resource(const ShaderMetadata& metadata, ResourceType p_resource)
      : resource(p_resource),
        dynamic_metadata_(std::make_unique<ShaderMetadata>(metadata)) {}

  const ShaderMetadata* GetMetadata() const {
    return dynamic_metadata_ ? dynamic_metadata_.get() : metadata_;
  }

 private:
  // Static shader metadata (typically generated by ImpellerC).
  const ShaderMetadata* metadata_ = nullptr;

  // Dynamically generated shader metadata.
  std::unique_ptr<const ShaderMetadata> dynamic_metadata_ = nullptr;
};

using BufferResource = Resource<BufferView>;
using TextureResource = Resource<std::shared_ptr<const Texture>>;

/// @brief combines the texture, sampler and sampler slot information.
struct TextureAndSampler {
  SampledImageSlot slot;
  TextureResource texture;
  const std::unique_ptr<const Sampler>* sampler;
};

/// @brief combines the buffer resource and its uniform slot information.
struct BufferAndUniformSlot {
  ShaderUniformSlot slot;
  BufferResource view;
};

struct Bindings {
  std::vector<TextureAndSampler> sampled_images;
  std::vector<BufferAndUniformSlot> buffers;
};

//------------------------------------------------------------------------------
/// @brief      An object used to specify work to the GPU along with references
///             to resources the GPU will used when doing said work.
///
///             To construct a valid command, follow these steps:
///             * Specify a valid pipeline.
///             * Specify vertex information via a call `BindVertices`
///             * Specify any stage bindings.
///             * (Optional) Specify a debug label.
///
///             Command can be created frequently and on demand. The resources
///             referenced in commands views into buffers managed by other
///             allocators and resource managers.
///
struct Command : public ResourceBinder {
  //----------------------------------------------------------------------------
  /// The pipeline to use for this command.
  ///
  std::shared_ptr<Pipeline<PipelineDescriptor>> pipeline;
  //----------------------------------------------------------------------------
  /// The buffer, texture, and sampler bindings used by the vertex pipeline
  /// stage.
  ///
  Bindings vertex_bindings;
  //----------------------------------------------------------------------------
  /// The buffer, texture, and sampler bindings used by the fragment pipeline
  /// stage.
  ///
  Bindings fragment_bindings;

#ifdef IMPELLER_DEBUG
  //----------------------------------------------------------------------------
  /// The debugging label to use for the command.
  ///
  std::string label;
#endif  // IMPELLER_DEBUG

  //----------------------------------------------------------------------------
  /// The reference value to use in stenciling operations. Stencil configuration
  /// is part of pipeline setup and can be read from the pipelines descriptor.
  ///
  /// @see         `Pipeline`
  /// @see         `PipelineDescriptor`
  ///
  uint32_t stencil_reference = 0u;
  //----------------------------------------------------------------------------
  /// The offset used when indexing into the vertex buffer.
  ///
  uint64_t base_vertex = 0u;
  //----------------------------------------------------------------------------
  /// The viewport coordinates that the rasterizer linearly maps normalized
  /// device coordinates to.
  /// If unset, the viewport is the size of the render target with a zero
  /// origin, znear=0, and zfar=1.
  ///
  std::optional<Viewport> viewport;
  //----------------------------------------------------------------------------
  /// The scissor rect to use for clipping writes to the render target. The
  /// scissor rect must lie entirely within the render target.
  /// If unset, no scissor is applied.
  ///
  std::optional<IRect> scissor;
  //----------------------------------------------------------------------------
  /// The number of instances of the given set of vertices to render. Not all
  /// backends support rendering more than one instance at a time.
  ///
  /// @warning      Setting this to more than one will limit the availability of
  ///               backends to use with this command.
  ///
  size_t instance_count = 1u;

  //----------------------------------------------------------------------------
  /// The vertex buffers used by the vertex shader stage.
  std::array<BufferView, kMaxVertexBuffers> vertex_buffers;

  //----------------------------------------------------------------------------
  /// The number of vertex buffers in the vertex_buffers array. Must not exceed
  /// kMaxVertexBuffers.
  size_t vertex_buffer_count = 0u;

  //----------------------------------------------------------------------------
  /// The index buffer binding used by the vertex shader stage.
  BufferView index_buffer;

  //----------------------------------------------------------------------------
  /// The number of elements to draw. When only a vertex buffer is set, this is
  /// the vertex count. When an index buffer is set, this is the index count.
  size_t element_count = 0u;

  //----------------------------------------------------------------------------
  /// The type of indices in the index buffer. The indices must be tightly
  /// packed in the index buffer.
  IndexType index_type = IndexType::kUnknown;

  //----------------------------------------------------------------------------
  /// @brief      Specify the vertex and index buffer to use for this command.
  ///
  /// @param[in]  buffer  The vertex and index buffer definition. If possible,
  ///             this value should be moved and not copied.
  ///
  /// @return     returns if the binding was updated.
  ///
  bool BindVertices(const VertexBuffer& buffer);

  // |ResourceBinder|
  bool BindResource(ShaderStage stage,
                    DescriptorType type,
                    const ShaderUniformSlot& slot,
                    const ShaderMetadata& metadata,
                    BufferView view) override;

  bool BindResource(ShaderStage stage,
                    DescriptorType type,
                    const ShaderUniformSlot& slot,
                    const std::shared_ptr<const ShaderMetadata>& metadata,
                    BufferView view);

  // |ResourceBinder|
  bool BindResource(ShaderStage stage,
                    DescriptorType type,
                    const SampledImageSlot& slot,
                    const ShaderMetadata& metadata,
                    std::shared_ptr<const Texture> texture,
                    const std::unique_ptr<const Sampler>& sampler) override;

  bool IsValid() const { return pipeline && pipeline->IsValid(); }

 private:
  template <class T>
  bool DoBindResource(ShaderStage stage,
                      const ShaderUniformSlot& slot,
                      T metadata,
                      BufferView view);
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_RENDERER_COMMAND_H_
