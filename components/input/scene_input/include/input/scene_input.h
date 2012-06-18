#ifndef INPUT_SYSTEM_SCENE_INPUT_HEADER
#define INPUT_SYSTEM_SCENE_INPUT_HEADER

#include <sg/input.h>
#include <sg/screen.h>

namespace input
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneInputManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneInputManager  ();
    SceneInputManager  (const SceneInputManager&);
    ~SceneInputManager ();
    
    SceneInputManager& operator = (const SceneInputManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
          scene_graph::Screen* Screen    ();
    const scene_graph::Screen* Screen    () const;
          void                 SetScreen (scene_graph::Screen* screen);
          
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
//    void SetTouchHalfSize ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessEvent (const char* event);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SceneInputManager&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SceneInputManager&, SceneInputManager&);

}

#endif