#ifndef SCENE_GRAPH_PAGE_CURL_HEADER
#define SCENE_GRAPH_PAGE_CURL_HEADER

#include <sg/visual_model.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Угол страницы
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlCorner
{
  PageCurlCorner_LeftTop,
  PageCurlCorner_LeftBottom,
  PageCurlCorner_RightTop,
  PageCurlCorner_RightBottom
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип страницы
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlPageType
{
  PageCurlPageType_FrontLeft,
  PageCurlPageType_FrontRight,
  PageCurlPageType_BackLeft,
  PageCurlPageType_BackRight,

  PageCurlPageType_Num,

  PageCurlPageType_Front = PageCurlPageType_FrontLeft,
  PageCurlPageType_Back  = PageCurlPageType_BackLeft,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим страницы и задания материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlMode
{
  PageCurlMode_SinglePage,                //одна страница
  PageCurlMode_DoublePageSingleMaterial,  //две страницы, один материал
  PageCurlMode_DoublePageDoubleMaterial   //две страницы, каждая со своим материалом
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс описания параметров перелистывания страницы
///////////////////////////////////////////////////////////////////////////////////////////////////
class PageCurl : public VisualModel
{
  public:
    typedef xtl::com_ptr<PageCurl>       Pointer;
    typedef xtl::com_ptr<const PageCurl> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка режима страниц
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetMode (PageCurlMode mode);
    PageCurlMode Mode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPageMaterial (PageCurlPageType type, const char* name);
    const char* PageMaterial    (PageCurlPageType type) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка полного размера страниц
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetSize (const math::vec2f& size);
    void               SetSize (float width, float height);
    const math::vec2f& Size    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка точки перетаскивания
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetCurlPoint         (PageCurlCorner corner);
    void               SetCurlPoint         (const math::vec2f& point);
    void               SetCurlPoint         (float x, float y);
    const math::vec2f& CurlPoint            () const;
    void               SetCurlPointPosition (const math::vec2f& position);
    void               SetCurlPointPosition (float x, float y);
    const math::vec2f& CurlPointPosition    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка загиба
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetCurlRadius        (float curl_radius);
    float CurlRadius           () const;
    void  SetMinimumCurlRadius (float minimum_curl_radius);
    float MinimumCurlRadius    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Опциональные параметры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetGridSize                      (const math::vec2ui& size);  //детализация разбиения сетки
    void                SetGridSize                      (size_t x, size_t y);
    const math::vec2ui& GridSize                         () const;
    void                SetPageColor                     (const math::vec4f& color);  //цвет страницы
    void                SetPageColor                     (float red, float green, float blue, float alpha);
    const math::vec4f&  PageColor                        () const;
    void                SetCornerShadowOffset            (float shadow_offset);       //смещение тени от угла
    float               CornerShadowOffset               () const;
    void                SetShadowWidth                   (float width);               //ширина тени
    float               ShadowWidth                      () const;
    void                SetShadowLogBase                 (float log_base);            //основание логарифма генерации тени
    float               ShadowLogBase                    () const;
    void                SetShadowMinLogValue             (float value);               //минимальное значение тени при логарифмировании
    float               ShadowMinLogValue                () const;
    void                SetFindBestCurlSteps             (size_t count);              //количество итераций поиска наилучшей позиции загиба
    size_t              FindBestCurlSteps                () const;
    void                SetBindingMismatchWeight         (float weight);              //вес отклонения позиции сгиба страницы при поиске наилучешй позиции загиба
    float               BindingMismatchWeight            () const;
    void                SetRigidPage                     (bool state);                //является ли страница жесткой
    bool                IsRigidPage                      () const;
    void                SetRigidPagePerspectiveFactor    (float factor);              //коэффициент увеличения края жесткой страницы для симуляции перспективы
    float               RigidPagePerspectiveFactor       () const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PageCurl  ();
    ~PageCurl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рассчёт ограничивающего объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
