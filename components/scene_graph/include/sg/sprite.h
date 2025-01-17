#ifndef SCENE_GRAPH_SPRITE_HEADER
#define SCENE_GRAPH_SPRITE_HEADER

#include <stl/auto_ptr.h>
#include <sg/visual_model.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об обновлении спрайтов в модели
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SpriteModelEvent
{
  SpriteModelEvent_AfterSpriteDescsUpdate,    //срабатывает после изменения данных спрайтов
  SpriteModelEvent_AfterCreationParamsUpdate, //срабатывает после изменения Mode/Usage/UpDirection/Batch параметров

  SpriteModelEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим использования спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SpriteUsage
{
  SpriteUsage_Static,   //не обновляемая модель
  SpriteUsage_Dynamic,  //обновляемая модель
  SpriteUsage_Stream,   //часто обновляемая модель (каждый кадр)
  SpriteUsage_Batching, //пакетирование
  
  SpriteUsage_Default = SpriteUsage_Batching
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SpriteMode
{
  SpriteMode_Billboard         = 1,                                          //up vector is view up,  normal & rotation is ignored
  SpriteMode_Oriented          = 2,                                          //up vector is local up, normal & rotation is used
  SpriteMode_OrientedBillboard = SpriteMode_Billboard | SpriteMode_Oriented, //up vector is view up,  normal & rotation is used

  SpriteMode_Default = SpriteMode_Oriented
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор спрайта
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SpriteDesc
{
  math::vec3f  position;   //положение центра спрайта
  math::vec2f  size;       //размер спрайта
  math::vec4f  color;      //цвет спрайта
  math::vec2f  tex_offset; //смещение начала спрайта в текстуре [0;1]
  math::vec2f  tex_size;   //размер спрайта в текстуре [0;1]
  math::vec3f  normal;     //нормаль
  math::anglef rotation;   //поворот относительно нормали
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Модель, состоящая из простейших примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteModel: public VisualModel
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество спрайтов / размер буфера линий / получение массива спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t            SpriteDescsCount    () const;
    size_t            SpriteDescsCapacity () const;
    const SpriteDesc* SpriteDescs         () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Материал
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMaterial  (const char* material);
    const char* Material     () const;
    size_t      MaterialHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пакет
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetBatch  (const char* name);
    const char* Batch     () const;
    size_t      BatchHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void       SetMode (SpriteMode mode);
    SpriteMode Mode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим использования спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetUsage (SpriteUsage usage);
    SpriteUsage Usage    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вектор "вверх"
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetOrtUp (const math::vec3f& up);
    const math::vec3f& OrtUp    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (SpriteModel& sender, SpriteModelEvent event_id)> EventHandler;

    xtl::connection RegisterEventHandler (SpriteModelEvent event_id, const EventHandler& event_handler);
    
    using VisualModel::RegisterEventHandler;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpriteModel  ();
    ~SpriteModel ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамическая диспетчеризация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об изменении данных спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateSpriteDescsNotify ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);    

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация получения количества спрайтов и массива спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t            SpriteDescsCountCore    () = 0;
    virtual size_t            SpriteDescsCapacityCore () = 0;
    virtual const SpriteDesc* SpriteDescsCore         () = 0;
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Спрайт
///////////////////////////////////////////////////////////////////////////////////////////////////
class Sprite: public SpriteModel
{
  public:
    typedef xtl::com_ptr<Sprite>       Pointer;
    typedef xtl::com_ptr<const Sprite> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание спрайта
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет спрайта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetColor (const math::vec4f& color);
    void               SetColor (float red, float green, float blue);
    void               SetColor (float red, float green, float blue, float alpha);
    void               SetAlpha (float alpha);
    const math::vec4f& Color    () const;
    float              Alpha    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстурные координаты
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetTexOffset (const math::vec2f&);
    void               SetTexSize   (const math::vec2f&);
    const math::vec2f& TexOffset    () const;
    const math::vec2f& TexSize      () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Sprite  ();
    ~Sprite ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамическая диспетчеризация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);    
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация получения количества спрайтов и массива спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t            SpriteDescsCountCore    ();
    size_t            SpriteDescsCapacityCore ();
    const SpriteDesc* SpriteDescsCore         ();        

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteList: public SpriteModel
{
  public:
    typedef xtl::com_ptr<SpriteList>       Pointer;
    typedef xtl::com_ptr<const SpriteList> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание списка спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер массива спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SpritesCount () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера списка спрайтов / резервирование места для хранения спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize  (size_t count);
    void Reserve (size_t count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Массив спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    const SpriteDesc* Sprites () const;
          SpriteDesc* Sprites ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление спрайтов в массив
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (const SpriteDesc& sprite);
    void Insert (size_t sprites_count, const SpriteDesc* sprites);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка списка спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об обновлении данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invalidate ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpriteList  ();
    ~SpriteList ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамическая диспетчеризация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация получения количества спрайтов и массива спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t            SpriteDescsCountCore    ();
    size_t            SpriteDescsCapacityCore ();
    const SpriteDesc* SpriteDescsCore         ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
