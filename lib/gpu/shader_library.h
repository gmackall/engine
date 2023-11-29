// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <string>
#include <unordered_map>

#include "flutter/lib/gpu/export.h"
#include "flutter/lib/gpu/shader.h"
#include "flutter/lib/ui/dart_wrapper.h"
#include "fml/memory/ref_ptr.h"

namespace flutter {
namespace gpu {

/// An immutable collection of shaders loaded from a shader bundle asset.
class ShaderLibrary : public RefCountedDartWrappable<ShaderLibrary> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ShaderLibrary);

 public:
  using ShaderMap = std::unordered_map<std::string, fml::RefPtr<Shader>>;

  static fml::RefPtr<ShaderLibrary> MakeFromAsset(const std::string& name,
                                                  std::string& out_error);

  static fml::RefPtr<ShaderLibrary> MakeFromShaders(ShaderMap shaders);

  /// Sets a return override for `MakeFromAsset` for testing purposes.
  static void SetOverride(fml::RefPtr<ShaderLibrary> override_shader_library);

  fml::RefPtr<Shader> GetShader(const std::string& shader_name,
                                Dart_Handle shader_wrapper) const;

  ~ShaderLibrary() override;

 private:
  /// A global override used to inject a ShaderLibrary when running with the
  /// Impeller playground. When set, `MakeFromAsset` will always just return
  /// this library.
  static fml::RefPtr<ShaderLibrary> override_shader_library_;

  ShaderMap shaders_;

  explicit ShaderLibrary(ShaderMap shaders);

  FML_DISALLOW_COPY_AND_ASSIGN(ShaderLibrary);
};

}  // namespace gpu
}  // namespace flutter

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

FLUTTER_GPU_EXPORT
extern Dart_Handle InternalFlutterGpu_ShaderLibrary_InitializeWithAsset(
    Dart_Handle wrapper,
    Dart_Handle asset_name);

FLUTTER_GPU_EXPORT
extern Dart_Handle InternalFlutterGpu_ShaderLibrary_GetShader(
    flutter::gpu::ShaderLibrary* wrapper,
    Dart_Handle shader_name,
    Dart_Handle shader_wrapper);

}  // extern "C"
