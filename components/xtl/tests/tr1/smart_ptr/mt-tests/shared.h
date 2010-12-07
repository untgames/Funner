#if defined(_MSC_VER)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

#include <xtl/shared_ptr.h>
#include <xtl/bind.h>
#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>

#include <stl/vector>
#include <stl/memory>
#include <stl/stdexcept>

#include <cstdio>
#include <ctime>
#include <cstdlib>

// 'portable' thread framework

class abstract_thread
{
public:

    virtual ~abstract_thread() {}
    virtual void run() = 0;
};

#ifdef _WIN32

char const * title = "Using Windows threads";

#include <windows.h>
#include <process.h>

typedef HANDLE pthread_t;

unsigned __stdcall common_thread_routine(void * pv)
{
    abstract_thread * pt = static_cast<abstract_thread *>(pv);
    pt->run();
    delete pt;
    return 0;
}

int pthread_create(pthread_t * thread, void const *, unsigned (__stdcall * start_routine) (void*), void* arg)
{
    HANDLE h = (HANDLE)_beginthreadex(0, 0, start_routine, arg, 0, 0);

    if(h != 0)
    {
        *thread = h;
        return 0;
    }
    else
    {
        return 1; // return errno;
    }
}

int pthread_join(pthread_t thread, void ** /*value_ptr*/)
{
    ::WaitForSingleObject(thread, INFINITE);
    ::CloseHandle(thread);
    return 0;
}

#else

char const * title = "Using POSIX threads";

#include <pthread.h>
#include <unistd.h>

extern "C" void * common_thread_routine(void * pv)
{
    abstract_thread * pt = static_cast<abstract_thread *>(pv);
    pt->run();
    delete pt;
    return 0;
}

#endif

//

template<class F> class thread: public abstract_thread
{
public:

    explicit thread(F f): f_(f)
    {
    }

    void run()
    {
        f_();
    }

private:

    F f_;
};

template<class F> pthread_t createThread(F f)
{
    stl::auto_ptr<abstract_thread> p(new thread<F>(f));

    pthread_t r;

    if(pthread_create(&r, 0, common_thread_routine, p.get()) == 0)
    {
        p.release();
        return r;
    }

    throw stl::runtime_error("createThread failed.");
}

class A
{
  public:
    A ()
    {
      ++count;
      printf ("A::A\n");
      fflush (stdout);
    }
    
    ~A ()
    {
      printf ("A::~A\n");
      
      if (!--count)
        printf ("all deleted\n");

      fflush (stdout);
    }
  
  private:
    A (const A&); //no impl
    A& operator = (const A&); //no impl
    
  private:
    static int count;
};

int A::count = 0;

class B: public A, public xtl::reference_counter
{
};
