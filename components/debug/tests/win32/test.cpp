#include <windows.h>
#include <process.h>

#include <stdio.h>

#include <stl/map>

class CriticalSection
{
  public:
    CriticalSection ()
    {
      InitializeCriticalSection (&cs);
    }
    
    ~CriticalSection ()
    {
      DeleteCriticalSection (&cs);
    }
    
    class Lock
    {
      public:
        Lock (CriticalSection& inCs)
          : cs (inCs.cs)
        {
          EnterCriticalSection (&cs);
        }
        
        ~Lock ()
        {
          LeaveCriticalSection (&cs);
        }
        
      private:
        CRITICAL_SECTION& cs;
    };
  
  private:
    CriticalSection (const CriticalSection&);
    CriticalSection& operator = (const CriticalSection&);
  
  private:
    CRITICAL_SECTION cs;
};

class CallStack
{
  public:
    struct Entry;
    
  private:
    typedef stl::multimap<DWORD, Entry*> EntryList;

  public:
    struct Entry
    {
      friend class CallStack;
      public:
        Entry (const char* in_func, const char* in_file, int in_line)
          : func (in_func)
          , file (in_file)
          , line (in_line)
          , thread_id (GetCurrentThreadId ())
        {
          CallStack& stack = Instance ();
          
          CriticalSection::Lock lock (stack.cs);
          
          iter = stack.entries.insert_pair (thread_id, this);
        }
        
        ~Entry ()
        {
          CallStack& stack = Instance ();
          
          CriticalSection::Lock lock (stack.cs);
          
          stack.entries.erase (iter);
        }
        
      private:
        Entry (const Entry&);
        Entry& operator = (const Entry&);
      
      private:
        DWORD               thread_id;
        const char*         func;
        const char*         file;
        int                 line;
        EntryList::iterator iter;
    };
    
    static void PrintDump ()
    {
      CallStack& stack = Instance ();
      
      CriticalSection::Lock lock (stack.cs);
      
      DWORD currentId = ~0;
      
      printf ("Callstack:\n");
      fflush (stdout);
            
      for (EntryList::reverse_iterator iter=stack.entries.rbegin (), end=stack.entries.rend (); iter!=end; ++iter)
      {
        Entry& entry = *iter->second;
        
        if (entry.thread_id != currentId)
        {
          printf ("  Thread %u:\n", entry.thread_id);
          fflush (stdout);
          
          currentId = entry.thread_id;
        }
        
        printf ("    %s\t%s(%u)\n", entry.func, entry.file, entry.line);
        fflush (stdout);
      }
      
      printf ("End callstack\n");
      fflush (stdout);
    }

  private:
    CallStack ()
    {
      SetUnhandledExceptionFilter (&CallStack::UnhandledExceptionFilter);
    }
    
    ~CallStack ()
    {
      SetUnhandledExceptionFilter (0);  
    }
    
    static CallStack& Instance ()
    {
      static CallStack instance;
      
      return instance;
    }
    
    static LONG WINAPI UnhandledExceptionFilter (EXCEPTION_POINTERS* info)
    {
      printf ("--------------- CRASH DETECTED! ---------------\n");
      
      PrintDump ();
      
      return EXCEPTION_EXECUTE_HANDLER;
    }
    
  private:
    CriticalSection cs;
    EntryList       entries;
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define CALL_STACK_ENTRY CallStack::Entry TOKENPASTE2(call_stack_entry,__LINE__)(__FUNCTION__, __FILE__, __LINE__);

void threadFn (void* param)
{
  CALL_STACK_ENTRY;

  *(int*)param = 0;
  
  CALL_STACK_ENTRY;  
}

int main ()
{
  CALL_STACK_ENTRY;
   
  CALL_STACK_ENTRY;
  
  HANDLE thread = 0;
  
  {
    CALL_STACK_ENTRY;
  
    CallStack::PrintDump ();
    
    thread = (HANDLE)_beginthread (threadFn, 0, 0);    
  }
  
  WaitForMultipleObjects (1, &thread, TRUE, INFINITE);  

  return 0;
}
