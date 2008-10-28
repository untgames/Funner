#include <cstdio>

#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>

#include <engine/attachments.h>

using namespace engine;

template <int TypeNumber> struct A: public xtl::reference_counter
{
  A (const A& a) : id (a.id) { printf ("A::A(const A&): id=%d\n", id); }
  A (int in_id) : id (in_id) { printf ("A::A(int):      id=%d\n", id); }
  ~A ()                      { printf ("A::~A():        id=%d\n", id); }

  void AddRef () { addref (this); }
  void Release () { release (this); }

  int id;
};

typedef A<1> A1;
typedef A<2> A2;

class MyListener : public IAttachmentRegistryListener<A1>, public IAttachmentRegistryListener<A2>
{
  public:
    void OnRegisterAttachment (const char* name, A1& value)
    {
      printf ("OnRegisterAttachment(%s, A1=%d)\n", name, value.id);
    }

    void OnUnregisterAttachment (const char* name, A1& value)
    {
      printf ("OnUnregisterAttachment(%s, A1=%d)\n", name, value.id);
    }

    void OnRegisterAttachment (const char* name, A2& value)
    {
      printf ("OnRegisterAttachment(%s, A2=%d)\n", name, value.id);
    }

    void OnUnregisterAttachment (const char* name, A2& value)
    {
      printf ("OnUnregisterAttachment(%s, A2=%d)\n", name, value.id);
    }    
};

int main ()
{
  printf ("Results of engine_attachments1_test:\n");
  
  MyListener listener;  
  
  try
  {
    A1 a11 = 1;
    xtl::shared_ptr<A1> a12 (new A1 (2));
    xtl::com_ptr<A2> a21 (new A2 (3), false);

    printf ("Attach listener\n");    
    
    AttachmentRegistry::Attach (static_cast<IAttachmentRegistryListener<A2>*> (&listener));    
    
    printf ("Register values\n");
    
    AttachmentRegistry::Register ("Value1", a11);
    AttachmentRegistry::Register ("Value2", a12);
    
    AttachmentRegistry::Attach (static_cast<IAttachmentRegistryListener<A1>*> (&listener), AttachmentRegistryAttachMode_ForceNotify);
    
    printf ("Change value 'Value1'\n");

    AttachmentRegistry::Register ("Value1", a21);
    
    printf ("Unregister value 'Value2'\n");

    AttachmentRegistry::Unregister ("Value2");
    
    printf ("Unregiser all A1 values\n");

    AttachmentRegistry::UnregisterAll<A1> ();

    printf ("Unregiser all A2 values\n");

    AttachmentRegistry::UnregisterAll<A2> ();
  }
  catch (std::exception& exception)
  {        
    printf ("exception: %s\n", exception.what ());
  }
  
  printf ("Detach listener\n");  
  
  AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<A1>*> (&listener));
  AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<A2>*> (&listener));  

  return 0;
}
