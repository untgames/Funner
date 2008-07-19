#ifndef RENDER_DESKTOP_HEADER
#define RENDER_DESKTOP_HEADER

#include <render/viewport.h>

namespace render
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDesktopListener
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnChangeName            (const char* new_name) {}
    virtual void OnChangeArea            (const Rect& new_area) {}
    virtual void OnChangeBackgroundColor (const math::vec4f& new_color) {}
    virtual void OnAttachViewport        (Viewport&) {}
    virtual void OnDetachViewport        (Viewport&) {}
    virtual void OnDestroy               () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () {}
    virtual void Release () {}
    
  protected:
    virtual ~IDesktopListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������������ - ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Desktop
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Desktop  ();
    Desktop  (const Desktop&); //�������� ����������� - ������� ������
    ~Desktop ();

    Desktop& operator = (const Desktop&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetBackgroundColor (const math::vec4f& color);
    void               SetBackgroundColor (float red, float green, float blue, float alpha=0.0f);
    const math::vec4f& BackgroundColor    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetArea   (const Rect& rect);
    void        SetArea   (int left, int top, size_t width, size_t height);    
    void        SetOrigin (int left, int top);
    void        SetSize   (size_t width, size_t height);
    const Rect& Area      () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach (const Viewport&);
    void Detach (const Viewport&);

    void DetachAllViewports ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ViewportsCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////    
          render::Viewport& Viewport (size_t index);
    const render::Viewport& Viewport (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachListener     (IDesktopListener*);
    void DetachListener     (IDesktopListener*);
    void DetachAllListeners ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Desktop&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Desktop&, Desktop&);

}

#endif
