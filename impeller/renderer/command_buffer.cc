// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/renderer/command_buffer.h"

#include "flutter/fml/trace_event.h"
#include "impeller/renderer/compute_pass.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/render_target.h"

namespace impeller {

CommandBuffer::CommandBuffer(std::weak_ptr<const Context> context)
    : context_(std::move(context)) {}

CommandBuffer::~CommandBuffer() = default;

bool CommandBuffer::SubmitCommands(const CompletionCallback& callback) {
  TRACE_EVENT0("impeller", "CommandBuffer::SubmitCommands");
  if (!IsValid()) {
    // Already committed or was never valid. Either way, this is caller error.
    if (callback) {
      callback(Status::kError);
    }
    return false;
  }
  return OnSubmitCommands(callback);
}

bool CommandBuffer::SubmitCommands() {
  return SubmitCommands(nullptr);
}

void CommandBuffer::WaitUntilScheduled() {
  return OnWaitUntilScheduled();
}

bool CommandBuffer::EncodeAndSubmit(
    const std::shared_ptr<RenderPass>& render_pass) {
  TRACE_EVENT0("impeller", "CommandBuffer::EncodeAndSubmit");
  if (!render_pass->IsValid() || !IsValid()) {
    return false;
  }
  if (!render_pass->EncodeCommands()) {
    return false;
  }

  return SubmitCommands(nullptr);
}

bool CommandBuffer::EncodeAndSubmit(
    const std::shared_ptr<BlitPass>& blit_pass,
    const std::shared_ptr<Allocator>& allocator) {
  TRACE_EVENT0("impeller", "CommandBuffer::EncodeAndSubmit");
  if (!blit_pass->IsValid() || !IsValid()) {
    return false;
  }
  if (!blit_pass->EncodeCommands(allocator)) {
    return false;
  }

  return SubmitCommands(nullptr);
}

std::shared_ptr<RenderPass> CommandBuffer::CreateRenderPass(
    const RenderTarget& render_target) {
  auto pass = OnCreateRenderPass(render_target);
  if (pass && pass->IsValid()) {
    pass->SetLabel("RenderPass");
    return pass;
  }
  return nullptr;
}

std::shared_ptr<BlitPass> CommandBuffer::CreateBlitPass() {
  auto pass = OnCreateBlitPass();
  if (pass && pass->IsValid()) {
    pass->SetLabel("BlitPass");
    return pass;
  }
  return nullptr;
}

std::shared_ptr<ComputePass> CommandBuffer::CreateComputePass() {
  if (!IsValid()) {
    return nullptr;
  }
  auto pass = OnCreateComputePass();
  if (pass && pass->IsValid()) {
    pass->SetLabel("ComputePass");
    return pass;
  }
  return nullptr;
}

}  // namespace impeller
