// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <map>

#include "flutter/fml/macros.h"
#include "flutter/fml/mapping.h"
#include "impeller/shader_archive/shader_archive_types.h"

namespace impeller {

class MultiArchShaderArchiveWriter {
 public:
  MultiArchShaderArchiveWriter();

  ~MultiArchShaderArchiveWriter();

  [[nodiscard]] bool RegisterShaderArchive(
      ArchiveRenderingBackend backend,
      std::shared_ptr<const fml::Mapping> mapping);

  std::shared_ptr<fml::Mapping> CreateMapping() const;

 private:
  std::map<ArchiveRenderingBackend, std::shared_ptr<const fml::Mapping>>
      archives_;

  MultiArchShaderArchiveWriter(const MultiArchShaderArchiveWriter&) = delete;

  MultiArchShaderArchiveWriter& operator=(const MultiArchShaderArchiveWriter&) =
      delete;
};

}  // namespace impeller
