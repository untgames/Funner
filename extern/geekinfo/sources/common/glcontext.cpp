#include "glcontext.h"
#include <stdexcept>

namespace geekinfo {

#if defined(PLATFORM_MACOSX)
  
GLContext::GLContext()
: m_ctx(0)
, m_octx(0)
{
  GLint attrib[] = { AGL_RGBA, AGL_NONE };

  AGLPixelFormat pixel_format = aglChoosePixelFormat(NULL, 0, attrib);
  if (!pixel_format) {
    throw std::runtime_error("aglChoosePixelFormat() failed");
  }
  
  m_ctx = aglCreateContext(pixel_format, 0);
  if (!m_ctx || aglGetError() != AGL_NO_ERROR) {
    throw std::runtime_error("aglCreateContext() failed");
  }

  aglDestroyPixelFormat(pixel_format);
  
  m_octx = aglGetCurrentContext();

  if (!aglSetCurrentContext(m_ctx)) {
    throw std::runtime_error("aglSetCurrentContext() failed");
  }
}

GLContext::~GLContext()
{
  aglSetCurrentContext(m_octx);
  if (m_ctx) {
    aglDestroyContext(m_ctx);
  }
}

#elif defined(PLATFORM_WINDOWS)

GLContext::GLContext()
: m_hwnd(0)
, m_dc(0)
, m_rc(0)
{
  WNDCLASS wc;

  ZeroMemory(&wc, sizeof(WNDCLASS));
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = DefWindowProc;
  wc.lpszClassName = "geekinfo";
  if (!RegisterClass(&wc)) {
    throw std::runtime_error("RegisterClass() failed");
  }
  
  m_hwnd = CreateWindow("geekinfo", "geekinfo", 0, CW_USEDEFAULT, CW_USEDEFAULT,
    CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
  
  if (!m_hwnd) {
    throw std::runtime_error("CreateWindow() failed");
  }

  m_dc = GetDC(m_hwnd);
  if (!m_dc) {
    throw std::runtime_error("GetDC() failed");
  }

  PIXELFORMATDESCRIPTOR pfd;
  
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  int pf = ChoosePixelFormat(m_dc, &pfd);
  if (!pf) {
    throw std::runtime_error("ChoosePixelFormat() failed");
  }
  
  if (!SetPixelFormat(m_dc, pf, &pfd)) {
    throw std::runtime_error("SetPixelFormat() failed");
  }
  
  m_rc = wglCreateContext(m_dc);
  if (!m_rc) {
    throw std::runtime_error("wglCreateContext() failed");
  }
  
  if (!wglMakeCurrent(m_dc, m_rc)) {
    throw std::runtime_error("wglMakeCurrent() failed");
  }
}

GLContext::~GLContext()
{
  if (m_rc) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(wglGetCurrentContext());
  }
  if (m_hwnd) {
    if (m_dc) {
      ReleaseDC(m_hwnd, m_dc);
    }
    DestroyWindow(m_hwnd);
  }
  UnregisterClass("geekinfo", GetModuleHandle(NULL));
}

#else

GLContext::GLContext()
{
  // TODO: Throw exception since this isn't implemented
}

GLContext::~GLContext()
{
}

#endif

} // namespace geekinfo 
