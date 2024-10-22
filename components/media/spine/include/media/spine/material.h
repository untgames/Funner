#ifndef MEDIALIB_SPINE_MATERIAL_HEADER
#define MEDIALIB_SPINE_MATERIAL_HEADER

namespace media
{

namespace spine
{

//forward declarations
class MaterialImpl;
class Wrappers;

//Possible blend modes
enum BlendMode
{
  BlendMode_Normal,
  BlendMode_Additive,
  BlendMode_Multiply,
  BlendMode_Screen
};

//possible texcoord wrap modes
enum TexcoordWrap
{
  TexcoordWrap_MirroredRepeat,
  TexcoordWrap_ClampToEdge,
  TexcoordWrap_Repeat
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Mesh material
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material (const Material&);
    ~Material ();

    Material& operator = (const Material&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::BlendMode BlendMode     () const;
    TexcoordWrap            TexcoordWrapU () const;
    TexcoordWrap            TexcoordWrapV () const;
    const char*             TexturePath   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Material&);

  protected:
    Material (MaterialImpl*);

  private:
    MaterialImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Material&, Material&);

}

}

#endif
