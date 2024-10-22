#ifndef SCENE_GRAPH_LIGHT_HEADER
#define SCENE_GRAPH_LIGHT_HEADER

#include <sg/entity.h>

namespace scene_graph
{

const float DEFAULT_LIGHT_RANGE  = 1e9;  //расстояние действия источника света по умолчанию
const float DEFAULT_LIGHT_RADIUS = 1e9;  //радиус действия источника света по умолчанию

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник света
///////////////////////////////////////////////////////////////////////////////////////////////////
class Light : public Entity
{
  public: 
    typedef xtl::com_ptr <Light> Pointer;
    typedef xtl::com_ptr <const Light> ConstPointer;  
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет источника света
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void               SetLightColor (const math::vec3f& color);
    const math::vec3f& LightColor    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Интенсивность источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetIntensity (float value);
    float Intensity    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Затухание (constant, linear, quadratic)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetAttenuation (const math::vec3f& multiplier);
    const math::vec3f& Attenuation    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Расстояние действия источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetRange (float range);
    float Range    () const;    

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Light  ();
    ~Light ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Направленный источник света
///////////////////////////////////////////////////////////////////////////////////////////////////
class DirectLight : public Light
{
  public:
    typedef xtl::com_ptr <DirectLight> Pointer;
    typedef xtl::com_ptr <const DirectLight> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetRadius    (float radius);
    float Radius       () const;
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DirectLight  ();
    ~DirectLight ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рассчёт ограничивающего объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конический источник света
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpotLight : public Light
{
  public:
    typedef xtl::com_ptr <SpotLight> Pointer;
    typedef xtl::com_ptr <const SpotLight> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetAngle    (const math::anglef& angle);
    void                SetExponent (float exponent);
    const math::anglef& Angle       () const;
    float               Exponent    () const;
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpotLight  ();
    ~SpotLight ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рассчёт ограничивающего объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Точечный источник света
///////////////////////////////////////////////////////////////////////////////////////////////////
class PointLight : public Light
{
  public:
    typedef xtl::com_ptr <PointLight> Pointer;
    typedef xtl::com_ptr <const PointLight> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание источника света
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PointLight  ();
    ~PointLight ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);    

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рассчёт ограничивающего объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();
};

}

#endif
