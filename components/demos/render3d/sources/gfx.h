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
    static Pointer Create ();

///Добавление выстрела
    void AddShot (const scene_graph::Node& gun, const math::vec4f& color, float distance);

///Получение направления выстрела
    const math::vec3f& ShotDirection (size_t sprite) const;

  protected:
///Конструктор / деструктор
    ShotGfx ();
    ~ShotGfx ();

///Метод, вызываемый при посещении данного объекта
    void AcceptCore (Visitor& visitor);

  private:
///Обновление эффекта
    void UpdateEffect (float dt);

  private:
    struct ShotDesc : public xtl::reference_counter
    {
      math::vec3f position;
      math::vec3f direction;
      math::vec4f color;
      float       distance;
      size_t      born_time;

      void AddRef  () { addref  (this); }
      void Release () { release (this); }
    };

  private:
    typedef xtl::com_ptr <ShotDesc>   ShotDescPtr;
    typedef stl::vector <ShotDescPtr> ShotsArray;

  private:
    size_t               time;
    ShotsArray           shots;
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
