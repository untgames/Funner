#include "shared.h"

#import <WebKit/CarbonUtils.h>
#import <WebKit/HIWebView.h>
#import <WebKit/WebFrame.h>
#import <WebKit/WebPolicyDelegate.h>
#import <WebKit/WebView.h>

using namespace syslib;
using namespace syslib::macosx;

@interface WebViewDelegate : NSObject
{
  @private
    IWebViewListener *listener;
}

@end

@implementation WebViewDelegate

-(id)initWithListener:(IWebViewListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  listener = in_listener;

  return self;
}

-(void)webView:(WebView *)webView decidePolicyForMIMEType:(NSString *)type request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id < WebPolicyDecisionListener >)policy_listener
{
  if (listener->ShouldStartLoading ([[[request URL] absoluteString] UTF8String]))
    [policy_listener use];
  else
    [policy_listener ignore];
}

-(void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame
{
  listener->OnLoadStarted ([[[[[frame dataSource] request] URL] absoluteString] UTF8String]);
}

-(void)webView:(WebView *)sender willPerformClientRedirectToURL:(NSURL *)URL delay:(NSTimeInterval)seconds fireDate:(NSDate *)date forFrame:(WebFrame *)frame
{
  listener->OnLoadStarted ([[URL absoluteString] UTF8String]);
}

-(void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
  listener->OnLoadFinished ();
}

-(void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
  listener->OnLoadFailed ([[error description] UTF8String]);
}

-(void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
  listener->OnLoadFailed ([[error description] UTF8String]);
}

@end

struct syslib::web_view_handle
{
  IWebViewListener *listener;
  WindowRef        window;
  HIViewRef        web_view;
  WebView          *ns_web_view;
  WebViewDelegate  *delegate;

  web_view_handle (IWebViewListener *in_listener)
    : listener (in_listener)
    , window (0)
    , web_view (0)
    , ns_web_view (0)
    , delegate (0)
  {
    try
    {
      ::Rect window_rect = {0, 0, 400, 600};

      check_window_manager_error (CreateNewWindow (kFloatingWindowClass, kWindowNoShadowAttribute | kWindowStandardHandlerAttribute | kWindowCompositingAttribute, &window_rect, &window), "::CreateNewWindow",
                                  "Can't create window");

      RepositionWindow (window, 0, kWindowCenterOnMainScreen);

      check_window_manager_error (HIWebViewCreate (&web_view), "::HIWebViewCreate", "Can't create web view");

      ns_web_view = HIWebViewGetWebView (web_view);

      if (listener)
      {
        delegate = [[WebViewDelegate alloc] initWithListener:listener];

        [ns_web_view setPolicyDelegate:delegate];
        [ns_web_view setFrameLoadDelegate:delegate];
      }

      HIViewRef root_view = HIViewGetRoot (window),
                content_view;

      if (!root_view)
        throw xtl::format_operation_exception ("::HIViewGetRoot", "Can't get root window view");

      check_window_manager_error (HIViewFindByID (root_view, kHIViewWindowContentID, &content_view), "::HIViewFindByID", "Can't find content view");

      HIRect bounds;

      check_window_manager_error (HIViewGetBounds (content_view, &bounds), "::HIViewGetBounds", "Can't get window bounds");
      check_window_manager_error (HIViewSetFrame (web_view, &bounds), "::HIViewSetFrame", "Can't set web view frame");

      HILayoutInfo layout_info;

      memset (&layout_info, 0, sizeof (layout_info));

      layout_info.version             = kHILayoutInfoVersionZero;
      layout_info.binding.top.kind    = kHILayoutBindTop;
      layout_info.binding.bottom.kind = kHILayoutBindBottom;
      layout_info.binding.left.kind   = kHILayoutBindLeft;
      layout_info.binding.right.kind  = kHILayoutBindRight;
      layout_info.scale.x.kind        = kHILayoutScaleAbsolute;
      layout_info.scale.y.kind        = kHILayoutScaleAbsolute;
      layout_info.position.x.kind     = kHILayoutPositionMin;
      layout_info.position.y.kind     = kHILayoutPositionMin;

      check_window_manager_error (HIViewSetLayoutInfo (web_view, &layout_info), "::HIViewSetLayoutInfo", "Can't set web view layout info");

      check_window_manager_error (HIViewAddSubview (content_view, web_view), "::HIViewAddSubview", "Can't add web view to window");
      check_window_manager_error (HIViewSetVisible (web_view, true), "::HIViewSetVisible", "Can't show web_view");
    }
    catch (xtl::exception& e)
    {
      if (web_view)
        CFRelease (web_view);

      if (window)
        DisposeWindow (window);

      [delegate release];

      e.touch ("syslib::CarbonWindowManager::CreateWebView");
      throw;
    }
  }

  ~web_view_handle ()
  {
    [ns_web_view setPolicyDelegate:nil];
    [ns_web_view setFrameLoadDelegate:nil];

    if (web_view)
      CFRelease (web_view);

    if (window)
      DisposeWindow (window);

    [delegate release];
  }
};

/*-(void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
  listener->OnLoadFailed ([[error description] UTF8String]);
}

-(BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
  return listener->ShouldStartLoading ([[request.URL absoluteString] UTF8String]);
}

-(void)webViewDidFinishLoad:(UIWebView *)webView
{
  listener->OnLoadFinished ();
}

-(void)webViewDidStartLoad:(UIWebView *)webView
{
  listener->OnLoadStarted ([[webView.request.URL absoluteString] UTF8String]);
}*/

/*
   Создание/уничтожение web-view
*/

web_view_t CarbonWindowManager::CreateWebView (IWebViewListener* listener)
{
  WebInitForCarbon ();

  return new web_view_handle (listener);
}

void CarbonWindowManager::DestroyWebView (web_view_t handle)
{
  delete handle;
}

/*
   Присоединенное окно
*/

window_t CarbonWindowManager::GetWindow (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::GetWindow", "handle");

  return (window_t)handle->window;
}

/*
   Загрузка данных
*/

void CarbonWindowManager::LoadRequest (web_view_t handle, const char* uri)
{
  static const char* METHOD_NAME = "syslib::CarbonWindowManager::LoadRequest";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (!uri)
    throw xtl::make_null_argument_exception (METHOD_NAME, "uri");

  NSURL* url = [[NSURL alloc] initWithString:[NSString stringWithUTF8String:uri]];

  if (!url)
    throw xtl::make_argument_exception (METHOD_NAME, "uri", uri);

  NSURLRequest* request = [[NSURLRequest alloc] initWithURL:url];

  [url release];

  [[handle->ns_web_view mainFrame] loadRequest:request];

  [request release];
}

void CarbonWindowManager::LoadData (web_view_t handle, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url)
{
  static const char* METHOD_NAME = "syslib::CarbonWindowManager::LoadData";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (data_size && !data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

  if (!mime_type)
    throw xtl::make_null_argument_exception (METHOD_NAME, "mime_type");

  if (!encoding)
    throw xtl::make_null_argument_exception (METHOD_NAME, "encoding");

  if (!base_url)
    throw xtl::make_null_argument_exception (METHOD_NAME, "base_url");

  try
  {
    NSURL *ns_base_url = [NSURL URLWithString:[NSString stringWithUTF8String:base_url]];

    if (!ns_base_url)
      throw xtl::make_argument_exception ("", "base_url", base_url);

    NSData *ns_data = [NSData dataWithBytes:data length:data_size];

    [[handle->ns_web_view mainFrame] loadData:ns_data MIMEType:[NSString stringWithUTF8String:mime_type] textEncodingName:[NSString stringWithUTF8String:encoding] baseURL:ns_base_url];
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
   Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
*/

void CarbonWindowManager::Reload (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::Reload", "handle");

  [handle->ns_web_view reload:handle->ns_web_view];
}

void CarbonWindowManager::StopLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::StopLoading", "handle");

  [handle->ns_web_view stopLoading:handle->ns_web_view];
}

bool CarbonWindowManager::IsLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::IsLoading", "handle");

  return [handle->ns_web_view isLoading];
}

/*
   Средства навигации
*/

bool CarbonWindowManager::CanGoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::CanGoBack", "handle");

  return [handle->ns_web_view canGoBack];
}

bool CarbonWindowManager::CanGoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::CanGoForward", "handle");

  return [handle->ns_web_view canGoForward];
}

void CarbonWindowManager::GoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::GoBack", "handle");

  [handle->ns_web_view goBack];
}

void CarbonWindowManager::GoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonWindowManager::GoForward", "handle");

  [handle->ns_web_view goForward];
}
