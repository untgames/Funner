#include "../shared.h"

using namespace syslib;

#undef CreateWindow

//TODO: исправить утечку

namespace
{

struct WebViewInitializer
{
  WebViewInitializer ()
  {
    if (OleInitialize (0) != S_OK)
      raise_error ("::OleInitialize failed");
  }
  
  ~WebViewInitializer ()
  {
    ::OleUninitialize ();
  }
};

typedef common::Singleton<WebViewInitializer> WebViewInitializerSingleton;

}

/*
    Реализация вида
*/

struct syslib::web_view_handle: public IUnknown
{
  IWebViewListener* listener;           //слушатель события вида
  window_t          host_window;        //окно-контейнер
  HWND              host_window_handle; //дескриптор окна-контейнера
  long              ref_count;          //счетчик ссылок
  bool              has_scroll_bars;    //есть ли возможность скроллирования
  IWebBrowser2*     ibrowser;           //указатель на браузер
  DWORD             cookie;             //cookie
  bool              is_loading;         //загружается ли контент
  bool              can_go_forward;     //возможно ли перемещение вперед
  bool              can_go_back;        //возможно ли перемещение назад
  
///Конструктор
  web_view_handle (IWebViewListener* in_listener)
    : listener (in_listener)
    , host_window (WindowsWindowManager::CreateWindow (WindowStyle_PopUp, &WindowMessageHandler, 0, 0, this, 0))
    , host_window_handle ((HWND)WindowsWindowManager::GetNativeWindowHandle (host_window))
    , ref_count (0)
    , has_scroll_bars (false)
    , ibrowser (0)
    , cookie ()
    , client_site (this)
    , site (this)
    , frame (this)
    , dispatch (this)
    , ui_handler (this)
    , is_loading (false)
    , can_go_forward (false)
    , can_go_back (false)
  {   
    IOleObject* iole = 0;
    
    try
    {
      has_scroll_bars = (GetWindowLongPtr (host_window_handle, GWL_STYLE) & (WS_HSCROLL | WS_VSCROLL)) != 0;
      
      RECT rc;
      
      if (!GetClientRect (host_window_handle, &rc))
        raise_error ("::GetClientRect");        
      
      HRESULT hr = CoCreateInstance (CLSID_WebBrowser, 0, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&iole); 

      if (!iole) 
        throw xtl::format_operation_exception ("", "::CoCreateInstance(CLSID_WebBrowser, ...) failed");

      if ((hr = iole->SetClientSite (&client_site)) != S_OK) 
        raise_com_error ("", "SetClientSite failed", hr);                

      if ((hr = iole->SetHostNames (L"MyHost", L"MyDoc")) != S_OK)
        raise_com_error ("", "SetHostNames failed", hr);        

      if ((hr = OleSetContainedObject (iole, TRUE)) != S_OK)
        raise_com_error ("", "SetContainedObject failed", hr);        

      if ((hr = iole->DoVerb (OLEIVERB_SHOW, 0, &client_site, 0, host_window_handle, &rc)) != S_OK)
        raise_com_error ("", "DoVerb failed", hr);

      IConnectionPointContainer* cpc = 0;           

      iole->QueryInterface (IID_IConnectionPointContainer, (void**)&cpc);

      if (!cpc)
        throw xtl::format_operation_exception ("", "QueryInterface(IID_IConnectionPointContainer, ...) failed");

      IConnectionPoint* cp = 0;
      
      cpc->FindConnectionPoint (DIID_DWebBrowserEvents2, &cp);

      if (!cp)
        throw xtl::format_operation_exception ("", "FindConnectionPoint failed");

      cp->Advise  (static_cast<IDispatch*> (&dispatch), &cookie);
      cp->Release ();
      cpc->Release ();

      iole->QueryInterface (IID_IWebBrowser2, (void**)&ibrowser); 

      if (!ibrowser)
        throw xtl::format_operation_exception ("", "QueryInterface(IID_IWebBrowser2, ...) failed");

      iole->Release ();      
    }
    catch (...)
    {
      if (iole) iole->Release ();
      
      WindowsWindowManager::DestroyWindow (host_window);
      throw;
    }
  }
  
///Деструктор
  ~web_view_handle ()
  {
    try
    {
      WindowsWindowManager::DestroyWindow (host_window);
    }
    catch (...)
    {      
    }
  }

/// Закрытие окна    
  void Close ()
  {
    IConnectionPointContainer* cpc = 0;

    ibrowser->QueryInterface (IID_IConnectionPointContainer, (void**)&cpc);

    if (cpc)
    {
      IConnectionPoint* cp = 0; 

      cpc->FindConnectionPoint (DIID_DWebBrowserEvents2, &cp);

      if (cp) 
      {
        cp->Unadvise (cookie);          
        cp->Release ();
      }

      cpc->Release();
    }

    IOleObject* iole = 0;

    ibrowser->QueryInterface (IID_IOleObject, (void**)&iole);     
    
    ibrowser->Quit ();

    ibrowser->Release ();

    if (iole) 
    {
      iole->Close  (OLECLOSE_NOSAVE); 
      iole->Release ();
    }
  }
  
/// IUnknown
  
  HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void** result)
  { 
    *result = 0;        

    if (riid == IID_IUnknown)
      *result = static_cast<IUnknown*> (this);

    if (riid == IID_IOleClientSite)
      *result = static_cast<IOleClientSite*> (&client_site);

    if (riid == IID_IOleWindow)
      *result = static_cast<IOleWindow*> (&site);

    if (riid == IID_IOleInPlaceSite)
      *result = static_cast<IOleInPlaceSite*> (&site);

    if (riid == IID_IOleInPlaceUIWindow)
      *result = static_cast<IOleInPlaceUIWindow*> (&frame);

    if (riid == IID_IOleInPlaceFrame)
      *result = static_cast<IOleInPlaceFrame*> (&frame);

    if (riid == IID_IDispatch)
      *result = static_cast<IDispatch*> (&dispatch);

    if (riid == IID_IDocHostUIHandler)
      *result = static_cast<IDocHostUIHandler*> (&ui_handler);

    if (*result)
    {
      AddRef ();
      
      return S_OK;
    }

    return E_NOINTERFACE;
  }
  
  ULONG STDMETHODCALLTYPE AddRef ()
  {
    return InterlockedIncrement (&ref_count);
  }
  
  ULONG STDMETHODCALLTYPE Release ()
  {
    int tmp = InterlockedDecrement (&ref_count);     
    
//    if (!tmp)  //it is not needed, because WebView removes manually
//      delete this;       
      
    return tmp;
  }  

/// IOleClientSite    
  struct OleClientSite: public IOleClientSite
  {
    web_view_handle* parent;
    
    OleClientSite (web_view_handle* in_parent) : parent (in_parent) {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface         (REFIID riid, void** result) { return parent->QueryInterface (riid, result); }
    ULONG   STDMETHODCALLTYPE AddRef                 ()                           { return parent->AddRef (); }
    ULONG   STDMETHODCALLTYPE Release                ()                           { return parent->Release (); }
    HRESULT STDMETHODCALLTYPE SaveObject             ()                           { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetMoniker             (DWORD,DWORD,IMoniker**)     { return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE GetContainer           (IOleContainer** result)     { *result = 0; return E_NOINTERFACE;}
    HRESULT STDMETHODCALLTYPE ShowObject             ()                           { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnShowWindow           (BOOL)                       { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE RequestNewObjectLayout ()                           { return E_NOTIMPL; }
  } client_site;
  
/// IOleInPlaceSite
  struct OleInPlaceSite: public IOleInPlaceSite
  {
    web_view_handle* parent;
    
    OleInPlaceSite (web_view_handle* in_parent) : parent (in_parent) {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface       (REFIID riid, void** result) { return parent->QueryInterface (riid, result); }
    ULONG   STDMETHODCALLTYPE AddRef               ()                           { return parent->AddRef (); }
    ULONG   STDMETHODCALLTYPE Release              ()                           { return parent->Release (); }    
    HRESULT STDMETHODCALLTYPE GetWindow            (HWND* result)               { *result = parent->host_window_handle; return S_OK; }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp (BOOL)                       { return E_NOTIMPL; }

    HRESULT STDMETHODCALLTYPE CanInPlaceActivate () { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate  () { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnUIActivate       () { return S_OK; }

    HRESULT STDMETHODCALLTYPE GetWindowContext (IOleInPlaceFrame** frame, IOleInPlaceUIWindow** doc, LPRECT posRect, LPRECT clipRect, LPOLEINPLACEFRAMEINFO info)
    { 
      *frame = &parent->frame;

      AddRef ();

      *doc = 0;

      info->fMDIApp       = FALSE; 
      info->hwndFrame     = parent->host_window_handle; 
      info->haccel        = 0; 
      info->cAccelEntries = 0;

      GetClientRect (parent->host_window_handle, posRect);
      GetClientRect (parent->host_window_handle, clipRect);
      
      return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Scroll              (SIZE) { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE OnUIDeactivate      (BOOL) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate ()     { return S_OK; }
    HRESULT STDMETHODCALLTYPE DiscardUndoState    ()     { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo   ()     { return E_NOTIMPL; }
    
    HRESULT STDMETHODCALLTYPE OnPosRectChange (LPCRECT posRect)
    { 
      IOleInPlaceObject *iole = 0;
      
      parent->ibrowser->QueryInterface (IID_IOleInPlaceObject, (void**)&iole);
      
      if (iole) 
      {
        iole->SetObjectRects (posRect, posRect); 
        
        iole->Release ();
      }

      return S_OK;
    }
  } site;
  
  struct OleInPlaceFrame : public IOleInPlaceFrame
  {
    web_view_handle* parent;
    
    OleInPlaceFrame (web_view_handle* in_parent) : parent (in_parent) {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void** result) { return parent->QueryInterface (riid, result); }
    ULONG   STDMETHODCALLTYPE AddRef         ()                           { return parent->AddRef (); }
    ULONG   STDMETHODCALLTYPE Release        ()                           { return parent->Release (); }
    
    HRESULT STDMETHODCALLTYPE GetWindow            (HWND* result)  { *result = parent->host_window_handle; return S_OK; }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp (BOOL)          { return E_NOTIMPL; }    
        
    /// IOleInPlaceUIWindow
    HRESULT STDMETHODCALLTYPE GetBorder          (LPRECT)                              { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE RequestBorderSpace (LPCBORDERWIDTHS)                     { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetBorderSpace     (LPCBORDERWIDTHS)                     { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetActiveObject    (IOleInPlaceActiveObject*, LPCOLESTR) { return S_OK; }

    /// IOleInPlaceFrame
    HRESULT STDMETHODCALLTYPE InsertMenus          (HMENU, LPOLEMENUGROUPWIDTHS) { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetMenu              (HMENU, HOLEMENU, HWND)       { return S_OK; }
    HRESULT STDMETHODCALLTYPE RemoveMenus          (HMENU)                       { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetStatusText        (LPCOLESTR)                   { return S_OK; }
    HRESULT STDMETHODCALLTYPE EnableModeless       (BOOL)                        { return S_OK; }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator (LPMSG,WORD)                  { return E_NOTIMPL; }
  } frame;
  
/// IDispatch
  struct Dispatch: public IDispatch
  {
    web_view_handle* parent;
    
    Dispatch (web_view_handle* in_parent) : parent (in_parent) {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void** result) { return parent->QueryInterface (riid, result); }
    ULONG   STDMETHODCALLTYPE AddRef         ()                           { return parent->AddRef (); }
    ULONG   STDMETHODCALLTYPE Release        ()                           { return parent->Release (); }    
    
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount (UINT* pctinfo)                      { *pctinfo = 0; return S_OK; }
    HRESULT STDMETHODCALLTYPE GetTypeInfo      (UINT,LCID,ITypeInfo**)              { return E_FAIL; }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames    (REFIID,LPOLESTR*,UINT,LCID,DISPID*) { return E_FAIL; }

    HRESULT STDMETHODCALLTYPE Invoke (DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* params, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
    { 
      switch (dispIdMember)
      { 
        // DWebBrowserEvents2
        case DISPID_BEFORENAVIGATE2:
          parent->BeforeNavigate2 (params->rgvarg [5].pvarVal->bstrVal, params->rgvarg [0].pboolVal); 
          break;
        case DISPID_DOCUMENTCOMPLETE:
          parent->DocumentComplete (params->rgvarg [0].pvarVal->bstrVal); 
          break;
        case DISPID_COMMANDSTATECHANGE:
        {
          DWORD command = params->rgvarg [1].lVal;
          bool  enabled = params->rgvarg [0].iVal != VARIANT_FALSE;

          switch (command)
          {
            case CSC_NAVIGATEFORWARD: parent->can_go_forward = enabled; break;
            case CSC_NAVIGATEBACK:    parent->can_go_back    = enabled; break;            
            default:                  break;
          }

          break;
        }
        case DISPID_NAVIGATEERROR: 
        {
          //Extract the status code from the DISPPARAMS structure
          VARIANT*   vt_statuscode   = params->rgvarg [1].pvarVal;
          DWORD      dwStatusCode    = vt_statuscode->lVal;
          
          parent->DocumentLoadFailed (dwStatusCode);                    

          break;          
        }
        // http://msdn2.microsoft.com/en-us/library/ms671911(VS.80).aspx
        case DISPID_AMBIENT_DLCONTROL: 
        {
          pVarResult->vt   = VT_I4;
          pVarResult->lVal = DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS | DLCTL_SILENT;
          //???break
        }
        default:
          return DISP_E_MEMBERNOTFOUND;
      }

      return S_OK;
    }
  } dispatch;  
    
/// IDocHostUIHandler
  struct DocHostUIHandler: public IDocHostUIHandler
  {
    web_view_handle* parent;
    
    DocHostUIHandler (web_view_handle* in_parent) : parent (in_parent) {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void** result) { return parent->QueryInterface (riid, result); }
    ULONG   STDMETHODCALLTYPE AddRef         ()                           { return parent->AddRef (); }
    ULONG   STDMETHODCALLTYPE Release        ()                           { return parent->Release (); }        
    
    HRESULT STDMETHODCALLTYPE ShowContextMenu (DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) { return S_OK; }
  
    HRESULT STDMETHODCALLTYPE GetHostInfo (DOCHOSTUIINFO* pInfo)
    {
      pInfo->dwFlags = (parent->has_scroll_bars ? 0 : DOCHOSTUIFLAG_SCROLL_NO) | DOCHOSTUIFLAG_NO3DOUTERBORDER;
      return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ShowUI (DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc) 
    {
      return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HideUI                ()                                      { return S_OK; }
    HRESULT STDMETHODCALLTYPE UpdateUI              ()                                      { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDocWindowActivate   (BOOL)                                  { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnFrameWindowActivate (BOOL)                                  { return S_OK; }
    HRESULT STDMETHODCALLTYPE ResizeBorder          (LPCRECT, IOleInPlaceUIWindow*, BOOL)   { return S_OK; }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator  (LPMSG, const GUID*, DWORD)             { return S_FALSE; }
    HRESULT STDMETHODCALLTYPE GetOptionKeyPath      (LPOLESTR*, DWORD)                      { return S_FALSE; }
    HRESULT STDMETHODCALLTYPE GetDropTarget         (IDropTarget*, IDropTarget**)           { return S_FALSE; }
    HRESULT STDMETHODCALLTYPE GetExternal           (IDispatch** ppDispatch)                { *ppDispatch = 0; return S_FALSE; }
    HRESULT STDMETHODCALLTYPE TranslateUrl          (DWORD, OLECHAR*, OLECHAR **ppchURLOut) { *ppchURLOut = 0; return S_FALSE; }
    HRESULT STDMETHODCALLTYPE FilterDataObject      (IDataObject*, IDataObject **ppDORet)   { *ppDORet    = 0; return S_FALSE; }
    HRESULT STDMETHODCALLTYPE EnableModeless        (BOOL)                                  { return S_OK; }    
  } ui_handler;
  
///Обработчик событий окна
  static void WindowMessageHandler (window_t, WindowEvent event, const WindowEventContext& context, void* user_data)
  {
    if (!user_data)
      return;
      
    reinterpret_cast<web_view_handle*> (user_data)->ProcessWindowMessage (event, context);
  }
  
  void ProcessWindowMessage (WindowEvent event, const WindowEventContext&)
  {
    switch (event)
    {
      case WindowEvent_OnSize:
      {
        RECT rc;
      
        if (!GetClientRect (host_window_handle, &rc))
          raise_error ("::GetClientRect");        

        ibrowser->put_Left   (rc.left);
        ibrowser->put_Top    (rc.top);
        ibrowser->put_Width  (rc.right);
        ibrowser->put_Height (rc.bottom);

        break;
      }
      case WindowEvent_OnDestroy:
        if (listener)
          listener->OnClose ();

        break;
      default:
        break;
    }
  }

///Загрузка данных
  void LoadRequest (const char* request)
  {
    stl::wstring wRequest = common::towstring (request);

    BSTR bs = SysAllocStringLen (wRequest.data (), wRequest.size ());

    if (!bs)
      throw xtl::format_operation_exception ("", "::SysAllocStringLen failed");
      
    HRESULT hr;                  
    
    _variant_t empty;
    
    empty.ChangeType (VT_I4);

    if ((hr = ibrowser->Navigate (bs, &empty, &empty, &empty, &empty)) != S_OK)
      raise_com_error ("", "IWebBrowser2::Navigate failed", hr);

    SysFreeString (bs);
  }
  
///Фильтр загрузки
  void BeforeNavigate2 (BSTR uri, short* cancel)
  {
    *cancel = FALSE;       
    
    stl::string utf8_uri = common::to_utf8_string (uri);
    
    if (listener)
      *cancel = listener->ShouldStartLoading (utf8_uri.c_str ()) == 0;          
    
    if (!*cancel)
    {
      is_loading = true;
      
      if (listener)
        listener->OnLoadStarted (utf8_uri.c_str ());
    }
  }
  
///Оповещение о конце загрузки
  void DocumentComplete (BSTR)
  {
    is_loading = false;
    
    if (!listener)
      return;
    
    listener->OnLoadFinished ();
  }
  
///Оповещение о проблемах в загрузке документа
  void DocumentLoadFailed (DWORD status)
  {
    is_loading = false;
    
    if (!listener)
      return;
    
    if (status & 0x80000000)
    {
      listener->OnLoadFailed (get_com_error_message (status).c_str ());
    }
    else
    {
      stl::string status_string;
      
      switch (status)
      {
        case HTTP_STATUS_BAD_REQUEST:          status_string = "HTTP_STATUS_BAD_REQUEST"; break;
        case HTTP_STATUS_DENIED:               status_string = "HTTP_STATUS_DENIED"; break;
        case HTTP_STATUS_PAYMENT_REQ:          status_string = "HTTP_STATUS_PAYMENT_REQ"; break;
        case HTTP_STATUS_FORBIDDEN:            status_string = "HTTP_STATUS_FORBIDDEN"; break;
        case HTTP_STATUS_NOT_FOUND:            status_string = "HTTP_STATUS_NOT_FOUND"; break;
        case HTTP_STATUS_BAD_METHOD:           status_string = "HTTP_STATUS_BAD_METHOD"; break;
        case HTTP_STATUS_NONE_ACCEPTABLE:      status_string = "HTTP_STATUS_NONE_ACCEPTABLE"; break;
        case HTTP_STATUS_PROXY_AUTH_REQ:       status_string = "HTTP_STATUS_PROXY_AUTH_REQ"; break;
        case HTTP_STATUS_REQUEST_TIMEOUT:      status_string = "HTTP_STATUS_REQUEST_TIMEOUT"; break;
        case HTTP_STATUS_CONFLICT:             status_string = "HTTP_STATUS_CONFLICT"; break;
        case HTTP_STATUS_GONE:                 status_string = "HTTP_STATUS_GONE"; break;
        case HTTP_STATUS_LENGTH_REQUIRED:      status_string = "HTTP_STATUS_LENGTH_REQUIRED"; break;
        case HTTP_STATUS_PRECOND_FAILED:       status_string = "HTTP_STATUS_PRECOND_FAILED"; break;
        case HTTP_STATUS_REQUEST_TOO_LARGE:    status_string = "HTTP_STATUS_REQUEST_TOO_LARGE"; break;
        case HTTP_STATUS_URI_TOO_LONG:         status_string = "HTTP_STATUS_URI_TOO_LONG"; break;
        case HTTP_STATUS_UNSUPPORTED_MEDIA:    status_string = "HTTP_STATUS_UNSUPPORTED_MEDIA"; break;
        case HTTP_STATUS_RETRY_WITH:           status_string = "HTTP_STATUS_RETRY_WITH"; break;
        case HTTP_STATUS_SERVER_ERROR:         status_string = "HTTP_STATUS_SERVER_ERROR"; break;
        case HTTP_STATUS_NOT_SUPPORTED:        status_string = "HTTP_STATUS_NOT_SUPPORTED"; break;
        case HTTP_STATUS_BAD_GATEWAY:          status_string = "HTTP_STATUS_BAD_GATEWAY"; break;
        case HTTP_STATUS_SERVICE_UNAVAIL:      status_string = "HTTP_STATUS_SERVICE_UNAVAIL"; break;
        case HTTP_STATUS_GATEWAY_TIMEOUT:      status_string = "HTTP_STATUS_GATEWAY_TIMEOUT"; break;
        case HTTP_STATUS_VERSION_NOT_SUP:      status_string = "HTTP_STATUS_VERSION_NOT_SUP"; break;
        default:
          status_string = common::format ("HTTP/Moniker error %d", status);
          break;
      }
      
      listener->OnLoadFailed (status_string.c_str ());
    }
  }
};

typedef web_view_handle WebViewImpl;

/*
    Создание/уничтожение web-view
*/

web_view_t WindowsWindowManager::CreateWebView (IWebViewListener* listener)
{
  try
  {
      //статическая инициализация
      
    WebViewInitializerSingleton::Instance ();        
    
      //создание вида
    
    stl::auto_ptr<web_view_handle> view (new web_view_handle (listener));
    
    return view.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CreateWebView");
    throw;
  }
}

void WindowsWindowManager::DestroyWebView (web_view_t handle)
{
  WebViewImpl* view = (WebViewImpl*)handle;
  
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::WindowsWindowManager::DestroyWebView", "handle");

  view->Close ();
  
  delete view;
}

/*
    Присоединенное окно
*/

window_t WindowsWindowManager::GetWindow (web_view_t handle)
{
  WebViewImpl* view = (WebViewImpl*)handle;
  
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::WindowsWindowManager::DestroyWebView", "handle");
    
  return view->host_window;
}

/*
    Загрузка данных
*/

void WindowsWindowManager::LoadRequest (web_view_t handle, const char* request)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    if (!request)
      throw xtl::make_null_argument_exception ("", "request");
      
    view->LoadRequest (request);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::LoadRequest");
    throw;
  }
}

void WindowsWindowManager::LoadData (web_view_t, const char*, size_t, const char*, const char*, const char*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::LoadData");
}

/*
    Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
*/

void WindowsWindowManager::Reload (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    HRESULT hr = view->ibrowser->Refresh ();
    
    if (hr != S_OK)
      throw xtl::format_operation_exception ("", "IWebBrowser2::Refresh failed", hr);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::Reload");
    throw;
  }
}

void WindowsWindowManager::StopLoading (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    HRESULT hr = view->ibrowser->Stop ();
    
    if (hr != S_OK)
      throw xtl::format_operation_exception ("", "IWebBrowser2::Stop failed", hr);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::StopLoading");
    throw;
  }
}

bool WindowsWindowManager::IsLoading (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    return view->is_loading;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::IsLoading");
    throw;
  }
}

/*
    Средства навигации
*/

bool WindowsWindowManager::CanGoBack (web_view_t handle)
{  
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    return view->can_go_back;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CanGoBack");
    throw;
  }
}

bool WindowsWindowManager::CanGoForward (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    return view->can_go_forward;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CanGoForward");
    throw;
  }
}

void WindowsWindowManager::GoBack (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    HRESULT hr = view->ibrowser->GoBack ();
    
    if (hr != S_OK)
      throw xtl::format_operation_exception ("", "IWebBrowser2::GoBack failed", hr);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GoBack");
    throw;
  }
}

void WindowsWindowManager::GoForward (web_view_t handle)
{
  try
  {
    WebViewImpl* view = (WebViewImpl*)handle;
  
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");        
      
    HRESULT hr = view->ibrowser->GoForward ();
    
    if (hr != S_OK)
      throw xtl::format_operation_exception ("", "IWebBrowser2::GoForward failed", hr);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GoForward");
    throw;
  }
}
