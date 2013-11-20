#include "shared.h"

#import <UIKit/UIActivityIndicatorView.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIWebView.h>

using namespace syslib;

@interface UIWebViewWrapper : UIWebView <UIWebViewDelegate>
{
  @private
    IWebViewListener*        listener;
    UIActivityIndicatorView* activity_indicator;
}

-(id)initWithListener:(IWebViewListener*)in_listener;

@end

@implementation UIWebViewWrapper

-(void)dealloc
{
  [activity_indicator release];

  [super dealloc];
}

-(id)initWithListener:(IWebViewListener*)in_listener
{
  self = [super initWithFrame:CGRectMake (-2, -2, 1, 1)];

  if (!self)
    return nil;

  listener = in_listener;

  if (listener)
    self.delegate = self;

  self.hidden = YES;

  self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
  self.scalesPageToFit  = YES;

  activity_indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];

  [activity_indicator startAnimating];

  [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:self];

  return self;
}

-(void)layoutSubviews
{
  [super layoutSubviews];

  CGRect activity_indicator_frame = activity_indicator.frame;

  activity_indicator_frame.origin.x = floor (self.bounds.size.width / 2 - activity_indicator_frame.size.width / 2);
  activity_indicator_frame.origin.y = floor (self.bounds.size.height / 2 - activity_indicator_frame.size.height / 2);

  activity_indicator.frame = activity_indicator_frame;
}

-(void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
  if ([error code] == NSURLErrorCancelled) //web view was redirected to another URL or load was cancelled by user
    return;

  listener->OnLoadFailed ([[error description] UTF8String]);

  [activity_indicator removeFromSuperview];
}

-(BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
  return listener->ShouldStartLoading ([[request.URL absoluteString] UTF8String]);
}

-(void)webViewDidFinishLoad:(UIWebView *)webView
{
  listener->OnLoadFinished ();

  [activity_indicator removeFromSuperview];
}

-(void)webViewDidStartLoad:(UIWebView *)webView
{
  if (webView.request.URL)
    listener->OnLoadStarted ([[webView.request.URL absoluteString] UTF8String]);

  [self layoutSubviews];

  [self addSubview:activity_indicator];
}

@end

/*
   Создание/уничтожение web-view
*/

web_view_t IPhoneWindowManager::CreateWebView (IWebViewListener* listener)
{
  return (web_view_t)[[UIWebViewWrapper alloc] initWithListener:listener];
}

void IPhoneWindowManager::DestroyWebView (web_view_t handle)
{
  UIWebView* web_view = (UIWebView*)handle;

  [web_view removeFromSuperview];
  [web_view release];
}

/*
   Присоединенное окно
*/

window_t IPhoneWindowManager::GetWindow (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetWindow", "handle");

  return (window_t)handle;
}

/*
   Загрузка данных
*/

void IPhoneWindowManager::LoadRequest (web_view_t handle, const char* uri)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::LoadRequest";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (!uri)
    throw xtl::make_null_argument_exception (METHOD_NAME, "uri");

  NSURL* url = [[NSURL alloc] initWithString:[NSString stringWithUTF8String:uri]];

  if (!url)
    throw xtl::make_argument_exception (METHOD_NAME, "uri", uri);

  NSURLRequest* request = [[NSURLRequest alloc] initWithURL:url];

  [url release];

  UIWebView* web_view = (UIWebView*)handle;

  [web_view loadRequest:request];

  [request release];
}

void IPhoneWindowManager::LoadData (web_view_t handle, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::LoadData";

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

    [(UIWebView*)handle loadData:ns_data MIMEType:[NSString stringWithUTF8String:mime_type] textEncodingName:[NSString stringWithUTF8String:encoding] baseURL:ns_base_url];
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

void IPhoneWindowManager::Reload (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::Reload", "handle");

  [(UIWebView*)handle reload];
}

void IPhoneWindowManager::StopLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::StopLoading", "handle");

  [(UIWebView*)handle stopLoading];
}

bool IPhoneWindowManager::IsLoading (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::IsLoading", "handle");

  return ((UIWebView*)handle).loading;
}

/*
   Средства навигации
*/

bool IPhoneWindowManager::CanGoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::CanGoBack", "handle");

  return ((UIWebView*)handle).canGoBack;
}

bool IPhoneWindowManager::CanGoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::CanGoForward", "handle");

  return ((UIWebView*)handle).canGoForward;
}

void IPhoneWindowManager::GoBack (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GoBack", "handle");

  [(UIWebView*)handle goBack];
}

void IPhoneWindowManager::GoForward (web_view_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GoForward", "handle");

  [(UIWebView*)handle goForward];
}
