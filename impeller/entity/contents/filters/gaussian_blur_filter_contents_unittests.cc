// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/testing/testing.h"
#include "fml/status_or.h"
#include "gmock/gmock.h"
#include "impeller/entity/contents/filters/gaussian_blur_filter_contents.h"
#include "impeller/entity/contents/texture_contents.h"
#include "impeller/entity/entity_playground.h"
#include "impeller/geometry/geometry_asserts.h"
#include "impeller/renderer/testing/mocks.h"

namespace impeller {
namespace testing {

namespace {

Scalar CalculateSigmaForBlurRadius(Scalar blur_radius) {
  // See Sigma.h
  return (blur_radius / kKernelRadiusPerSigma) + 0.5;
}

}  // namespace

class GaussianBlurFilterContentsTest : public EntityPlayground {
 public:
  std::shared_ptr<Texture> MakeTexture(const TextureDescriptor& desc) {
    return GetContentContext()
        ->GetContext()
        ->GetResourceAllocator()
        ->CreateTexture(desc);
  }
};
INSTANTIATE_PLAYGROUND_SUITE(GaussianBlurFilterContentsTest);

TEST(GaussianBlurFilterContentsTest, Create) {
  GaussianBlurFilterContents contents;
  ASSERT_EQ(contents.GetSigma(), 0.0);
}

TEST(GaussianBlurFilterContentsTest, CoverageEmpty) {
  GaussianBlurFilterContents contents;
  FilterInput::Vector inputs = {};
  Entity entity;
  std::optional<Rect> coverage =
      contents.GetFilterCoverage(inputs, entity, /*effect_transform=*/Matrix());
  ASSERT_FALSE(coverage.has_value());
}

TEST(GaussianBlurFilterContentsTest, CoverageSimple) {
  GaussianBlurFilterContents contents;
  FilterInput::Vector inputs = {
      FilterInput::Make(Rect::MakeLTRB(10, 10, 110, 110))};
  Entity entity;
  std::optional<Rect> coverage =
      contents.GetFilterCoverage(inputs, entity, /*effect_transform=*/Matrix());
  ASSERT_EQ(coverage, Rect::MakeLTRB(10, 10, 110, 110));
}

TEST(GaussianBlurFilterContentsTest, CoverageWithSigma) {
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  GaussianBlurFilterContents contents(/*sigma=*/sigma_radius_1);
  FilterInput::Vector inputs = {
      FilterInput::Make(Rect::MakeLTRB(100, 100, 200, 200))};
  Entity entity;
  std::optional<Rect> coverage =
      contents.GetFilterCoverage(inputs, entity, /*effect_transform=*/Matrix());
  ASSERT_EQ(coverage, Rect::MakeLTRB(99, 99, 201, 201));
}

TEST_P(GaussianBlurFilterContentsTest, CoverageWithTexture) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  GaussianBlurFilterContents contents(/*sigma=*/sigma_radius_1);
  std::shared_ptr<Texture> texture =
      GetContentContext()->GetContext()->GetResourceAllocator()->CreateTexture(
          desc);
  FilterInput::Vector inputs = {FilterInput::Make(texture)};
  Entity entity;
  entity.SetTransform(Matrix::MakeTranslation({100, 100, 0}));
  std::optional<Rect> coverage =
      contents.GetFilterCoverage(inputs, entity, /*effect_transform=*/Matrix());
  ASSERT_EQ(coverage, Rect::MakeLTRB(99, 99, 201, 201));
}

TEST_P(GaussianBlurFilterContentsTest, CoverageWithEffectTransform) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  GaussianBlurFilterContents contents(/*sigma=*/sigma_radius_1);
  std::shared_ptr<Texture> texture =
      GetContentContext()->GetContext()->GetResourceAllocator()->CreateTexture(
          desc);
  FilterInput::Vector inputs = {FilterInput::Make(texture)};
  Entity entity;
  entity.SetTransform(Matrix::MakeTranslation({100, 100, 0}));
  std::optional<Rect> coverage = contents.GetFilterCoverage(
      inputs, entity, /*effect_transform=*/Matrix::MakeScale({2.0, 2.0, 1.0}));
  ASSERT_EQ(coverage, Rect::MakeLTRB(100 - 2, 100 - 2, 200 + 2, 200 + 2));
}

TEST(GaussianBlurFilterContentsTest, FilterSourceCoverage) {
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  std::optional<Rect> coverage = contents->GetFilterSourceCoverage(
      /*effect_transform=*/Matrix::MakeScale({2.0, 2.0, 1.0}),
      /*output_limit=*/Rect::MakeLTRB(100, 100, 200, 200));
  ASSERT_EQ(coverage, Rect::MakeLTRB(100 - 2, 100 - 2, 200 + 2, 200 + 2));
}

TEST(GaussianBlurFilterContentsTest, CalculateSigmaValues) {
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(1.0f), 1);
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(2.0f), 1);
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(3.0f), 1);
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(4.0f), 1);
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(16.0f), 0.25);
  EXPECT_EQ(GaussianBlurFilterContents::CalculateScale(1024.0f), 4.f / 1024.f);
}

TEST_P(GaussianBlurFilterContentsTest, RenderCoverageMatchesGetCoverage) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };
  std::shared_ptr<Texture> texture = MakeTexture(desc);
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  contents->SetInputs({FilterInput::Make(texture)});
  std::shared_ptr<ContentContext> renderer = GetContentContext();

  Entity entity;
  std::optional<Entity> result =
      contents->GetEntity(*renderer, entity, /*coverage_hint=*/{});
  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value().GetBlendMode(), BlendMode::kSourceOver);
    std::optional<Rect> result_coverage = result.value().GetCoverage();
    std::optional<Rect> contents_coverage = contents->GetCoverage(entity);
    EXPECT_TRUE(result_coverage.has_value());
    EXPECT_TRUE(contents_coverage.has_value());
    if (result_coverage.has_value() && contents_coverage.has_value()) {
      EXPECT_TRUE(RectNear(contents_coverage.value(),
                           Rect::MakeLTRB(-1, -1, 101, 101)));
      EXPECT_TRUE(
          RectNear(result_coverage.value(), Rect::MakeLTRB(-1, -1, 101, 101)));
    }
  }
}

TEST_P(GaussianBlurFilterContentsTest,
       RenderCoverageMatchesGetCoverageTranslate) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };
  std::shared_ptr<Texture> texture = MakeTexture(desc);
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  contents->SetInputs({FilterInput::Make(texture)});
  std::shared_ptr<ContentContext> renderer = GetContentContext();

  Entity entity;
  entity.SetTransform(Matrix::MakeTranslation({100, 200, 0}));
  std::optional<Entity> result =
      contents->GetEntity(*renderer, entity, /*coverage_hint=*/{});

  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value().GetBlendMode(), BlendMode::kSourceOver);
    std::optional<Rect> result_coverage = result.value().GetCoverage();
    std::optional<Rect> contents_coverage = contents->GetCoverage(entity);
    EXPECT_TRUE(result_coverage.has_value());
    EXPECT_TRUE(contents_coverage.has_value());
    if (result_coverage.has_value() && contents_coverage.has_value()) {
      EXPECT_TRUE(RectNear(contents_coverage.value(),
                           Rect::MakeLTRB(99, 199, 201, 301)));
      EXPECT_TRUE(
          RectNear(result_coverage.value(), Rect::MakeLTRB(99, 199, 201, 301)));
    }
  }
}

TEST_P(GaussianBlurFilterContentsTest,
       RenderCoverageMatchesGetCoverageRotated) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(400, 300),
  };
  std::shared_ptr<Texture> texture = MakeTexture(desc);
  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  contents->SetInputs({FilterInput::Make(texture)});
  std::shared_ptr<ContentContext> renderer = GetContentContext();

  Entity entity;
  // Rotate around the top left corner, then push it over to (100, 100).
  entity.SetTransform(Matrix::MakeTranslation({400, 100, 0}) *
                      Matrix::MakeRotationZ(Degrees(90.0)));
  std::optional<Entity> result =
      contents->GetEntity(*renderer, entity, /*coverage_hint=*/{});
  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value().GetBlendMode(), BlendMode::kSourceOver);
    std::optional<Rect> result_coverage = result.value().GetCoverage();
    std::optional<Rect> contents_coverage = contents->GetCoverage(entity);
    EXPECT_TRUE(result_coverage.has_value());
    EXPECT_TRUE(contents_coverage.has_value());
    if (result_coverage.has_value() && contents_coverage.has_value()) {
      EXPECT_TRUE(RectNear(contents_coverage.value(),
                           Rect::MakeLTRB(99, 99, 401, 501)));
      EXPECT_TRUE(
          RectNear(result_coverage.value(), Rect::MakeLTRB(99, 99, 401, 501)));
    }
  }
}

TEST_P(GaussianBlurFilterContentsTest, CalculateUVsSimple) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };
  std::shared_ptr<Texture> texture = MakeTexture(desc);
  auto filter_input = FilterInput::Make(texture);
  Entity entity;
  Quad uvs = GaussianBlurFilterContents::CalculateUVs(filter_input, entity,
                                                      ISize(100, 100));
  std::optional<Rect> uvs_bounds = Rect::MakePointBounds(uvs);
  EXPECT_TRUE(uvs_bounds.has_value());
  if (uvs_bounds.has_value()) {
    EXPECT_TRUE(RectNear(uvs_bounds.value(), Rect::MakeXYWH(0, 0, 1, 1)));
  }
}

TEST_P(GaussianBlurFilterContentsTest, TextureContentsWithDestinationRect) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };

  std::shared_ptr<Texture> texture = MakeTexture(desc);
  auto texture_contents = std::make_shared<TextureContents>();
  texture_contents->SetSourceRect(Rect::MakeSize(texture->GetSize()));
  texture_contents->SetTexture(texture);
  texture_contents->SetDestinationRect(Rect::MakeXYWH(
      50, 40, texture->GetSize().width, texture->GetSize().height));

  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  contents->SetInputs({FilterInput::Make(texture_contents)});
  std::shared_ptr<ContentContext> renderer = GetContentContext();

  Entity entity;
  std::optional<Entity> result =
      contents->GetEntity(*renderer, entity, /*coverage_hint=*/{});
  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value().GetBlendMode(), BlendMode::kSourceOver);
    std::optional<Rect> result_coverage = result.value().GetCoverage();
    std::optional<Rect> contents_coverage = contents->GetCoverage(entity);
    EXPECT_TRUE(result_coverage.has_value());
    EXPECT_TRUE(contents_coverage.has_value());
    if (result_coverage.has_value() && contents_coverage.has_value()) {
      EXPECT_TRUE(RectNear(result_coverage.value(), contents_coverage.value()));
      EXPECT_TRUE(RectNear(result_coverage.value(),
                           Rect::MakeLTRB(49.f, 39.f, 151.f, 141.f)));
    }
  }
}

TEST_P(GaussianBlurFilterContentsTest,
       TextureContentsWithDestinationRectScaled) {
  TextureDescriptor desc = {
      .storage_mode = StorageMode::kDevicePrivate,
      .format = PixelFormat::kB8G8R8A8UNormInt,
      .size = ISize(100, 100),
  };

  std::shared_ptr<Texture> texture = MakeTexture(desc);
  auto texture_contents = std::make_shared<TextureContents>();
  texture_contents->SetSourceRect(Rect::MakeSize(texture->GetSize()));
  texture_contents->SetTexture(texture);
  texture_contents->SetDestinationRect(Rect::MakeXYWH(
      50, 40, texture->GetSize().width, texture->GetSize().height));

  Scalar sigma_radius_1 = CalculateSigmaForBlurRadius(1.0);
  auto contents = std::make_unique<GaussianBlurFilterContents>(sigma_radius_1);
  contents->SetInputs({FilterInput::Make(texture_contents)});
  std::shared_ptr<ContentContext> renderer = GetContentContext();

  Entity entity;
  entity.SetTransform(Matrix::MakeScale({2.0, 2.0, 1.0}));
  std::optional<Entity> result =
      contents->GetEntity(*renderer, entity, /*coverage_hint=*/{});
  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value().GetBlendMode(), BlendMode::kSourceOver);
    std::optional<Rect> result_coverage = result.value().GetCoverage();
    std::optional<Rect> contents_coverage = contents->GetCoverage(entity);
    EXPECT_TRUE(result_coverage.has_value());
    EXPECT_TRUE(contents_coverage.has_value());
    if (result_coverage.has_value() && contents_coverage.has_value()) {
      EXPECT_TRUE(RectNear(result_coverage.value(), contents_coverage.value()));
      EXPECT_TRUE(RectNear(contents_coverage.value(),
                           Rect::MakeLTRB(98.f, 78.f, 302.f, 282.f)));
    }
  }
}

}  // namespace testing
}  // namespace impeller
