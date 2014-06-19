#ifndef SCENE_GRAPH_LINE_HEADER
#define SCENE_GRAPH_LINE_HEADER

#include <stl/auto_ptr.h>
#include <sg/visual_model.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ���������� ����� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LineModelEvent
{
  LineModelEvent_AfterLineDescsUpdate,      //����������� ����� ��������� ������ �����
  LineModelEvent_AfterCreationParamsUpdate, //����������� ����� ��������� Usage/Batch ����������

  LineModelEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LineUsage
{
  LineUsage_Static,   //�� ����������� ������
  LineUsage_Dynamic,  //����������� ������
  LineUsage_Stream,   //����� ����������� ������ (������ ����)
  LineUsage_Batching, //�������������
  
  LineUsage_Default = LineUsage_Batching
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LinePoint
{
  math::vec3f position;   //��������� ����� � ������������
  math::vec4f color;      //���� �����
  math::vec2f tex_offset; //�������� ����� � �������� [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LineDesc
{
  LinePoint point [2]; //����� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������, ��������� �� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LineModel: public VisualModel
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� / ������ ������ ����� / ��������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t          LineDescsCount    () const;
    size_t          LineDescsCapacity () const;
    const LineDesc* LineDescs         () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMaterial  (const char* material);
    const char* Material     () const;
    size_t      MaterialHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetBatch  (const char* name);
    const char* Batch     () const;
    size_t      BatchHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void      SetUsage (LineUsage usage);
    LineUsage Usage    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (LineModel& sender, LineModelEvent event_id)> EventHandler;

    xtl::connection RegisterEventHandler (LineModelEvent event_id, const EventHandler& event_handler);
    
    using VisualModel::RegisterEventHandler;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LineModel  ();
    ~LineModel ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateLineDescsNotify ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ���� � �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);    

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ���������� ����� � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t          LineDescsCountCore    () = 0;
    virtual size_t          LineDescsCapacityCore () = 0;
    virtual const LineDesc* LineDescsCore         () = 0;
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Line: public LineModel
{
  public:
    typedef xtl::com_ptr<Line>       Pointer;
    typedef xtl::com_ptr<const Line> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetColor (size_t point_index, const math::vec4f& color);
    void               SetColor (size_t point_index, float red, float green, float blue);
    void               SetColor (size_t point_index, float red, float green, float blue, float alpha);
    void               SetAlpha (size_t point_index, float alpha);
    const math::vec4f& Color    (size_t point_index) const;
    float              Alpha    (size_t point_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetTexOffset (size_t point_index, const math::vec2f&);
    const math::vec2f& TexOffset    (size_t point_index) const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Line  ();
    ~Line ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ���� � �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);    
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ���������� ����� � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t          LineDescsCountCore    ();
    size_t          LineDescsCapacityCore ();
    const LineDesc* LineDescsCore         ();        

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class LineList: public LineModel
{
  public:
    typedef xtl::com_ptr<LineList>       Pointer;
    typedef xtl::com_ptr<const LineList> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LinesCount () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ����� / �������������� ����� ��� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize  (size_t count);
    void Reserve (size_t count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const LineDesc* Lines () const;
          LineDesc* Lines ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (const LineDesc& Line);
    void Insert (size_t Lines_count, const LineDesc* Lines);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invalidate ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LineList  ();
    ~LineList ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ���������� ����� � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t          LineDescsCountCore    ();
    size_t          LineDescsCapacityCore ();
    const LineDesc* LineDescsCore         ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
