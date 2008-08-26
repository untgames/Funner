// This code is in the public domain -- castanyo@yahoo.es

#include "ColorBlock.h"

using namespace nv;

namespace {
  
  // Get approximate luminance.
  inline static uint colorLuminance(Color32 c)
  {
    return c.r + c.g + c.b;
  }
  
  // Get the euclidean distance between the given colors.
  inline static uint colorDistance(Color32 c0, Color32 c1)
  {
    return (c0.r - c1.r) * (c0.r - c1.r) + (c0.g - c1.g) * (c0.g - c1.g) + (c0.b - c1.b) * (c0.b - c1.b);
  }
  
} // namespace`


/// Default constructor.
ColorBlock::ColorBlock()
{
}

/// Init the color block from an array of colors.
ColorBlock::ColorBlock(const uint * linearImage)
{
  for(uint i = 0; i < 16; i++) {
    color(i) = Color32(linearImage[i]);
  }
}

/// Init the color block with the contents of the given block.
ColorBlock::ColorBlock(const ColorBlock & block)
{
  for(uint i = 0; i < 16; i++) {
    color(i) = block.color(i);
  }
}
  