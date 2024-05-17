// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_TYPOGRAPHER_FONT_GLYPH_PAIR_H_
#define FLUTTER_IMPELLER_TYPOGRAPHER_FONT_GLYPH_PAIR_H_

#include <unordered_map>
#include <unordered_set>

#include "impeller/typographer/font.h"
#include "impeller/typographer/glyph.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      A font and a scale.  Used as a key that represents a typeface
///             within a glyph atlas.
///
struct ScaledFont {
  Font font;
  Scalar scale;
};

using FontGlyphMap = std::unordered_map<ScaledFont, std::unordered_set<Glyph>>;

//------------------------------------------------------------------------------
/// @brief      A font along with a glyph in that font rendered at a particular
///             scale.
///
struct FontGlyphPair {
  FontGlyphPair(const ScaledFont& sf, const Glyph& g)
      : scaled_font(sf), glyph(g) {}
  const ScaledFont& scaled_font;
  const Glyph& glyph;
};

}  // namespace impeller

template <>
struct std::hash<impeller::ScaledFont> {
  constexpr std::size_t operator()(const impeller::ScaledFont& sf) const {
    return fml::HashCombine(sf.font.GetHash(), sf.scale);
  }
};

template <>
struct std::equal_to<impeller::ScaledFont> {
  constexpr bool operator()(const impeller::ScaledFont& lhs,
                            const impeller::ScaledFont& rhs) const {
    return lhs.font.IsEqual(rhs.font) && lhs.scale == rhs.scale;
  }
};

#endif  // FLUTTER_IMPELLER_TYPOGRAPHER_FONT_GLYPH_PAIR_H_
