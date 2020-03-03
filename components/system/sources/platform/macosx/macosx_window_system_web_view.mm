#import <WebKit/WebFrame.h>
#import <WebKit/WebFrameLoadDelegate.h>
#import <WebKit/WebPolicyDelegate.h>
#import <WebKit/WebView.h>

#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

@interface NSWindowWrapper : NSWindow
{
}

@end

@interface WebViewDelegate : NSObject<WebPolicyDelegate, WebFrameLoadDelegate>
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
  NSWindowWrapper  *window;
  WebView          *ns_web_view;
  WebViewDelegate  *delegate;

  web_view_handle (IWebViewListener *in_listener)
    : listener (in_listener)
    , window (0)
    , ns_web_view (0)
    , delegate (0)
  {
    try
    {
      NSRect window_rect = NSMakeRect (0, 0, 400, 600);

      window = [[NSWindowWrapper alloc] initWithContentRect:window_rect
                                                  styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                                                    backing:NSBackingStoreBuffered
                                                      defer:true];

      ns_web_view = [[WebView alloc] initWithFrame:window_rect
                                         frameName:nil
                                         groupName:nil];

      if (listener)
      {
        delegate = [[WebViewDelegate alloc] initWithListener:listener];

        [ns_web_view setPolicyDelegate:delegate];
        [ns_web_view setFrameLoadDelegate:delegate];
      }

      window.contentView = ns_web_view;
    }
    catch (xtl::exception& e)
    {
      [ns_web_view release];
      [window release];
      [delegate release];

      e.touch ("syslib::MacOSXWindowManager::CreateWebView");
      throw;
    }
  }

  ~web_view_handle ()
  {
    [ns_web_view setPolicyDelegate:nil];
    [ns_web_view setFrameLoadDelegate:nil];
    [ns_web_view release];
    [window release];

    [delegate release];
  }
};

/*
   Создание/уничтожение web-view
*/

web_view_t MacOSXWindowManager::CreateWebView (IWebViewListener* listener)
{
  return new web_view_handle (listener);
}

void MacOSXWindowManager::DestroyWebView (web_view_t handle)
{
  delete handle;
}

/*
   Присоединенное окно
*/

window_t MacOSXWindowManager::GetWindow (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::GetWindow", "handle");

  return (window_t)handle->window;
}

/*
   Загрузка данных
*/

void MacOSXWindowManager::LoadRequest (web_view_t handle, const char* uri)
{
  static const char* METHOD_NAME = "syslib::MacOSXWindowManager::LoadRequest";

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

void MacOSXWindowManager::LoadData (web_view_t handle, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url)
{
  static const char* METHOD_NAME = "syslib::MacOSXWindowManager::LoadData";

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

void MacOSXWindowManager::Reload (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::Reload", "handle");

  [handle->ns_web_view reload:handle->ns_web_view];
}

void MacOSXWindowManager::StopLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::StopLoading", "handle");

  [handle->ns_web_view stopLoading:handle->ns_web_view];
}

bool MacOSXWindowManager::IsLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::IsLoading", "handle");

  return [handle->ns_web_view isLoading];
}

/*
   Средства навигации
*/

bool MacOSXWindowManager::CanGoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::CanGoBack", "handle");

  return [handle->ns_web_view canGoBack];
}

bool MacOSXWindowManager::CanGoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::CanGoForward", "handle");

  return [handle->ns_web_view canGoForward];
}

void MacOSXWindowManager::GoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::GoBack", "handle");

  [handle->ns_web_view goBack];
}

void MacOSXWindowManager::GoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::MacOSXWindowManager::GoForward", "handle");

  [handle->ns_web_view goForward];
}
