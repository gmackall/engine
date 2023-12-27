// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_CORE_DEVICE_BUFFER_H_
#define FLUTTER_IMPELLER_CORE_DEVICE_BUFFER_H_

#include <memory>
#include <string>

#include "impeller/core/allocator.h"
#include "impeller/core/buffer.h"
#include "impeller/core/buffer_view.h"
#include "impeller/core/device_buffer_descriptor.h"
#include "impeller/core/range.h"
#include "impeller/core/texture.h"

namespace impeller {

class DeviceBuffer : public Buffer,
                     public std::enable_shared_from_this<DeviceBuffer> {
 public:
  virtual ~DeviceBuffer();

  [[nodiscard]] bool CopyHostBuffer(const uint8_t* source,
                                    Range source_range,
                                    size_t offset = 0u);

  virtual bool SetLabel(const std::string& label) = 0;

  virtual bool SetLabel(const std::string& label, Range range) = 0;

  BufferView AsBufferView() const;

  virtual std::shared_ptr<Texture> AsTexture(
      Allocator& allocator,
      const TextureDescriptor& descriptor,
      uint16_t row_bytes) const;

  // |Buffer|
  std::shared_ptr<const DeviceBuffer> GetDeviceBuffer(
      Allocator& allocator) const;

  const DeviceBufferDescriptor& GetDeviceBufferDescriptor() const;

  virtual uint8_t* OnGetContents() const = 0;

 protected:
  const DeviceBufferDescriptor desc_;

  explicit DeviceBuffer(DeviceBufferDescriptor desc);

  virtual bool OnCopyHostBuffer(const uint8_t* source,
                                Range source_range,
                                size_t offset) = 0;

 private:
  DeviceBuffer(const DeviceBuffer&) = delete;

  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_CORE_DEVICE_BUFFER_H_
