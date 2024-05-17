// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/display_list/utils/dl_matrix_clip_tracker.h"
#include "flutter/testing/assertions_skia.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {

using DlRect = impeller::Rect;
using DlMatrix = impeller::Matrix;
using Degrees = impeller::Degrees;

TEST(DisplayListMatrixClipTracker, Constructor) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), m44);
  EXPECT_EQ(tracker1.matrix(), dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), m44);
  EXPECT_EQ(tracker2.matrix(), dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), m44);
  EXPECT_EQ(tracker3.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipState, Constructor) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), matrix);
  EXPECT_EQ(state1.matrix_4x4(), m44);
  EXPECT_EQ(state1.matrix(), dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), matrix);
  EXPECT_EQ(state2.matrix_4x4(), m44);
  EXPECT_EQ(state2.matrix(), dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), matrix);
  EXPECT_EQ(state3.matrix_4x4(), m44);
  EXPECT_EQ(state3.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipTracker, Constructor4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, m44);
  DisplayListMatrixClipTracker tracker2(dl_cull_rect, dl_matrix);

  EXPECT_TRUE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), m44);
  EXPECT_EQ(tracker1.matrix(), dl_matrix);

  EXPECT_TRUE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), m44);
  EXPECT_EQ(tracker2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipState, Constructor4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipState state1(cull_rect, m44);
  DisplayListMatrixClipState state2(dl_cull_rect, dl_matrix);

  EXPECT_TRUE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_4x4(), m44);
  EXPECT_EQ(state1.matrix(), dl_matrix);

  EXPECT_TRUE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_4x4(), m44);
  EXPECT_EQ(state2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipTracker, TransformTo4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, SkMatrix::I());
  DisplayListMatrixClipTracker tracker2(dl_cull_rect, DlMatrix());
  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_FALSE(tracker2.using_4x4_matrix());

  tracker1.transform(m44);
  EXPECT_TRUE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), m44);
  EXPECT_EQ(tracker1.matrix(), dl_matrix);

  tracker2.transform(dl_matrix);
  EXPECT_TRUE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), m44);
  EXPECT_EQ(tracker2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipState, TransformTo4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker state1(cull_rect, SkMatrix::I());
  DisplayListMatrixClipTracker state2(dl_cull_rect, DlMatrix());
  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_FALSE(state2.using_4x4_matrix());

  state1.transform(m44);
  EXPECT_TRUE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_4x4(), m44);
  EXPECT_EQ(state1.matrix(), dl_matrix);

  state2.transform(dl_matrix);
  EXPECT_TRUE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_4x4(), m44);
  EXPECT_EQ(state2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipTracker, SetTo4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, SkMatrix::I());
  DisplayListMatrixClipTracker tracker2(dl_cull_rect, DlMatrix());
  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_FALSE(tracker2.using_4x4_matrix());

  tracker1.setTransform(m44);
  EXPECT_TRUE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), m44);
  EXPECT_EQ(tracker1.matrix(), dl_matrix);

  tracker2.setTransform(dl_matrix);
  EXPECT_TRUE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), m44);
  EXPECT_EQ(tracker2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipState, SetTo4x4) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipState state1(cull_rect, SkMatrix::I());
  DisplayListMatrixClipState state2(dl_cull_rect, DlMatrix());
  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_FALSE(state2.using_4x4_matrix());

  state1.setTransform(m44);
  EXPECT_TRUE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_4x4(), m44);
  EXPECT_EQ(state1.matrix(), dl_matrix);

  state2.setTransform(dl_matrix);
  EXPECT_TRUE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_4x4(), m44);
  EXPECT_EQ(state2.matrix(), dl_matrix);
}

TEST(DisplayListMatrixClipTracker, UpgradeTo4x4SaveAndRestore) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  // clang-format off
  const SkM44 m44 = SkM44(4, 0, 0.5, 0,
                          0, 4, 0.5, 0,
                          0, 0, 4.0, 0,
                          0, 0, 0.0, 1);
  const DlMatrix dl_matrix = DlMatrix::MakeRow(4, 0, 0.5, 0,
                                               0, 4, 0.5, 0,
                                               0, 0, 4.0, 0,
                                               0, 0, 0.0, 1);
  // clang-format on
  const SkRect local_cull_rect = SkRect::MakeLTRB(5, 10, 15, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, SkMatrix::I());
  DisplayListMatrixClipTracker tracker2(dl_cull_rect, DlMatrix());
  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_FALSE(tracker2.using_4x4_matrix());

  tracker1.save();
  tracker2.save();
  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_FALSE(tracker2.using_4x4_matrix());

  tracker1.transform(m44);
  EXPECT_TRUE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), m44);
  EXPECT_EQ(tracker1.matrix(), dl_matrix);

  tracker1.restore();
  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), SkM44());
  EXPECT_EQ(tracker1.matrix(), DlMatrix());

  tracker2.transform(dl_matrix);
  EXPECT_TRUE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), m44);
  EXPECT_EQ(tracker2.matrix(), dl_matrix);

  tracker2.restore();
  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), SkM44());
  EXPECT_EQ(tracker2.matrix(), DlMatrix());
}

// No "State" version of UpgradeTo4x4SaveAndRestore since the State objects
// do not have save and restore semantics.

TEST(DisplayListMatrixClipTracker, Translate) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  const SkMatrix translated_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Translate(5, 1));
  const SkM44 translated_m44 = SkM44(translated_matrix);
  const DlMatrix dl_translated_matrix =
      dl_matrix * DlMatrix::MakeTranslation({5.0, 1.0});
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 9, 10, 19);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.translate(5, 1);
  tracker2.translate(5, 1);
  tracker3.translate(5, 1);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), translated_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), translated_m44);
  EXPECT_EQ(tracker1.matrix(), dl_translated_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), translated_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), translated_m44);
  EXPECT_EQ(tracker2.matrix(), dl_translated_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), translated_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), translated_m44);
  EXPECT_EQ(tracker3.matrix(), dl_translated_matrix);
}

TEST(DisplayListMatrixClipState, Translate) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  const SkMatrix translated_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Translate(5, 1));
  const SkM44 translated_m44 = SkM44(translated_matrix);
  const DlMatrix dl_translated_matrix =
      dl_matrix * DlMatrix::MakeTranslation({5.0, 1.0});
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 9, 10, 19);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.translate(5, 1);
  state2.translate(5, 1);
  state3.translate(5, 1);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), translated_matrix);
  EXPECT_EQ(state1.matrix_4x4(), translated_m44);
  EXPECT_EQ(state1.matrix(), dl_translated_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), translated_matrix);
  EXPECT_EQ(state2.matrix_4x4(), translated_m44);
  EXPECT_EQ(state2.matrix(), dl_translated_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), translated_matrix);
  EXPECT_EQ(state3.matrix_4x4(), translated_m44);
  EXPECT_EQ(state3.matrix(), dl_translated_matrix);
}

TEST(DisplayListMatrixClipTracker, Scale) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  // Scale factor carefully chosen to multiply cleanly and invert
  // without any non-binary-power-of-2 approximation errors.
  const SkMatrix scaled_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Scale(0.5, 2));
  const SkM44 scaled_m44 = SkM44(scaled_matrix);
  const DlMatrix scaled_dl_matrix = dl_matrix.Scale({0.5, 2, 1});
  const SkRect local_cull_rect = SkRect::MakeLTRB(10, 5, 30, 10);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.scale(0.5, 2);
  tracker2.scale(0.5, 2);
  tracker3.scale(0.5, 2);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), scaled_m44);
  EXPECT_EQ(tracker1.matrix(), scaled_dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), scaled_m44);
  EXPECT_EQ(tracker2.matrix(), scaled_dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), scaled_m44);
  EXPECT_EQ(tracker3.matrix(), scaled_dl_matrix);
}

TEST(DisplayListMatrixClipState, Scale) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4.0, 4.0, 1.0});
  // Scale factor carefully chosen to multiply cleanly and invert
  // without any non-binary-power-of-2 approximation errors.
  const SkMatrix scaled_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Scale(0.5, 2));
  const SkM44 scaled_m44 = SkM44(scaled_matrix);
  const DlMatrix scaled_dl_matrix = dl_matrix.Scale({0.5, 2, 1});
  const SkRect local_cull_rect = SkRect::MakeLTRB(10, 5, 30, 10);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.scale(0.5, 2);
  state2.scale(0.5, 2);
  state3.scale(0.5, 2);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(state1.matrix_4x4(), scaled_m44);
  EXPECT_EQ(state1.matrix(), scaled_dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(state2.matrix_4x4(), scaled_m44);
  EXPECT_EQ(state2.matrix(), scaled_dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), scaled_matrix);
  EXPECT_EQ(state3.matrix_4x4(), scaled_m44);
  EXPECT_EQ(state3.matrix(), scaled_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, Skew) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});
  const SkMatrix skewed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Skew(.25, 0));
  const SkM44 skewed_m44 = SkM44(skewed_matrix);
  const DlMatrix skewed_dl_matrix = dl_matrix * DlMatrix::MakeSkew(0.25, 0);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 10, 12.5, 20);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.skew(.25, 0);
  tracker2.skew(.25, 0);
  tracker3.skew(.25, 0);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), skewed_m44);
  EXPECT_EQ(tracker1.matrix(), skewed_dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), skewed_m44);
  EXPECT_EQ(tracker2.matrix(), skewed_dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), skewed_m44);
  EXPECT_EQ(tracker3.matrix(), skewed_dl_matrix);
}

TEST(DisplayListMatrixClipState, Skew) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});
  const SkMatrix skewed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::Skew(.25, 0));
  const SkM44 skewed_m44 = SkM44(skewed_matrix);
  const DlMatrix skewed_dl_matrix = dl_matrix * DlMatrix::MakeSkew(0.25, 0);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 10, 12.5, 20);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.skew(.25, 0);
  state2.skew(.25, 0);
  state3.skew(.25, 0);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(state1.matrix_4x4(), skewed_m44);
  EXPECT_EQ(state1.matrix(), skewed_dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(state2.matrix_4x4(), skewed_m44);
  EXPECT_EQ(state2.matrix(), skewed_dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), skewed_matrix);
  EXPECT_EQ(state3.matrix_4x4(), skewed_m44);
  EXPECT_EQ(state3.matrix(), skewed_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, Rotate) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});
  const SkMatrix rotated_matrix =
      SkMatrix::Concat(matrix, SkMatrix::RotateDeg(90));
  const SkM44 rotated_m44 = SkM44(rotated_matrix);
  const DlMatrix rotated_dl_matrix =
      dl_matrix * DlMatrix::MakeRotationZ(Degrees(90));
  const SkRect local_cull_rect = SkRect::MakeLTRB(10, -15, 20, -5);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.rotate(90);
  tracker2.rotate(90);
  tracker3.rotate(90);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), rotated_m44);
  EXPECT_EQ(tracker1.matrix(), rotated_dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), rotated_m44);
  EXPECT_EQ(tracker2.matrix(), rotated_dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), rotated_m44);
  EXPECT_EQ(tracker3.matrix(), rotated_dl_matrix);
}

TEST(DisplayListMatrixClipState, Rotate) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});
  const SkMatrix rotated_matrix =
      SkMatrix::Concat(matrix, SkMatrix::RotateDeg(90));
  const SkM44 rotated_m44 = SkM44(rotated_matrix);
  const DlMatrix rotated_dl_matrix =
      dl_matrix * DlMatrix::MakeRotationZ(Degrees(90));
  const SkRect local_cull_rect = SkRect::MakeLTRB(10, -15, 20, -5);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.rotate(90);
  state2.rotate(90);
  state3.rotate(90);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(state1.matrix_4x4(), rotated_m44);
  EXPECT_EQ(state1.matrix(), rotated_dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(state2.matrix_4x4(), rotated_m44);
  EXPECT_EQ(state2.matrix(), rotated_dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), rotated_matrix);
  EXPECT_EQ(state3.matrix_4x4(), rotated_m44);
  EXPECT_EQ(state3.matrix(), rotated_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, Transform2DAffine) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkMatrix transformed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::MakeAll(2, 0, 5,  //
                                                 0, 2, 6,  //
                                                 0, 0, 1));
  const SkM44 transformed_m44 = SkM44(transformed_matrix);
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.transform2DAffine(2, 0, 5,  //
                             0, 2, 6);
  tracker2.transform2DAffine(2, 0, 5,  //
                             0, 2, 6);
  tracker3.transform2DAffine(2, 0, 5,  //
                             0, 2, 6);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker1.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker2.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipState, Transform2DAffine) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkMatrix transformed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::MakeAll(2, 0, 5,  //
                                                 0, 2, 6,  //
                                                 0, 0, 1));
  const SkM44 transformed_m44 = SkM44(transformed_matrix);
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.transform2DAffine(2, 0, 5,  //
                           0, 2, 6);
  state2.transform2DAffine(2, 0, 5,  //
                           0, 2, 6);
  state3.transform2DAffine(2, 0, 5,  //
                           0, 2, 6);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state1.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state2.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, TransformFullPerspectiveUsing3x3Matrix) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkMatrix transformed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::MakeAll(2, 0, 5,  //
                                                 0, 2, 6,  //
                                                 0, 0, 1));
  const SkM44 transformed_m44 = SkM44(transformed_matrix);
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  tracker2.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  tracker3.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);

  EXPECT_FALSE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker1.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker2.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(tracker3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipState, TransformFullPerspectiveUsing3x3Matrix) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkMatrix transformed_matrix =
      SkMatrix::Concat(matrix, SkMatrix::MakeAll(2, 0, 5,  //
                                                 0, 2, 6,  //
                                                 0, 0, 1));
  const SkM44 transformed_m44 = SkM44(transformed_matrix);
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 0,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 0,  //
                                  0, 0, 0, 1);
  state2.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 0,  //
                                  0, 0, 0, 1);
  state3.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 0,  //
                                  0, 0, 0, 1);

  EXPECT_FALSE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state1.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state2.matrix(), transformed_dl_matrix);

  EXPECT_FALSE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_3x3(), transformed_matrix);
  EXPECT_EQ(state3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, TransformFullPerspectiveUsing4x4Matrix) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkM44 transformed_m44 = SkM44(m44, SkM44(2, 0, 0, 5,  //
                                                 0, 2, 0, 6,  //
                                                 0, 0, 1, 7,  //
                                                 0, 0, 0, 1));
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 7,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipTracker tracker1(cull_rect, matrix);
  DisplayListMatrixClipTracker tracker2(cull_rect, m44);
  DisplayListMatrixClipTracker tracker3(dl_cull_rect, dl_matrix);
  tracker1.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 7,  //
                                    0, 0, 0, 1);
  tracker2.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 7,  //
                                    0, 0, 0, 1);
  tracker3.transformFullPerspective(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 7,  //
                                    0, 0, 0, 1);

  EXPECT_TRUE(tracker1.using_4x4_matrix());
  EXPECT_EQ(tracker1.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker1.matrix(), transformed_dl_matrix);

  EXPECT_TRUE(tracker2.using_4x4_matrix());
  EXPECT_EQ(tracker2.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker2.matrix(), transformed_dl_matrix);

  EXPECT_TRUE(tracker3.using_4x4_matrix());
  EXPECT_EQ(tracker3.device_cull_rect(), cull_rect);
  EXPECT_EQ(tracker3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(tracker3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(tracker3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipState, TransformFullPerspectiveUsing4x4Matrix) {
  const SkRect cull_rect = SkRect::MakeLTRB(20, 40, 60, 80);
  const DlRect dl_cull_rect = DlRect::MakeLTRB(20, 40, 60, 80);
  const SkMatrix matrix = SkMatrix::Scale(4, 4);
  const SkM44 m44 = SkM44::Scale(4, 4);
  const DlMatrix dl_matrix = DlMatrix::MakeScale({4, 4, 1});

  const SkM44 transformed_m44 = SkM44(m44, SkM44(2, 0, 0, 5,  //
                                                 0, 2, 0, 6,  //
                                                 0, 0, 1, 7,  //
                                                 0, 0, 0, 1));
  const DlMatrix transformed_dl_matrix =
      dl_matrix * DlMatrix::MakeRow(2, 0, 0, 5,  //
                                    0, 2, 0, 6,  //
                                    0, 0, 1, 7,  //
                                    0, 0, 0, 1);
  const SkRect local_cull_rect = SkRect::MakeLTRB(0, 2, 5, 7);

  DisplayListMatrixClipState state1(cull_rect, matrix);
  DisplayListMatrixClipState state2(cull_rect, m44);
  DisplayListMatrixClipState state3(dl_cull_rect, dl_matrix);
  state1.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 7,  //
                                  0, 0, 0, 1);
  state2.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 7,  //
                                  0, 0, 0, 1);
  state3.transformFullPerspective(2, 0, 0, 5,  //
                                  0, 2, 0, 6,  //
                                  0, 0, 1, 7,  //
                                  0, 0, 0, 1);

  EXPECT_TRUE(state1.using_4x4_matrix());
  EXPECT_EQ(state1.device_cull_rect(), cull_rect);
  EXPECT_EQ(state1.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state1.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state1.matrix(), transformed_dl_matrix);

  EXPECT_TRUE(state2.using_4x4_matrix());
  EXPECT_EQ(state2.device_cull_rect(), cull_rect);
  EXPECT_EQ(state2.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state2.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state2.matrix(), transformed_dl_matrix);

  EXPECT_TRUE(state3.using_4x4_matrix());
  EXPECT_EQ(state3.device_cull_rect(), cull_rect);
  EXPECT_EQ(state3.local_cull_rect(), local_cull_rect);
  EXPECT_EQ(state3.matrix_4x4(), transformed_m44);
  EXPECT_EQ(state3.matrix(), transformed_dl_matrix);
}

TEST(DisplayListMatrixClipTracker, ClipDifference) {
  SkRect cull_rect = SkRect::MakeLTRB(20, 20, 40, 40);

  auto non_reducing = [&cull_rect](const SkRect& diff_rect,
                                   const std::string& label) {
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      tracker.clipRect(diff_rect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), cull_rect) << label;
    }
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      const SkRRect diff_rrect = SkRRect::MakeRect(diff_rect);
      tracker.clipRRect(diff_rrect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), cull_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      const SkPath diff_path = SkPath().addRect(diff_rect);
      tracker.clipPath(diff_path, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), cull_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      state.clipRect(diff_rect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), cull_rect) << label;
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      const SkRRect diff_rrect = SkRRect::MakeRect(diff_rect);
      state.clipRRect(diff_rrect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), cull_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      const SkPath diff_path = SkPath().addRect(diff_rect);
      state.clipPath(diff_path, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), cull_rect) << label << " (RRect)";
    }
  };

  auto reducing = [&cull_rect](const SkRect& diff_rect,
                               const SkRect& result_rect,
                               const std::string& label) {
    EXPECT_TRUE(result_rect.isEmpty() || cull_rect.contains(result_rect));
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      tracker.clipRect(diff_rect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), result_rect) << label;
    }
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      const SkRRect diff_rrect = SkRRect::MakeRect(diff_rect);
      tracker.clipRRect(diff_rrect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), result_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
      const SkPath diff_path = SkPath().addRect(diff_rect);
      tracker.clipPath(diff_path, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(tracker.device_cull_rect(), result_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      state.clipRect(diff_rect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), result_rect) << label;
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      const SkRRect diff_rrect = SkRRect::MakeRect(diff_rect);
      state.clipRRect(diff_rrect, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), result_rect) << label << " (RRect)";
    }
    {
      DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
      const SkPath diff_path = SkPath().addRect(diff_rect);
      state.clipPath(diff_path, DlCanvas::ClipOp::kDifference, false);
      EXPECT_EQ(state.device_cull_rect(), result_rect) << label << " (RRect)";
    }
  };

  // Skim the corners and edge
  non_reducing(SkRect::MakeLTRB(10, 10, 20, 20), "outside UL corner");
  non_reducing(SkRect::MakeLTRB(20, 10, 40, 20), "Above");
  non_reducing(SkRect::MakeLTRB(40, 10, 50, 20), "outside UR corner");
  non_reducing(SkRect::MakeLTRB(40, 20, 50, 40), "Right");
  non_reducing(SkRect::MakeLTRB(40, 40, 50, 50), "outside LR corner");
  non_reducing(SkRect::MakeLTRB(20, 40, 40, 50), "Below");
  non_reducing(SkRect::MakeLTRB(10, 40, 20, 50), "outside LR corner");
  non_reducing(SkRect::MakeLTRB(10, 20, 20, 40), "Left");

  // Overlap corners
  non_reducing(SkRect::MakeLTRB(15, 15, 25, 25), "covering UL corner");
  non_reducing(SkRect::MakeLTRB(35, 15, 45, 25), "covering UR corner");
  non_reducing(SkRect::MakeLTRB(35, 35, 45, 45), "covering LR corner");
  non_reducing(SkRect::MakeLTRB(15, 35, 25, 45), "covering LL corner");

  // Overlap edges, but not across an entire side
  non_reducing(SkRect::MakeLTRB(20, 15, 39, 25), "Top edge left-biased");
  non_reducing(SkRect::MakeLTRB(21, 15, 40, 25), "Top edge, right biased");
  non_reducing(SkRect::MakeLTRB(35, 20, 45, 39), "Right edge, top-biased");
  non_reducing(SkRect::MakeLTRB(35, 21, 45, 40), "Right edge, bottom-biased");
  non_reducing(SkRect::MakeLTRB(20, 35, 39, 45), "Bottom edge, left-biased");
  non_reducing(SkRect::MakeLTRB(21, 35, 40, 45), "Bottom edge, right-biased");
  non_reducing(SkRect::MakeLTRB(15, 20, 25, 39), "Left edge, top-biased");
  non_reducing(SkRect::MakeLTRB(15, 21, 25, 40), "Left edge, bottom-biased");

  // Slice all the way through the middle
  non_reducing(SkRect::MakeLTRB(25, 15, 35, 45), "Vertical interior slice");
  non_reducing(SkRect::MakeLTRB(15, 25, 45, 35), "Horizontal interior slice");

  // Slice off each edge
  reducing(SkRect::MakeLTRB(20, 15, 40, 25),  //
           SkRect::MakeLTRB(20, 25, 40, 40),  //
           "Slice off top");
  reducing(SkRect::MakeLTRB(35, 20, 45, 40),  //
           SkRect::MakeLTRB(20, 20, 35, 40),  //
           "Slice off right");
  reducing(SkRect::MakeLTRB(20, 35, 40, 45),  //
           SkRect::MakeLTRB(20, 20, 40, 35),  //
           "Slice off bottom");
  reducing(SkRect::MakeLTRB(15, 20, 25, 40),  //
           SkRect::MakeLTRB(25, 20, 40, 40),  //
           "Slice off left");

  // cull rect contains diff rect
  non_reducing(SkRect::MakeLTRB(21, 21, 39, 39), "Contained, non-covering");

  // cull rect equals diff rect
  reducing(cull_rect, SkRect::MakeEmpty(), "Perfectly covering");

  // diff rect contains cull rect
  reducing(SkRect::MakeLTRB(15, 15, 45, 45), SkRect::MakeEmpty(), "Smothering");
}

TEST(DisplayListMatrixClipTracker, ClipPathWithInvertFillType) {
  SkRect cull_rect = SkRect::MakeLTRB(0, 0, 100.0, 100.0);
  DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());
  SkPath clip = SkPath().addCircle(10.2, 11.3, 2).addCircle(20.4, 25.7, 2);
  clip.setFillType(SkPathFillType::kInverseWinding);
  tracker.clipPath(clip, DlCanvas::ClipOp::kIntersect, false);

  EXPECT_EQ(tracker.local_cull_rect(), cull_rect);
  EXPECT_EQ(tracker.device_cull_rect(), cull_rect);
}

TEST(DisplayListMatrixClipState, ClipPathWithInvertFillType) {
  SkRect cull_rect = SkRect::MakeLTRB(0, 0, 100.0, 100.0);
  DisplayListMatrixClipState state(cull_rect, SkMatrix::I());
  SkPath clip = SkPath().addCircle(10.2, 11.3, 2).addCircle(20.4, 25.7, 2);
  clip.setFillType(SkPathFillType::kInverseWinding);
  state.clipPath(clip, DlCanvas::ClipOp::kIntersect, false);

  EXPECT_EQ(state.local_cull_rect(), cull_rect);
  EXPECT_EQ(state.device_cull_rect(), cull_rect);
}

TEST(DisplayListMatrixClipTracker, DiffClipPathWithInvertFillType) {
  SkRect cull_rect = SkRect::MakeLTRB(0, 0, 100.0, 100.0);
  DisplayListMatrixClipTracker tracker(cull_rect, SkMatrix::I());

  SkPath clip = SkPath().addCircle(10.2, 11.3, 2).addCircle(20.4, 25.7, 2);
  clip.setFillType(SkPathFillType::kInverseWinding);
  SkRect clip_bounds = SkRect::MakeLTRB(8.2, 9.3, 22.4, 27.7);
  tracker.clipPath(clip, DlCanvas::ClipOp::kDifference, false);

  EXPECT_EQ(tracker.local_cull_rect(), clip_bounds);
  EXPECT_EQ(tracker.device_cull_rect(), clip_bounds);
}

TEST(DisplayListMatrixClipState, DiffClipPathWithInvertFillType) {
  SkRect cull_rect = SkRect::MakeLTRB(0, 0, 100.0, 100.0);
  DisplayListMatrixClipState state(cull_rect, SkMatrix::I());

  SkPath clip = SkPath().addCircle(10.2, 11.3, 2).addCircle(20.4, 25.7, 2);
  clip.setFillType(SkPathFillType::kInverseWinding);
  SkRect clip_bounds = SkRect::MakeLTRB(8.2, 9.3, 22.4, 27.7);
  state.clipPath(clip, DlCanvas::ClipOp::kDifference, false);

  EXPECT_EQ(state.local_cull_rect(), clip_bounds);
  EXPECT_EQ(state.device_cull_rect(), clip_bounds);
}

}  // namespace testing
}  // namespace flutter
