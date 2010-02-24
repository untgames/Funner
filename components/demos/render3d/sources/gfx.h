#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_GFX_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_GFX_HEADER

namespace scene_graph
{

///Эффект выстрела
class ShotGfx : public SpriteList
{
  public:
    typedef xtl::com_ptr <ShotGfx>       Pointer;
    typedef xtl::com_ptr <const ShotGfx> ConstPointer;

///Создание эффекта
    static Pointer Create (const math::vec4f& color, float distance);

    void               SetShotDirection (const math::vec3f& in_shot_direction) { shot_direction = in_shot_direction; }
    const math::vec3f& ShotDirection    () const { return shot_direction; }

  protected:
///Конструктор / деструктор
    ShotGfx (const math::vec4f& in_color, float distance);
    ~ShotGfx ();

///Метод, вызываемый при посещении данного объекта
    void AcceptCore (Visitor& visitor);

  private:
///Обновление эффекта
    void UpdateEffect (float dt);

  private:
    math::vec4f          color;
    math::vec3f          shot_direction;
    float                life_time;
    xtl::auto_connection update_connection;
};

///Спрайты всегда повернутые к камере
class BillboardSpriteList : public SpriteList
{
  public:
    typedef xtl::com_ptr <BillboardSpriteList>       Pointer;
    typedef xtl::com_ptr <const BillboardSpriteList> ConstPointer;

///Создание
    static Pointer Create ();

  protected:
///Конструктор / деструктор
    BillboardSpriteList ();
    ~BillboardSpriteList ();

///Метод, вызываемый при посещении данного объекта
    void AcceptCore (Visitor& visitor);
};

}

#endif
