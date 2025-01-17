///////////////////////////////////////////////////////////////////////////////////////////////////
///Прямоугольный объем (для совместного доступа нескольких объектов рендеринга)
///////////////////////////////////////////////////////////////////////////////////////////////////
class BoxAreaImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    BoxAreaImpl ();
    BoxAreaImpl (const render::manager::Box&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры области
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                        SetBox (const render::manager::Box& Box);
    const render::manager::Box& Box    () const { return box; }
        
  private:
    render::manager::Box box;
};
