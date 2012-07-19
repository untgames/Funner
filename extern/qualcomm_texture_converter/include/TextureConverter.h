/*=============================================================================
FILE: TextureConverter.h

DESCRIPTION: Interface to Qualcomm Texture Conversion and Compression Library

AUTHOR:			QUALCOMM
                        
                Copyright (c) 2007,2008 QUALCOMM Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
					   Advanced Content Group (ACG)
=============================================================================*/

#ifndef _TEXTURECONVERTER_H_
#define _TEXTURECONVERTER_H_

#ifdef __cplusplus
extern "C" 
{
#endif

//=============================================================================
// CONSTS/ENUMS
//=============================================================================

//
// TQonvertImage.nFormat: Image format 
//
enum 
{
    Q_FORMAT_RGBA_8888 = 1,
    Q_FORMAT_RGB_888,
    Q_FORMAT_RGB_565,
    Q_FORMAT_RGBA_5551,
    Q_FORMAT_RGBA_4444,
    Q_FORMAT_LUMINANCE_ALPHA_88,
    Q_FORMAT_LUMINANCE_8,
    Q_FORMAT_ALPHA_8,
    Q_FORMAT_RGB_16,
    Q_FORMAT_RGBA_16,
    Q_FORMAT_PALETTE_8_RGBA_8888,
    Q_FORMAT_PALETTE_8_RGB_888,
    Q_FORMAT_PALETTE_8_RGBA_5551,
    Q_FORMAT_PALETTE_8_RGBA_4444,
    Q_FORMAT_PALETTE_8_RGB_565,
    Q_FORMAT_PALETTE_4_RGBA_8888,
    Q_FORMAT_PALETTE_4_RGB_888,
    Q_FORMAT_PALETTE_4_RGBA_5551,
    Q_FORMAT_PALETTE_4_RGBA_4444,
    Q_FORMAT_PALETTE_4_RGB_565,
    Q_FORMAT_PALETTE_1_RGBA_8888,
    Q_FORMAT_ATITC_RGBA,
    Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA        = Q_FORMAT_ATITC_RGBA,
    Q_FORMAT_ATITC_RGB,
    Q_FORMAT_ATC_RGB                        = Q_FORMAT_ATITC_RGB,
    Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA,
    Q_FORMAT_ETC1_RGB8,
    Q_FORMAT_3DC_X,
    Q_FORMAT_3DC_XY,
    Q_FORMAT_RGBA_2101010,                  //TODO: NOT IMPLEMENTED YET
    Q_FORMAT_RGBA_F,
    Q_FORMAT_RGB_F,
    Q_FORMAT_LUMINANCE_ALPHA_F,
    Q_FORMAT_LUMINANCE_F,
    Q_FORMAT_ALPHA_F,
    Q_FORMAT_RGBA_HF,
    Q_FORMAT_RGB_HF,
    Q_FORMAT_LUMINANCE_ALPHA_HF,
    Q_FORMAT_LUMINANCE_HF,
    Q_FORMAT_ALPHA_HF,
    Q_FORMAT_BGRA_8888,
    Q_FORMAT_DEPTH_16,
    Q_FORMAT_DEPTH_24,
    Q_FORMAT_DEPTH_24_STENCIL_8,
    Q_FORMAT_DEPTH_32,
};

//
// TFormatFlags.nEncodeFlag: Image encoding flags 
//
enum TEncodeFlag
{
    Q_FLAG_ENCODE_NONE = 0,
    Q_FLAG_ENCODE_DEFAULT = 0,
    Q_FLAG_ENCODE_ATITC_FAST,
};

//
// TFormatFlags.nScaleFilter: Scaling option to use when creating scaled output (for example for mipmap generation)
//
enum TScaleFilterFlag
{
    Q_FLAG_SCALEFILTER_DEFAULT = 0,
    Q_FLAG_SCALEFILTER_NEAREST,
    Q_FLAG_SCALEFILTER_MEAN, 
    Q_FLAG_SCALEFILTER_BILINEAR,
    Q_FLAG_SCALEFILTER_BICUBIC,
    Q_FLAG_SCALEFILTER_KAISER,
};

//
// TFormatFlags.nNormalMap: Describes the algorithm to use for creating normal maps (for bumpmapping)
//
enum TNormalMapFlag
{
    Q_FLAG_NORMALMAP_NONE = 0,
    Q_FLAG_NORMALMAP_ROBERTSCROSS,
    Q_FLAG_NORMALMAP_SOBEL,
    Q_FLAG_NORMALMAP_PREWITTGRADIENT,
};

//
// TFormatFlags.nDebugFlag: Debug options
//
enum TDebugFlags
{
    Q_FLAG_DEBUG_DEFAULT         = 0,
    Q_FLAG_DEBUG_DISABLE_VERBOSE = 0x00000001,  // Disable verbose error output to stderr
    Q_FLAG_DEBUG_VERSION         = 0x00000002,  // Return the library version (MAJOR<<16 | MINOR) as an unsigned int in pOutput->pData
};

//
// Qonvert function Success and Error return values
//
enum TReturnCode
{
    Q_SUCCESS = 0,
    Q_ERROR_UNSUPPORTED_DIMENSIONS,
    Q_ERROR_UNSUPPORTED_SRC_FORMAT,
    Q_ERROR_UNSUPPORTED_DST_FORMAT,
    Q_ERROR_UNSUPPORTED_SRC_FORMAT_FLAG,
    Q_ERROR_UNSUPPORTED_DST_FORMAT_FLAG,
    Q_ERROR_INCORRECT_SRC_PARAMETER,
    Q_ERROR_INCORRECT_DST_PARAMETER,
    Q_ERROR_INCORRECT_DATASIZE,
    Q_ERROR_OTHER,
};

//=============================================================================
// TYPEDEFS 
//=============================================================================

//
// Additional format flags, leaving any value to 0 means the library will use a proper default value
//
typedef struct _TFormatFlags
{
    unsigned int nStride;           // Specify if the image stride is different then the default (bpp*width)
    unsigned int nMaskRed;          // Specify which of the bits in a color are red, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskGreen;        // Specify which of the bits in a color are green, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskBlue;         // Specify which of the bits in a color are blue, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskAlpha;        // Specify which of the bits in a color are alpha, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nFlipX;            // Flip image horizontally
    unsigned int nFlipY;            // Flip image vertically
    unsigned int nEncodeFlag;       // Specifying image encoding options (see TEncodeFlag above)
    unsigned int nScaleFilterFlag;  // TScaleFilterFlag specifying the scale filter used when scaling the source image to the destination image (for mipmapping)
    unsigned int nNormalMapFlag;    // TNormalMapFlag specify if a normal map should be created (for destination images only) 
    unsigned int nNormalMapScale;   // Specify the scale when creating a normalmap (0 = no scaling)
    unsigned int nNormalMapWrap;    // Specify if wrapping is enabled when creating a normalmap (0 = clamp to border, 1 = wrap)
    unsigned int nDebugFlags;       // Debug flags (logical OR TDebugFlags)
} TFormatFlags;

//
// Basic image format description
//
typedef struct _TQonvertImage
{
    unsigned int    nWidth;          // Image width
    unsigned int    nHeight;         // Image height
    unsigned int    nFormat;         // Image format, see Q_FORMAT_xxx enums above.
    TFormatFlags*   pFormatFlags;    // Points to additional image format flags, can be NULL (will assume default values).
    unsigned int    nDataSize;       // Size of the pData buffer
    unsigned char*  pData;           // Image data
} TQonvertImage;

//=============================================================================
// FUNCTION PROTOTYPES
//=============================================================================

//-----------------------------------------------------------------------------
// Qonvert
//
// Convert the input texture to the output texture format. Typically used to convert
// simple image formats into OpenGL ES supported compression formats, like ATITC
// supported on the Conversion Platform MSM series chips.
//
// If pOutput->pData is set to NULL, the pOutput->nDataSize will be calculated based 
// on the pOutput format information. With this information the application is expected
// to create a memory block for pOutput->pData. 
// 
// Return: 
//   0     == SUCCESS
//   other == ERROR CODE
//-----------------------------------------------------------------------------
extern unsigned int Qonvert( 
    const TQonvertImage*    pSrcImg,  // source image
    TQonvertImage*          pDstImg   // destination image
);

#ifdef __cplusplus
};
#endif

#endif  //_TEXTURECONVERTER_H_
