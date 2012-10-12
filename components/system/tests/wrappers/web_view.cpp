#include "shared.h"

void event_handler (WebView& view, WebViewEvent event)
{
  switch (event)
  {
    case WebViewEvent_OnLoadStart:
      printf ("load started: request='%s', status='%s'\n", view.Request (), view.Status ());
      break;
    case WebViewEvent_OnLoadFinish:
      printf ("load finished: request='%s', status='%s'\n", view.Request (), view.Status ());
      break;      
    case WebViewEvent_OnLoadFail:
      printf ("load failed: request='%s', status='%s'\n", view.Request (), view.Status ());
      break;
    case WebViewEvent_OnClose:
      printf ("web view closed: status='%s'\n", view.Status ());
      Application::Exit (0);
      break;
    case WebViewEvent_OnDestroy:
      printf ("web view destroyed\n");
      break;      
    default:
      break;
  }
  fflush (stdout);  
}

bool filter (WebView& view, const char* request)
{
  printf ("navigate to '%s'\n", request);
  fflush (stdout);  
  
  return true;
}

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of web_view_test:\n");
  fflush (stdout);
  
  try
  {
    Window parent_window (WindowStyle_Overlapped);
    
//    parent_window.SetSize (800, 600);
//    parent_window.SetPosition (100, 100);
    parent_window.SetBackgroundColor (Color (0, 255, 0));
    parent_window.SetBackgroundState (true);
//    parent_window.Show ();    
    parent_window.Maximize ();    
    
    parent_window.RegisterEventHandler (WindowEvent_OnClose, &destroy);    
    
    WebView view;
    
    view.RegisterEventHandler (WebViewEvent_OnLoadStart, &event_handler);
    view.RegisterEventHandler (WebViewEvent_OnLoadFinish, &event_handler);
    view.RegisterEventHandler (WebViewEvent_OnLoadFail, &event_handler);
    view.RegisterEventHandler (WebViewEvent_OnClose, &event_handler);
    view.RegisterEventHandler (WebViewEvent_OnDestroy, &event_handler);
    view.RegisterFilter       (&filter);
    
    view.Window ().SetPosition (parent_window.Position ().x + parent_window.Width () / 4, parent_window.Position ().y + parent_window.Height () / 4);
    view.Window ().SetSize     (parent_window.Width () / 2, parent_window.Height () / 2);
    
    view.Window ().Show ();

    view.Window ().SetTitle ("Test web view");
    view.Window ().EnableBackground ();    

//    CookieManager::DeleteCookies ("https://www.login.vk.com/");

//    view.LoadRequest ("http://google.com");
    view.LoadRequest ("http://google.com");
//    view.LoadRequest ("http://oauth.vk.com/authorize?client_id=2972852&scope=friends&redirect_uri=http://oauth.vk.com/blank.html&display=touch&response_type=token");
//    view.LoadRequest ("https://oauth.vk.com/logout?client_id=2972852");

    Application::Run ();
    
    printf ("can_go_forward: %d, can_go_back: %d\n", view.CanGoForward (), view.CanGoBack ());
    fflush (stdout);
  
    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
