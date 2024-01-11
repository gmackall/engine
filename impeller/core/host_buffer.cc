// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/core/host_buffer.h"

#include <cstring>
#include <tuple>

#include "impeller/core/allocator.h"
#include "impeller/core/buffer_view.h"
#include "impeller/core/device_buffer.h"
#include "impeller/core/device_buffer_descriptor.h"
#include "impeller/core/formats.h"

namespace impeller {

constexpr size_t kAllocatorBlockSize = 1024000;  // 1024 Kb.

std::shared_ptr<HostBuffer> HostBuffer::Create(
    const std::shared_ptr<Allocator>& allocator) {
  return std::shared_ptr<HostBuffer>(new HostBuffer(allocator));
}

HostBuffer::HostBuffer(const std::shared_ptr<Allocator>& allocator)
    : allocator_(allocator) {
  DeviceBufferDescriptor desc;
  desc.size = kAllocatorBlockSize;
  desc.storage_mode = StorageMode::kHostVisible;
  for (auto i = 0u; i < kHostBufferArenaSize; i++) {
    device_buffers_[i].push_back(allocator->CreateBuffer(desc));
  }
}

HostBuffer::~HostBuffer() = default;

void HostBuffer::SetLabel(std::string label) {
  label_ = std::move(label);
}

BufferView HostBuffer::Emplace(const void* buffer,
                               size_t length,
                               size_t align) {
  auto [data, range, device_buffer] = EmplaceInternal(buffer, length, align);
  if (!device_buffer) {
    return {};
  }
  return BufferView{std::move(device_buffer), data, range};
}

BufferView HostBuffer::Emplace(const void* buffer, size_t length) {
  auto [data, range, device_buffer] = EmplaceInternal(buffer, length);
  if (!device_buffer) {
    return {};
  }
  return BufferView{std::move(device_buffer), data, range};
}

BufferView HostBuffer::Emplace(size_t length,
                               size_t align,
                               const EmplaceProc& cb) {
  auto [data, range, device_buffer] = EmplaceInternal(length, align, cb);
  if (!device_buffer) {
    return {};
  }
  return BufferView{std::move(device_buffer), data, range};
}

HostBuffer::TestStateQuery HostBuffer::GetStateForTest() {
  return HostBuffer::TestStateQuery{
      .current_frame = frame_index_,
      .current_buffer = current_buffer_,
      .total_buffer_count = device_buffers_[frame_index_].size(),
  };
}

void HostBuffer::MaybeCreateNewBuffer(size_t required_size) {
  current_buffer_++;
  if (current_buffer_ >= device_buffers_[frame_index_].size()) {
    FML_DCHECK(required_size <= kAllocatorBlockSize);
    DeviceBufferDescriptor desc;
    desc.size = kAllocatorBlockSize;
    desc.storage_mode = StorageMode::kHostVisible;
    device_buffers_[frame_index_].push_back(allocator_->CreateBuffer(desc));
  }
  offset_ = 0;
}

std::tuple<uint8_t*, Range, std::shared_ptr<DeviceBuffer>>
HostBuffer::EmplaceInternal(size_t length,
                            size_t align,
                            const EmplaceProc& cb) {
  if (!cb) {
    return {};
  }

  // If the requested allocation is bigger than the block size, create a one-off
  // device buffer and write to that.
  if (length > kAllocatorBlockSize) {
    DeviceBufferDescriptor desc;
    desc.size = length;
    desc.storage_mode = StorageMode::kHostVisible;
    auto device_buffer = allocator_->CreateBuffer(desc);
    if (!device_buffer) {
      return {};
    }
    if (cb) {
      cb(device_buffer->OnGetContents());
      device_buffer->Flush(Range{0, length});
    }
    return std::make_tuple(device_buffer->OnGetContents(), Range{0, length},
                           device_buffer);
  }

  auto old_length = GetLength();
  if (old_length + length > kAllocatorBlockSize) {
    MaybeCreateNewBuffer(length);
  }
  old_length = GetLength();

  auto current_buffer = GetCurrentBuffer();
  cb(current_buffer->OnGetContents() + old_length);
  current_buffer->Flush(Range{old_length, length});

  offset_ += length;
  auto contents = current_buffer->OnGetContents();
  return std::make_tuple(contents, Range{old_length, length},
                         std::move(current_buffer));
}

std::tuple<uint8_t*, Range, std::shared_ptr<DeviceBuffer>>
HostBuffer::EmplaceInternal(const void* buffer, size_t length) {
  // If the requested allocation is bigger than the block size, create a one-off
  // device buffer and write to that.
  if (length > kAllocatorBlockSize) {
    DeviceBufferDescriptor desc;
    desc.size = length;
    desc.storage_mode = StorageMode::kHostVisible;
    auto device_buffer = allocator_->CreateBuffer(desc);
    if (!device_buffer) {
      return {};
    }
    if (buffer) {
      if (!device_buffer->CopyHostBuffer(static_cast<const uint8_t*>(buffer),
                                         Range{0, length})) {
        return {};
      }
    }
    return std::make_tuple(device_buffer->OnGetContents(), Range{0, length},
                           device_buffer);
  }

  auto old_length = GetLength();
  if (old_length + length > kAllocatorBlockSize) {
    MaybeCreateNewBuffer(length);
  }
  old_length = GetLength();

  auto current_buffer = GetCurrentBuffer();
  if (buffer) {
    ::memmove(current_buffer->OnGetContents() + old_length, buffer, length);
    current_buffer->Flush(Range{old_length, length});
  }
  offset_ += length;
  auto contents = current_buffer->OnGetContents();
  return std::make_tuple(contents, Range{old_length, length},
                         std::move(current_buffer));
}

std::tuple<uint8_t*, Range, std::shared_ptr<DeviceBuffer>>
HostBuffer::EmplaceInternal(const void* buffer, size_t length, size_t align) {
  if (align == 0 || (GetLength() % align) == 0) {
    return EmplaceInternal(buffer, length);
  }

  {
    auto [buffer, range, device_buffer] =
        EmplaceInternal(nullptr, align - (GetLength() % align));
    if (!buffer) {
      return {};
    }
  }

  return EmplaceInternal(buffer, length);
}

void HostBuffer::Reset() {
  // When resetting the host buffer state at the end of the frame, check if
  // there are any unused buffers and remove them.
  while (device_buffers_[frame_index_].size() > current_buffer_ + 1) {
    device_buffers_[frame_index_].pop_back();
  }

  offset_ = 0u;
  current_buffer_ = 0u;
  frame_index_ = (frame_index_ + 1) % kHostBufferArenaSize;
}

}  // namespace impeller
