#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

/*
    Константы
*/

const int    FILENO_STDOUT             = 1;
const int    FILENO_STDERR             = 2;
const size_t MAX_LINE_LENGTH           = 512;
const int    SELECT_TIMEOUT_IN_SECONDS = 1;

/*
    Утилитарный класс для перенаправления Stdio
*/

class StdioRedirector
{
  public:
///Начало перенаправления
    static void Init ()
    {
      if (instance)
        return;
        
      instance = new StdioRedirector;
    }
    
///Конец перенаправления
    static void Shutdown ()
    {
      if (!instance)
        return;
        
      delete instance;
      
      instance = 0;
    }
    
  private:
///Конструктор
    StdioRedirector ()
      : stop_request (false)
      , thread_ready (false)
    {
      try
      {
        for (size_t i=0; i<2; i++)
          stderr_pipe [i] = stdout_pipe [i] = -1;
        
        try
        {
            //инициализация перенаправления
          
          if (pipe (stdout_pipe) != 0)
            throw xtl::format_operation_exception ("", "::pipe failed. %s", strerror (errno));

          if (pipe (stderr_pipe) != 0)
            throw xtl::format_operation_exception ("", "::pipe failed. %s", strerror (errno));

          if (dup2 (stdout_pipe [1], FILENO_STDOUT) != FILENO_STDOUT)
            throw xtl::format_operation_exception ("", "dup2 failed. %s", strerror (errno));

          close (stdout_pipe [1]);

          stdout_pipe [1] = -1;

          if (dup2 (stderr_pipe [1], FILENO_STDERR) != FILENO_STDERR)
            throw xtl::format_operation_exception ("", "dup2 failed. %s", strerror (errno));
            
          close (stderr_pipe [1]);
          
          stderr_pipe [1] = -1;
          
           //создание нити для перенаправления потоков
           
          Lock lock (mutex);
          
          thread.reset (new Thread ("Android stdio redirector", xtl::bind (&StdioRedirector::Run, this)));

          while (!thread_ready)
            condition.Wait (mutex);
        }
        catch (...)
        {
          for (size_t i=0; i<2; i++)
          {
            if (stdout_pipe [i] != -1)
              close (stdout_pipe [i]);
              
            if (stderr_pipe [i] != -1)
              close (stderr_pipe [i]);              
          }

          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::StdioRedirector::StdioRedirector");
        throw;
      }
    }
    
///Деструктор
    ~StdioRedirector ()
    {
      stop_request = true;      

        //печать сообщения для остановки
//      printf("Shutting down\n");
//      fflush(stdout);

      thread->Join ();
    }
    
    struct BufferedData
    {
      char   buffer [MAX_LINE_LENGTH + 1];
      size_t count;
      
      BufferedData () : count () {}
    };    
    
///Основной цикл
    int Run ()
    {
      {
        Lock lock (mutex);

        thread_ready = true;

        condition.NotifyOne ();
      }
      
      JNIEnv* env = 0;
      
      jint status = get_context ().vm->AttachCurrentThread (&env, 0);
      
      if (status)
        return 0;
      
      stl::auto_ptr<BufferedData> stdoutData (new BufferedData), stderrData (new BufferedData);            

      while (!stop_request)
      {
        fd_set read_fds;

        FD_ZERO (&read_fds);
        FD_SET  (stdout_pipe [0], &read_fds);
        FD_SET  (stderr_pipe [0], &read_fds);
        
        timeval tv;
        
        tv.tv_sec  = SELECT_TIMEOUT_IN_SECONDS;
        tv.tv_usec = 0;        

        int max_fd   = stl::max (stdout_pipe [0], stderr_pipe [0]);
        int fd_count = select (max_fd+1, &read_fds, 0, 0, &tv);

        if (fd_count < 0)
        {
          if (errno != EINTR)
          {
            log_error ("select on stdout/stderr failed");
            break;
          }          
        }
        else if (fd_count != 0)
        {
          bool err = false;
          
          if (FD_ISSET (stdout_pipe [0], &read_fds))
            err |= !Redirect (stdout_pipe [0], *stdoutData, ANDROID_LOG_INFO);
          
          if (FD_ISSET (stderr_pipe [0], &read_fds))
            err |= !Redirect (stderr_pipe [0], *stderrData, ANDROID_LOG_ERROR);

          if (err)
          {
            log_error ("stdio converter got read error; shutting it down");
            break;
          }
        }
      }

      close (stdout_pipe [0]);
      close (stderr_pipe [0]);
      
      return 0;
    }
    
    bool Redirect (int fd, BufferedData& data, int log_level)
    {
        //чтение данных
      
      size_t  available = MAX_LINE_LENGTH - data.count;
      ssize_t actual    = read (fd, data.buffer + data.count, available);

      if (actual <= 0)
          return false;

      data.count += actual;
      
        //поиск маркера окончания строки

      char* s           = data.buffer;
      const char* start = data.buffer;
      
      for (size_t i=data.count; i; i--, s++)
      {
        if (*s == '\n' || (*s == '\r' && i && s [1] != '\n'))
        {
          *s = '\0';

          log_printf (log_level, "%s", start);

          start = s + 1;
        }
      }

        //поиск переполнения
      
      if (start == data.buffer && data.count == MAX_LINE_LENGTH)
      {
        data.buffer [MAX_LINE_LENGTH] = '\0';

        log_printf (log_level, "%s!", start);        

        start = s + MAX_LINE_LENGTH;
      }
      
        //обновление данных в случае отсутствия маркера окончания строки

      if (start != data.buffer)
      {
        if (start >= data.buffer + data.count)
        {
          data.count = 0;
        }
        else
        {
          int remaining = data.count - (start - data.buffer);

          memmove (data.buffer, start, remaining);

          data.count = remaining;
        }
      }

      return true;
    }    

  private:
    volatile bool                 stop_request;
    volatile bool                 thread_ready;
    stl::auto_ptr<syslib::Thread> thread;
    Mutex                         mutex;
    Condition                     condition;
    int                           stdout_pipe [2];
    int                           stderr_pipe [2];
    static StdioRedirector*       instance;
};

StdioRedirector* StdioRedirector::instance = 0;

}

namespace syslib
{

namespace android
{

void startStdioRedirection ()
{
  StdioRedirector::Init ();
}

void shutdownStdioRedirection ()
{
  StdioRedirector::Shutdown ();
}

}

}
