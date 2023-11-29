// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>
#include <string>

#include "flutter/lib/gpu/context.h"
#include "flutter/lib/ui/dart_wrapper.h"
#include "fml/memory/ref_ptr.h"
#include "impeller/core/runtime_types.h"
#include "impeller/core/shader_types.h"
#include "impeller/renderer/shader_function.h"
#include "impeller/renderer/vertex_descriptor.h"

namespace flutter {
namespace gpu {

/// An immutable collection of shaders loaded from a shader bundle asset.
class Shader : public RefCountedDartWrappable<Shader> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Shader);

 public:
  ~Shader() override;

  static fml::RefPtr<Shader> Make(
      std::string entrypoint,
      impeller::ShaderStage stage,
      std::shared_ptr<fml::Mapping> code_mapping,
      std::vector<impeller::RuntimeUniformDescription> uniforms,
      std::shared_ptr<impeller::VertexDescriptor> vertex_desc);

  std::shared_ptr<const impeller::ShaderFunction> GetFunctionFromLibrary(
      impeller::ShaderLibrary& library);

  bool IsRegistered(Context& context);

  bool RegisterSync(Context& context);

  std::shared_ptr<impeller::VertexDescriptor> GetVertexDescriptor() const;

  impeller::ShaderStage GetShaderStage() const;

  int GetUniformSlot(const std::string& name) const;

 private:
  Shader();

  std::string entrypoint_;
  impeller::ShaderStage stage_;
  std::shared_ptr<fml::Mapping> code_mapping_;
  std::vector<impeller::RuntimeUniformDescription> uniforms_;
  std::shared_ptr<impeller::VertexDescriptor> vertex_desc_;

  FML_DISALLOW_COPY_AND_ASSIGN(Shader);
};

}  // namespace gpu
}  // namespace flutter

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

FLUTTER_GPU_EXPORT
extern int InternalFlutterGpu_Shader_GetShaderStage(
    flutter::gpu::Shader* wrapper);

FLUTTER_GPU_EXPORT
extern int InternalFlutterGpu_Shader_GetUniformSlot(
    flutter::gpu::Shader* wrapper,
    Dart_Handle name_handle);

}  // extern "C"
