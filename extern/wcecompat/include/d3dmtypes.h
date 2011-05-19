//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
// This is a prerelease header file and subject to change.
//
// d3dmtypes.h
// Direct3D Mobile Data Types Header File
//

#pragma once

// Generic 32 bit value type.

typedef signed int D3DMVALUE;

#ifdef __cplusplus

// Inline functions for conversion of input values to the D3DMVALUE type.

// The typedefs in each of the functions is a compile time assert that
// the various types we are converting are all the same size. If they are
// not the same size, the == operator will return 0, which causes the
// compiler to attempt to typedef an array of size 0, which is an error.

inline D3DMVALUE D3DM_MAKE_D3DMVALUE(D3DMVALUE Value)
{
	return (Value);
}

inline D3DMVALUE D3DM_MAKE_D3DMVALUE(float Value)
{
	typedef int float_and_D3DMVALUE_arent_of_the_same_size[sizeof(D3DMVALUE) == sizeof(float)];
	return *reinterpret_cast<D3DMVALUE *>(&Value);
}

inline D3DMVALUE D3DM_MAKE_D3DMVALUE(unsigned int Value)
{
	typedef int uint_and_D3DMVALUE_arent_of_the_same_size[sizeof(D3DMVALUE) == sizeof(unsigned int)];
	return *reinterpret_cast<D3DMVALUE *>(&Value);
}

inline D3DMVALUE D3DM_MAKE_D3DMVALUE(double Value)
{
	float FloatValue = (float)Value;
	return *reinterpret_cast<D3DMVALUE *>(&FloatValue);
}

#endif

// Color representation.

// A D3DMCOLOR structure represnets a color using the D3DMFMT_A8R8G8B8 format.

typedef ULONG D3DMCOLOR;

// Maps unsigned 8 bits/channel to D3DMCOLOR.

#define D3DMCOLOR_ARGB(a,r,g,b) \
    ((D3DMCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DMCOLOR_RGBA(r,g,b,a) D3DMCOLOR_ARGB(a,r,g,b)
#define D3DMCOLOR_XRGB(r,g,b)   D3DMCOLOR_ARGB(0xff,r,g,b)

// D3DMCOLORVALUE is used for lighting parameters that may be under or over
// saturated. Potentially fixed point or floating point values.

typedef struct _D3DMCOLORVALUE {
    D3DMVALUE r;
    D3DMVALUE g;
    D3DMVALUE b;
    D3DMVALUE a;
} D3DMCOLORVALUE;

// Basic matmatics types. Potentially fixed point or floating point values.

typedef struct _D3DMVECTOR {
    D3DMVALUE x;
    D3DMVALUE y;
    D3DMVALUE z;
} D3DMVECTOR;

typedef struct _D3DMMATRIX {
    union {
        struct {
            D3DMVALUE        _11, _12, _13, _14;
            D3DMVALUE        _21, _22, _23, _24;
            D3DMVALUE        _31, _32, _33, _34;
            D3DMVALUE        _41, _42, _43, _44;

        };
        D3DMVALUE m[4][4];
    };
} D3DMMATRIX;

// Viewport definition structure.

typedef struct _D3DMVIEWPORT {
    ULONG   X;
    ULONG   Y;            // Viewport Top left
    ULONG   Width;
    ULONG   Height;       // Viewport Dimensions
    float   MinZ;         // Min/max of clip Volume
    float   MaxZ;
} D3DMVIEWPORT;

// Geometry clipping.

// The following bits are used in the ClipUnion and ClipIntersection
// members of the D3DMCLIPSTATUS

#define D3DMCS_LEFT        0x00000001
#define D3DMCS_RIGHT       0x00000002
#define D3DMCS_TOP         0x00000004
#define D3DMCS_BOTTOM      0x00000008
#define D3DMCS_FRONT       0x00000010
#define D3DMCS_BACK        0x00000020

#define D3DMCS_ALL (D3DMCS_LEFT   | \
                    D3DMCS_RIGHT  | \
                    D3DMCS_TOP    | \
                    D3DMCS_BOTTOM | \
                    D3DMCS_FRONT  | \
                    D3DMCS_BACK)

typedef struct _D3DMCLIPSTATUS {
    ULONG ClipUnion;
    ULONG ClipIntersection;
} D3DMCLIPSTATUS;

// Light and material descrtiption.

typedef struct _D3DMMATERIAL {
    D3DMCOLORVALUE  Diffuse;        // Diffuse color RGBA
    D3DMCOLORVALUE  Ambient;        // Ambient color RGB
    D3DMCOLORVALUE  Specular;       // Specular 'shininess'
    float           Power;          // Sharpness if specular highlight
} D3DMMATERIAL;

typedef enum _D3DMLIGHTTYPE {
    D3DMLIGHT_POINT          = 1,
    D3DMLIGHT_DIRECTIONAL    = 3,
    D3DMLIGHT_FORCE_ULONG    = 0x7fffffff, // force 32-bit size enum
} D3DMLIGHTTYPE;

typedef struct _D3DMLIGHT {
    D3DMLIGHTTYPE    Type;          // Type of light source
    D3DMCOLORVALUE   Diffuse;       // Diffuse color of light
    D3DMCOLORVALUE   Specular;      // Specular color of light
    D3DMCOLORVALUE   Ambient;       // Ambient color of light
    D3DMVECTOR       Position;      // Position in world space
    D3DMVECTOR       Direction;     // Direction in world space
    float            Range;         // Cutoff range
    float            Attenuation0;  // Constant attenuation
    float            Attenuation1;  // Linear attenuation
    float            Attenuation2;  // Quadratic attenuation
} D3DMLIGHT;

// Options for clearing.

#define D3DMCLEAR_TARGET   0x00000001  // Clear target surface
#define D3DMCLEAR_ZBUFFER  0x00000002  // Clear target z buffer
#define D3DMCLEAR_STENCIL  0x00000004  // Clear stencil planes

#define D3DMCLEAR_VALID    0x00000007

// The following enumerations define possible values for many of the render
// states.

typedef enum _D3DMSHADEMODE {
    D3DMSHADE_FLAT               = 1,
    D3DMSHADE_GOURAUD            = 2,
    D3DMSHADE_FORCE_ULONG        = 0x7fffffff, // force 32-bit size enum
} D3DMSHADEMODE;

typedef enum _D3DMFILLMODE {
    D3DMFILL_POINT               = 1,
    D3DMFILL_WIREFRAME           = 2,
    D3DMFILL_SOLID               = 3,
    D3DMFILL_FORCE_ULONG         = 0x7fffffff, // force 32-bit size enum
} D3DMFILLMODE;

typedef enum _D3DMBLEND {
    D3DMBLEND_ZERO               = 1,
    D3DMBLEND_ONE                = 2,
    D3DMBLEND_SRCCOLOR           = 3,
    D3DMBLEND_INVSRCCOLOR        = 4,
    D3DMBLEND_SRCALPHA           = 5,
    D3DMBLEND_INVSRCALPHA        = 6,
    D3DMBLEND_DESTALPHA          = 7,
    D3DMBLEND_INVDESTALPHA       = 8,
    D3DMBLEND_DESTCOLOR          = 9,
    D3DMBLEND_INVDESTCOLOR       = 10,
    D3DMBLEND_SRCALPHASAT        = 11,
    D3DMBLEND_FORCE_ULONG        = 0x7fffffff, // force 32-bit size enum
} D3DMBLEND;

typedef enum _D3DMBLENDOP {
    D3DMBLENDOP_ADD              = 1,
    D3DMBLENDOP_SUBTRACT         = 2,
    D3DMBLENDOP_REVSUBTRACT      = 3,
    D3DMBLENDOP_MIN              = 4,
    D3DMBLENDOP_MAX              = 5,
    D3DMBLENDOP_FORCE_ULONG      = 0x7fffffff, // force 32-bit size enum
} D3DMBLENDOP;

typedef enum _D3DMCULL {
    D3DMCULL_NONE                = 1,
    D3DMCULL_CW                  = 2,
    D3DMCULL_CCW                 = 3,
    D3DMCULL_FORCE_ULONG         = 0x7fffffff, // force 32-bit size enum
} D3DMCULL;

typedef enum _D3DMCMPFUNC {
    D3DMCMP_NEVER                = 1,
    D3DMCMP_LESS                 = 2,
    D3DMCMP_EQUAL                = 3,
    D3DMCMP_LESSEQUAL            = 4,
    D3DMCMP_GREATER              = 5,
    D3DMCMP_NOTEQUAL             = 6,
    D3DMCMP_GREATEREQUAL         = 7,
    D3DMCMP_ALWAYS               = 8,
    D3DMCMP_FORCE_ULONG          = 0x7fffffff, // force 32-bit size enum
} D3DMCMPFUNC;

typedef enum _D3DMSTENCILOP {
    D3DMSTENCILOP_KEEP           = 1,
    D3DMSTENCILOP_ZERO           = 2,
    D3DMSTENCILOP_REPLACE        = 3,
    D3DMSTENCILOP_INCRSAT        = 4,
    D3DMSTENCILOP_DECRSAT        = 5,
    D3DMSTENCILOP_INVERT         = 6,
    D3DMSTENCILOP_INCR           = 7,
    D3DMSTENCILOP_DECR           = 8,
    D3DMSTENCILOP_FORCE_ULONG    = 0x7fffffff, // force 32-bit size enum
} D3DMSTENCILOP;

typedef enum _D3DMFOGMODE {
    D3DMFOG_NONE                 = 0,
    D3DMFOG_EXP                  = 1,
    D3DMFOG_EXP2                 = 2,
    D3DMFOG_LINEAR               = 3,
    D3DMFOG_FORCE_ULONG          = 0x7fffffff, // force 32-bit size enum
} D3DMFOGMODE;

typedef enum _D3DMZBUFFERTYPE {
    D3DMZB_FALSE                 = 0,
    D3DMZB_TRUE                  = 1, // Z buffering
    D3DMZB_USEW                  = 2, // W buffering
    D3DMZB_FORCE_DWORD           = 0x7fffffff, // force 32-bit size enum
} D3DMZBUFFERTYPE;

typedef enum _D3DMMATERIALCOLORSOURCE
{
    D3DMMCS_MATERIAL         = 0,            // Color from material is used
    D3DMMCS_COLOR1           = 1,            // Diffuse vertex color is used
    D3DMMCS_COLOR2           = 2,            // Specular vertex color is used
    D3DMMCS_FORCE_ULONG      = 0x7fffffff,   // force 32-bit size enum
} D3DMMATERIALCOLORSOURCE;

// Add this to a texture stage number to get the wrapping RS for that stage.
// Eg. 0 + D3DMRENDERSTATE_WRAPBIAS = D3DMRS_WRAP0

#define D3DMRENDERSTATE_WRAPBIAS 32

// Flags to construct the D3DMRS_WRAP* render states

#define D3DMWRAPCOORD_0   0x00000001
#define D3DMWRAPCOORD_1   0x00000002

// Flags to construct D3DMRS_COLORWRITEENABLE

#define D3DMCOLORWRITEENABLE_RED     0x00000001
#define D3DMCOLORWRITEENABLE_GREEN   0x00000002
#define D3DMCOLORWRITEENABLE_BLUE    0x00000004
#define D3DMCOLORWRITEENABLE_ALPHA   0x00000008

#define D3DMCOLORWRITEENABLE_ALL     0x0000000F

// Primitives supported by draw-primitive API

typedef enum _D3DMPRIMITIVETYPE {
    D3DMPT_POINTLIST             = 1,
    D3DMPT_LINELIST              = 2,
    D3DMPT_LINESTRIP             = 3,
    D3DMPT_TRIANGLELIST          = 4,
    D3DMPT_TRIANGLESTRIP         = 5,
    D3DMPT_TRIANGLEFAN           = 6,
    D3DMPT_FORCE_ULONG           = 0x7fffffff, // force 32-bit size enum
} D3DMPRIMITIVETYPE;

typedef enum _D3DMTRANSFORMSTATETYPE {
    D3DMTS_WORLD         = 0,
    D3DMTS_VIEW          = 1,
    D3DMTS_PROJECTION    = 2,
    D3DMTS_TEXTURE0      = 3,
    D3DMTS_TEXTURE1      = 4,
    D3DMTS_TEXTURE2      = 5,
    D3DMTS_TEXTURE3      = 6,
    D3DMTS_NUMTRANSFORM  = 7,
    D3DMTS_FORCE_ULONG   = 0x7fffffff, // force 32-bit size enum
} D3DMTRANSFORMSTATETYPE;

typedef enum _D3DMRENDERSTATETYPE {
    D3DMRS_FILLMODE                  = 0,    // D3DMFILLMODE
    D3DMRS_SHADEMODE                 = 1,    // D3DMSHADEMODE
    D3DMRS_ZWRITEENABLE              = 2,    // TRUE to enable z writes
    D3DMRS_ALPHATESTENABLE           = 3,    // TRUE to enable alpha tests
    D3DMRS_LASTPIXEL                 = 4,    // TRUE for last-pixel on lines
    D3DMRS_SRCBLEND                  = 5,    // D3DMBLEND - Source in BLENDOP
    D3DMRS_DESTBLEND                 = 6,    // D3DMBLEND - Dest in BLENDOP
    D3DMRS_CULLMODE                  = 7,    // D3DMCULL
    D3DMRS_ZFUNC                     = 8,    // D3DMCMPFUNC
    D3DMRS_ALPHAREF                  = 9,    // ULONG
    D3DMRS_ALPHAFUNC                 = 10,   // D3DMCMPFUNC
    D3DMRS_DITHERENABLE              = 11,   // TRUE to enable dithering
    D3DMRS_ALPHABLENDENABLE          = 12,   // TRUE to enable alpha blending
    D3DMRS_FOGENABLE                 = 13,   // TRUE to enable fog blending
    D3DMRS_SPECULARENABLE            = 14,   // TRUE to enable specular
    D3DMRS_FOGCOLOR                  = 15,   // D3DMCOLOR
    D3DMRS_FOGTABLEMODE              = 16,   // D3DMFOGMODE
    D3DMRS_FOGSTART                  = 17,   // Fog start (D3DMVALUE)
    D3DMRS_FOGEND                    = 18,   // Fog end (D3DMVALUE)
    D3DMRS_FOGDENSITY                = 19,   // Fog density (D3DMVALUE)
    D3DMRS_DEPTHBIAS                 = 20,   // Depth bias (float)
    D3DMRS_RANGEFOGENABLE            = 21,   // TRUE to enable range-based fog
    D3DMRS_STENCILENABLE             = 22,   // TRUE to enable stenciling
    D3DMRS_STENCILFAIL               = 23,   // D3DMSTENCILOP on test fail
    D3DMRS_STENCILZFAIL              = 24,   // D3DMSTENCILOP on Z test fail
    D3DMRS_STENCILPASS               = 25,   // D3DMSTENCILOP on pass
    D3DMRS_STENCILFUNC               = 26,   // D3DMCMPFUNC
    D3DMRS_STENCILREF                = 27,   // ULONG
    D3DMRS_STENCILMASK               = 28,   // ULONG
    D3DMRS_STENCILWRITEMASK          = 29,   // ULONG
    D3DMRS_TEXTUREFACTOR             = 30,   // D3DMCOLOR
	D3DMRS_TEXTUREPERSPECTIVE        = 31,   // BOOL
    D3DMRS_WRAP0                     = 32,   // D3DMWRAPCOORD_* flags
    D3DMRS_WRAP1                     = 33,   // D3DMWRAPCOORD_* flags  
    D3DMRS_WRAP2                     = 34,   // D3DMWRAPCOORD_* flags  
    D3DMRS_WRAP3                     = 35,   // D3DMWRAPCOORD_* flags
    D3DMRS_CLIPPING                  = 36,   // TRUE to enable prim clipping
    D3DMRS_LIGHTING                  = 37,   // TRUE to enable lighting
    D3DMRS_AMBIENT                   = 38,   // D3DMCOLOR
    D3DMRS_FOGVERTEXMODE             = 39,   // D3DMFOGMODE
    D3DMRS_COLORVERTEX               = 40,   // TRUE to enable per-vertex color
    D3DMRS_LOCALVIEWER               = 41,   // BOOL
    D3DMRS_NORMALIZENORMALS          = 42,   // BOOL
    D3DMRS_DIFFUSEMATERIALSOURCE     = 43,   // D3DMMATERIALCOLORSOURCE
    D3DMRS_SPECULARMATERIALSOURCE    = 44,   // D3DMMATERIALCOLORSOURCE
    D3DMRS_AMBIENTMATERIALSOURCE     = 45,   // D3DMMATERIALCOLORSOURCE
    D3DMRS_COLORWRITEENABLE          = 46,   // D3DMCOLORWRITEENABLE_* flags
    D3DMRS_BLENDOP                   = 47,   // D3DMBLENDOP
    D3DMRS_ZENABLE                   = 48,   // D3DMZBUFFERTYPE
    D3DMRS_SLOPESCALEDEPTHBIAS       = 49,   // Depth bias (float)
    D3DM_MAXRENDERSTATES             = 50,   // Total number of render states

    D3DMRS_FORCE_ULONG               = 0x7fffffff, // force 32-bit size enum

} D3DMRENDERSTATETYPE;

// This macro is used to convert types to ULONG to pass as a render state.
// The value of the variable is not changed.

#define D3DM_MAKE_RSVALUE(Value) (*(ULONG *)(&(Value)))

// Currently, Direct3D Mobile supports up to 4 texture stages per pass.

#define D3DM_TEXSTAGECOUNT 4

// Texture stage information (one of these is held per stage.)

typedef enum _D3DMTEXTURESTAGESTATETYPE
{
    D3DMTSS_COLOROP        =  1, // D3DMTEXTUREOP
    D3DMTSS_COLORARG1      =  2, // D3DMTA_* flags
    D3DMTSS_COLORARG2      =  3, // D3DMTA_* flags
    D3DMTSS_ALPHAOP        =  4, // D3DMTEXTUREOP
    D3DMTSS_ALPHAARG1      =  5, // D3DMTA_* flags
    D3DMTSS_ALPHAARG2      =  6, // D3DMTA_* flags
    D3DMTSS_TEXCOORDINDEX  =  7, // index ULONG | D3DMTSS_TCI_* flags
    D3DMTSS_ADDRESSU       =  8, // D3DMTEXTUREADDRESS for U coordinate
    D3DMTSS_ADDRESSV       =  9, // D3DMTEXTUREADDRESS for V coordinate
    D3DMTSS_BORDERCOLOR    = 10, // D3DMCOLOR
    D3DMTSS_MAGFILTER      = 11, // D3DMTEXTUREFILTER
    D3DMTSS_MINFILTER      = 12, // D3DMTEXTUREFILTER
    D3DMTSS_MIPFILTER      = 13, // D3DMTEXTUREFILTER
    D3DMTSS_MIPMAPLODBIAS  = 14, // D3DMVALUE
    D3DMTSS_MAXMIPLEVEL    = 15, // ULONG
    D3DMTSS_MAXANISOTROPY  = 16, // ULONG
    D3DMTSS_TEXTURETRANSFORMFLAGS = 17, // D3DMTEXTURETRANSFORMFLAGS
    D3DMTSS_ADDRESSW       = 18, // D3DMTEXTUREADDRESS for W coordinate
    D3DMTSS_COLORARG0      = 19, // D3DMTA_* third arg for triadic ops
    D3DMTSS_ALPHAARG0      = 20, // D3DMTA_* third arg for triadic ops
    D3DMTSS_RESULTARG      = 21, // D3DMTA_* arg for result (CURRENT or TEMP)
    D3DMTSS_MAXTEXTURESTATES = 22, // Total number of texture stage states.
    D3DMTSS_FORCE_ULONG    = 0x7fffffff, // force 32-bit size enum
} D3DMTEXTURESTAGESTATETYPE;

// Texture coordinate addressing modes.

typedef enum _D3DMTEXTUREADDRESS {
    D3DMTADDRESS_WRAP            = 1,
    D3DMTADDRESS_MIRROR          = 2,
    D3DMTADDRESS_CLAMP           = 3,
    D3DMTADDRESS_BORDER          = 4,
    D3DMTADDRESS_FORCE_ULONG     = 0x7fffffff, // force 32-bit size enum
} D3DMTEXTUREADDRESS;

// Values, used with D3DMTSS_TEXCOORDINDEX, to specify that the vertex
// data (position and normal in the camera space) should be taken as texture
// coordinates. Low 16 bits are used to specify texture coordinate index, to
// take the WRAP mode from.

#define D3DMTSS_TCI_PASSTHRU                    0x00000000
#define D3DMTSS_TCI_CAMERASPACENORMAL           0x00010000
#define D3DMTSS_TCI_CAMERASPACEPOSITION         0x00020000
#define D3DMTSS_TCI_CAMERASPACEREFLECTIONVECTOR 0x00030000

// Enumerations for COLOROP and ALPHAOP texture blending operations set in
// texture processing stage controls in D3DMRENDERSTATE.

typedef enum _D3DMTEXTUREOP
{
    // Control
    D3DMTOP_DISABLE              = 1,      // disables stage
    D3DMTOP_SELECTARG1           = 2,      // the default
    D3DMTOP_SELECTARG2           = 3,

    // Modulate
    D3DMTOP_MODULATE             = 4,      // multiply args together
    D3DMTOP_MODULATE2X           = 5,      // multiply and  1 bit
    D3DMTOP_MODULATE4X           = 6,      // multiply and  2 bits

    // Add
    D3DMTOP_ADD                  =  7,   // add arguments together
    D3DMTOP_ADDSIGNED            =  8,   // add with -0.5 bias
    D3DMTOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
    D3DMTOP_SUBTRACT             = 10,   // Arg1 - Arg2, with no saturation
    D3DMTOP_ADDSMOOTH            = 11,   // add 2 args, subtract product
                                         // Arg1 + Arg2 - Arg1*Arg2
                                         // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    D3DMTOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
    D3DMTOP_BLENDTEXTUREALPHA    = 13, // texture alpha
    D3DMTOP_BLENDFACTORALPHA     = 14, // alpha from D3DMRENDERSTATE_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    D3DMTOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
    D3DMTOP_BLENDCURRENTALPHA    = 16, // by alpha of current color

    // Specular mapping
    D3DMTOP_PREMODULATE            = 17,     // modulate with next texture before use
    D3DMTOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
                                             // COLOROP only
    D3DMTOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
                                             // COLOROP only
    D3DMTOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
                                             // COLOROP only
    D3DMTOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
                                             // COLOROP only

    // This can do either diffuse or specular bump mapping with correct input.
    // Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
    // where each component has been scaled and offset to make it signed.
    // The result is replicated into all four (including alpha) channels.
    // This is a valid COLOROP only.

    D3DMTOP_DOTPRODUCT3          = 24,

    // Triadic ops
    D3DMTOP_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
    D3DMTOP_LERP                 = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

    D3DMTOP_FORCE_ULONG = 0x7fffffff,
} D3DMTEXTUREOP;

// Values for COLORARG0,1,2, ALPHAARG0,1,2, and RESULTARG texture blending
// operations set in texture processing stage controls in D3DRENDERSTATE.

#define D3DMTA_SELECTMASK        0x0000000f  // mask for arg selector
#define D3DMTA_DIFFUSE           0x00000000  // select diffuse color (read only)
#define D3DMTA_CURRENT           0x00000001  // select stage destination register (read/write)
#define D3DMTA_TEXTURE           0x00000002  // select texture color (read only)
#define D3DMTA_TFACTOR           0x00000003  // select RENDERSTATE_TEXTUREFACTOR (read only)
#define D3DMTA_SPECULAR          0x00000004  // select specular color (read only)
#define D3DMTA_TEMP              0x00000005  // select temporary register color (read/write)
#define D3DMTA_OPTIONMASK        0x00000030  // mask for option bits
#define D3DMTA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define D3DMTA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)
#define D3DMTA_VALIDMASK         (D3DMTA_OPTIONMASK | D3DMTA_SELECTMASK)

// Values for D3DMTSS_***FILTER texture stage states

typedef enum _D3DMTEXTUREFILTERTYPE
{
    D3DMTEXF_NONE            = 0,    // filtering disabled (valid for mip filter only)
    D3DMTEXF_POINT           = 1,    // nearest
    D3DMTEXF_LINEAR          = 2,    // linear interpolation
    D3DMTEXF_ANISOTROPIC     = 3,    // anisotropic
    D3DMTEXF_FORCE_ULONG     = 0x7fffffff,   // force 32-bit size enum
} D3DMTEXTUREFILTERTYPE;

typedef enum _D3DMTEXTURETRANSFORMFLAGS {
    D3DMTTFF_DISABLE         = 0,    // texture coordinates are passed directly
    D3DMTTFF_COUNT1          = 1,    // rasterizer should expect 1-D texture coords
    D3DMTTFF_COUNT2          = 2,    // rasterizer should expect 2-D texture coords
    D3DMTTFF_COUNT3          = 3,    // rasterizer should expect 3-D texture coords
    D3DMTTFF_PROJECTED       = 256,  // texcoords to be divided by COUNTth element
    D3DMTTFF_FORCE_ULONG     = 0x7fffffff,
} D3DMTEXTURETRANSFORMFLAGS;

// Bits for Flags in ProcessVertices call.

#define D3DMPV_DONOTCOPYDATA     (1 << 0)

#define D3DMPV_VALID             D3DMPV_DONOTCOPYDATA

// Flexible vertex format bits

#define D3DMFVF_POSITION_MASK      0x0007
#define D3DMFVF_XYZ_FLOAT          0x0000
#define D3DMFVF_XYZ_FIXED          0x0001
#define D3DMFVF_XYZRHW_FIXED       0x0002
#define D3DMFVF_XYZRHW_FLOAT       0x0003
#define D3DMFVF_RESERVED_POSITION1 0x0004
#define D3DMFVF_RESERVED_POSITION2 0x0005
#define D3DMFVF_RESERVED_POSITION3 0x0006
#define D3DMFVF_RESERVED_POSITION4 0x0007

#define D3DMFVF_MAX_POSITION       D3DMFVF_XYZRHW_FLOAT

#define D3DMFVF_NORMAL_MASK        0x0038
#define D3DMFVF_NORMAL_NONE        0x0000
#define D3DMFVF_NORMAL_FLOAT       0x0008
#define D3DMFVF_NORMAL_FIXED       0x0010
#define D3DMFVF_RESERVED_NORMAL1   0x0018
#define D3DMFVF_RESERVED_NORMAL2   0x0020
#define D3DMFVF_RESERVED_NORMAL3   0x0028
#define D3DMFVF_RESERVED_NORMAL4   0x0030
#define D3DMFVF_RESERVED_NORMAL5   0x0038

#define D3DMFVF_MAX_NORMAL         D3DMFVF_NORMAL_FIXED

#define D3DMFVF_DIFFUSE            0x0040
#define D3DMFVF_SPECULAR           0x0080

#define D3DMFVF_TEXCOUNT_MASK      0x0700
#define D3DMFVF_TEXCOUNT_SHIFT     8
#define D3DMFVF_TEX0               0x0000
#define D3DMFVF_TEX1               0x0100
#define D3DMFVF_TEX2               0x0200
#define D3DMFVF_TEX3               0x0300
#define D3DMFVF_TEX4               0x0400

#define D3DMFVF_MAX_TEXCOUNT       D3DMFVF_TEX4

#define D3DMFVF_RESERVED           0xF800

#define D3DMFVF_TEXTURECOORDCOUNTS  0x00FF0000
#define D3DMFVF_TEXTURECOORDFORMATS 0xFF000000

// Macros to set and read texture coordinate count bits in the FVF id.

#define D3DMFVF_TEXCOORDCOUNT2 0         // Two values
#define D3DMFVF_TEXCOORDCOUNT1 3         // One value
#define D3DMFVF_TEXCOORDCOUNT3 1         // Three values

#define D3DMFVF_TEXCOORDCOUNT_SHIFT(CoordIndex) ((CoordIndex)*2 + 16)

#define D3DMFVF_TEXCOORDSIZE3(CoordIndex) (D3DMFVF_TEXCOORDCOUNT3 << D3DMFVF_TEXCOORDCOUNT_SHIFT(CoordIndex))
#define D3DMFVF_TEXCOORDSIZE2(CoordIndex) (D3DMFVF_TEXCOORDCOUNT2)
#define D3DMFVF_TEXCOORDSIZE1(CoordIndex) (D3DMFVF_TEXCOORDCOUNT1 << D3DMFVF_TEXCOORDCOUNT_SHIFT(CoordIndex))

#define D3DMFVF_TEXCOORDCOUNT_MASK(CoordIndex) ((D3DMFVF_TEXCOORDCOUNT1 | D3DMFVF_TEXCOORDCOUNT2 | D3DMFVF_TEXCOORDCOUNT3) << D3DMFVF_TEXCOORDCOUNT_SHIFT(CoordIndex))

// Macros to set and read texture coordinate format bits in the FVF id.

#define D3DMFVF_TEXCOORDFORMAT_FLOAT     0
#define D3DMFVF_TEXCOORDFORMAT_FIXED     1
#define D3DMFVF_TEXCOORDFORMAT_RESERVED1 2
#define D3DMFVF_TEXCOORDFORMAT_RESERVED2 3

#define D3DMFVF_TEXCOORDFORMAT_SHIFT(CoordIndex) ((CoordIndex)*2 + 24)

#define D3DMFVF_TEXCOORDFLOAT(CoordIndex) (D3DMFVF_TEXCOORDFORMAT_FLOAT)
#define D3DMFVF_TEXCOORDFIXED(CoordIndex) (D3DMFVF_TEXCOORDFORMAT_FIXED << D3DMFVF_TEXCOORDFORMAT_SHIFT(CoordIndex))

#define D3DMFVF_TEXCOORDFORMAT_MASK(CoordIndex) ((D3DMFVF_TEXCOORDFORMAT_FLOAT | D3DMFVF_TEXCOORDFORMAT_FIXED) << D3DMFVF_TEXCOORDFORMAT_SHIFT(CoordIndex))

// Direct3D Mobile Device types.

typedef enum _D3DMDEVTYPE
{
    D3DMDEVTYPE_DEFAULT      = 0,   // Use default load order

    D3DMDEVTYPE_FORCE_ULONG  = 0x7fffffff
} D3DMDEVTYPE;

// Multi-Sample buffer types.

typedef enum _D3DMMULTISAMPLE_TYPE
{
    D3DMMULTISAMPLE_NONE        = 0,
    D3DMMULTISAMPLE_2_SAMPLES   = 2,
    D3DMMULTISAMPLE_3_SAMPLES   = 3,
    D3DMMULTISAMPLE_4_SAMPLES   = 4,
    D3DMMULTISAMPLE_5_SAMPLES   = 5,
    D3DMMULTISAMPLE_6_SAMPLES   = 6,
    D3DMMULTISAMPLE_7_SAMPLES   = 7,
    D3DMMULTISAMPLE_8_SAMPLES   = 8,
    D3DMMULTISAMPLE_9_SAMPLES   = 9,
    D3DMMULTISAMPLE_10_SAMPLES  = 10,
    D3DMMULTISAMPLE_11_SAMPLES  = 11,
    D3DMMULTISAMPLE_12_SAMPLES  = 12,
    D3DMMULTISAMPLE_13_SAMPLES  = 13,
    D3DMMULTISAMPLE_14_SAMPLES  = 14,
    D3DMMULTISAMPLE_15_SAMPLES  = 15,
    D3DMMULTISAMPLE_16_SAMPLES  = 16,

    D3DMMULTISAMPLE_FORCE_ULONG = 0x7fffffff
} D3DMMULTISAMPLE_TYPE;

/* Formats
 * Most of these names have the following convention:
 *      A = Alpha
 *      R = Red
 *      G = Green
 *      B = Blue
 *      X = Unused Bits
 *      P = Palette
 *      S = Stencil
 *      D = Depth (e.g. Z or W buffer)
 *
 *      Further, the order of the pieces are from MSB first; hence
 *      D3DMFMT_A8P8 indicates that the high byte of this two byte
 *      format is alpha.
 *
 *      D3DMFMT_D16_LOCKABLE indicates:
 *           - An integer 16-bit value.
 *           - An app-lockable surface.
 *
 *      All Depth/Stencil formats except D3DMFMT_D16_LOCKABLE indicate:
 *          - no particular bit ordering per pixel, and
 *          - are not app lockable, and
 *          - the driver is allowed to consume more than the indicated
 *            number of bits per Depth channel (but not Stencil channel).
 *
 */

typedef enum _D3DMFORMAT
{
    D3DMFMT_UNKNOWN                =  0,

    D3DMFMT_R8G8B8                 =  1,
    D3DMFMT_A8R8G8B8               =  2,
    D3DMFMT_X8R8G8B8               =  3,
    D3DMFMT_R5G6B5                 =  4,
    D3DMFMT_X1R5G5B5               =  5,
    D3DMFMT_A1R5G5B5               =  6,
    D3DMFMT_A4R4G4B4               =  7,
    D3DMFMT_R3G3B2                 =  8,
    D3DMFMT_A8R3G3B2               =  9,
    D3DMFMT_X4R4G4B4               = 10,

    D3DMFMT_A8P8                   = 11,
    D3DMFMT_P8                     = 12,

    D3DMFMT_A8                     = 13,

    D3DMFMT_UYVY                   = 14,
    D3DMFMT_YUY2                   = 15,
    D3DMFMT_DXT1                   = 16,
    D3DMFMT_DXT2                   = 17,
    D3DMFMT_DXT3                   = 18,
    D3DMFMT_DXT4                   = 19,
    D3DMFMT_DXT5                   = 20,

    D3DMFMT_D32                    = 21,
    D3DMFMT_D15S1                  = 22,
    D3DMFMT_D24S8                  = 23,
    D3DMFMT_D16                    = 24,
    D3DMFMT_D24X8                  = 25,
    D3DMFMT_D24X4S4                = 26,

    D3DMFMT_INDEX16                = 27,
    D3DMFMT_INDEX32                = 28,

    D3DMFMT_VERTEXDATA             = 29,

    D3DMFMT_D3DMVALUE_FLOAT        = 30,
    D3DMFMT_D3DMVALUE_FIXED        = 31,

    D3DMFMT_NUMFORMAT              = 32,

    D3DMFMT_FORCE_ULONG            = 0xFFFFFFFF

} D3DMFORMAT;

typedef struct _D3DMDISPLAYMODE
{
    UINT            Width;
    UINT            Height;
    UINT            RefreshRate;
    D3DMFORMAT      Format;
} D3DMDISPLAYMODE;

typedef struct _D3DMDEVICE_CREATION_PARAMETERS
{
    UINT            AdapterOrdinal;
    D3DMDEVTYPE     DeviceType;
    HWND            hFocusWindow;
    ULONG           BehaviorFlags;
} D3DMDEVICE_CREATION_PARAMETERS;

typedef enum _D3DMSWAPEFFECT
{
    D3DMSWAPEFFECT_DISCARD           = 1,
    D3DMSWAPEFFECT_FLIP              = 2,
    D3DMSWAPEFFECT_COPY              = 3,
    D3DMSWAPEFFECT_COPY_VSYNC        = 4,

    D3DMSWAPEFFECT_FORCE_ULONG       = 0x7fffffff,
} D3DMSWAPEFFECT;

typedef enum _D3DMPOOL {
    D3DMPOOL_VIDEOMEM                = 0,
    D3DMPOOL_SYSTEMMEM               = 1,
    D3DMPOOL_MANAGED                 = 2,

    D3DMPOOL_FORCE_ULONG             = 0x7fffffff,
} D3DMPOOL;

typedef struct _D3DMPRESENT_PARAMETERS_
{
    UINT                 BackBufferWidth;
    UINT                 BackBufferHeight;
    D3DMFORMAT           BackBufferFormat;
    UINT                 BackBufferCount;

    D3DMMULTISAMPLE_TYPE MultiSampleType;

    D3DMSWAPEFFECT       SwapEffect;
    BOOL                 Windowed;
    BOOL                 EnableAutoDepthStencil;
    D3DMFORMAT           AutoDepthStencilFormat;
    ULONG                Flags;

    UINT                 FullScreen_PresentationInterval;

} D3DMPRESENT_PARAMETERS;

// Values for D3DMPRESENT_PARAMETERS.Flags

#define D3DMPRESENTFLAG_LOCKABLE_BACKBUFFER  0x00000001

// Resource types.

typedef enum _D3DMRESOURCETYPE {
    D3DMRTYPE_SURFACE                = 1,
    D3DMRTYPE_DEPTHSTENCIL           = 2,
    D3DMRTYPE_TEXTURE                = 3,
    D3DMRTYPE_FRONTBUFFER            = 4,
    D3DMRTYPE_BACKBUFFER             = 5,
    D3DMRTYPE_VERTEXBUFFER           = 6,
    D3DMRTYPE_INDEXBUFFER            = 7,

    D3DMRTYPE_FORCE_ULONG            = 0x7fffffff
} D3DMRESOURCETYPE;

// Resource usages.

#define D3DMUSAGE_RENDERTARGET       0x00000001
#define D3DMUSAGE_TEXTURE            0x00000008

#define D3DMUSAGE_LOCKABLE           0x00000010
#define D3DMUSAGE_DYNAMIC            0x00000020
#define D3DMUSAGE_DONOTCLIP          0x00000040
#define D3DMUSAGE_WRITEONLY          0x00000080

#define D3DMUSAGE_VALID              0x000000F9

// Lock flags

#define D3DMLOCK_READONLY            0x00000001
#define D3DMLOCK_NO_DIRTY_UPDATE     0x00000002
#define D3DMLOCK_NOOVERWRITE         0x00000004
#define D3DMLOCK_DISCARD             0x00000008

#define D3DMLOCK_VALID               0x0000000F

// Vertex Buffer Description

typedef struct _D3DMVERTEXBUFFER_DESC
{
    D3DMFORMAT          Format;
    D3DMRESOURCETYPE    Type;
    ULONG               Usage;
    D3DMPOOL            Pool;
    UINT                Size;

    ULONG               FVF;

} D3DMVERTEXBUFFER_DESC;

// Index Buffer Description

typedef struct _D3DMINDEXBUFFER_DESC
{
    D3DMFORMAT          Format;
    D3DMRESOURCETYPE    Type;
    ULONG               Usage;
    D3DMPOOL            Pool;
    UINT                Size;

} D3DMINDEXBUFFER_DESC;

// Surface Description

typedef struct _D3DMSURFACE_DESC
{
    D3DMFORMAT          Format;
    D3DMRESOURCETYPE    Type;
    ULONG               Usage;
    D3DMPOOL            Pool;
    UINT                Size;

    D3DMMULTISAMPLE_TYPE MultiSampleType;
    UINT                 Width;
    UINT                 Height;
} D3DMSURFACE_DESC;

typedef struct _D3DMLOCKED_RECT
{
    INT                 Pitch;
    void*               pBits;
} D3DMLOCKED_RECT;

typedef enum _D3DMBACKBUFFER_TYPE
{
    D3DMBACKBUFFER_TYPE_MONO         = 0,
    D3DMBACKBUFFER_TYPE_FORCE_DWORD  = 0x7fffffff,
} D3DMBACKBUFFER_TYPE;

// Adapter Identifier

#define MAX_DEVICE_IDENTIFIER_STRING        512
typedef struct _D3DMADAPTER_IDENTIFIER
{
    TCHAR           Driver[MAX_DEVICE_IDENTIFIER_STRING];
    TCHAR           Description[MAX_DEVICE_IDENTIFIER_STRING];

    LARGE_INTEGER   DriverVersion;

    ULONG           VendorId;
    ULONG           DeviceId;
    ULONG           SubSysId;
    ULONG           Revision;

    GUID            DeviceIdentifier;
} D3DMADAPTER_IDENTIFIER;

typedef enum _D3DMPROFILE
{
	D3DMPROFILE_BASE         = 0,

	D3DMPROFILE_FORCE_DWORD  = 0x7fffffff,
} D3DMPROFILE;
