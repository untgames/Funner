// This code is in the public domain -- castanyo@yahoo.es

#ifndef NV_IMAGE_COLORBLOCK_H
#define NV_IMAGE_COLORBLOCK_H

#include "color.h"

namespace nv
{
  /// Uncompressed 4x4 color block.
  struct ColorBlock
  {
    ColorBlock();
    ColorBlock(const uint * linearImage);
    ColorBlock(const ColorBlock & block);
    
    // Accessors
    const Color32 * colors() const;

    Color32 color(uint i) const;
    Color32 & color(uint i);
    
    Color32 color(uint x, uint y) const;
    Color32 & color(uint x, uint y);
    
  private:
    
    Color32 m_color[4*4];
    
  };
  

  /// Get pointer to block colors.
  inline const Color32 * ColorBlock::colors() const
  {
    return m_color;
  }
  
  /// Get block color.
  inline Color32 ColorBlock::color(uint i) const
  {
    return m_color[i];
  }
  
  /// Get block color.
  inline Color32 & ColorBlock::color(uint i)
  {
    return m_color[i];
  }
  
  /// Get block color.
  inline Color32 ColorBlock::color(uint x, uint y) const
  {
    return m_color[y * 4 + x];
  }
  
  /// Get block color.
  inline Color32 & ColorBlock::color(uint x, uint y)
  {
    return m_color[y * 4 + x];
  }
  
} // nv namespace

#endif // NV_IMAGE_COLORBLOCK_H
