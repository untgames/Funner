#ifndef RENDER_GL_DRIVER_GLX_TYPES_HEADER
#define RENDER_GL_DRIVER_GLX_TYPES_HEADER

#include <cstddef>

#define GLX_VERSION_1_1   1
#define GLX_VERSION_1_2   1
#define GLX_VERSION_1_3   1
#define GLX_VERSION_1_4   1

#define GLX_EXTENSION_NAME   "GLX"

/*
 * Tokens for glXChooseVisual and glXGetConfig:
 */
#define GLX_USE_GL    1
#define GLX_BUFFER_SIZE   2
#define GLX_LEVEL   3
#define GLX_RGBA    4
#define GLX_DOUBLEBUFFER  5
#define GLX_STEREO    6
#define GLX_AUX_BUFFERS   7
#define GLX_RED_SIZE    8
#define GLX_GREEN_SIZE    9
#define GLX_BLUE_SIZE   10
#define GLX_ALPHA_SIZE    11
#define GLX_DEPTH_SIZE    12
#define GLX_STENCIL_SIZE  13
#define GLX_ACCUM_RED_SIZE  14
#define GLX_ACCUM_GREEN_SIZE  15
#define GLX_ACCUM_BLUE_SIZE 16
#define GLX_ACCUM_ALPHA_SIZE  17


/*
 * Error codes returned by glXGetConfig:
 */
#define GLX_BAD_SCREEN    1
#define GLX_BAD_ATTRIBUTE 2
#define GLX_NO_EXTENSION  3
#define GLX_BAD_VISUAL    4
#define GLX_BAD_CONTEXT   5
#define GLX_BAD_VALUE         6
#define GLX_BAD_ENUM    7


/*
 * GLX 1.1 and later:
 */
#define GLX_VENDOR    1
#define GLX_VERSION   2
#define GLX_EXTENSIONS    3


/*
 * GLX 1.3 and later:
 */
#define GLX_CONFIG_CAVEAT   0x20
#define GLX_DONT_CARE     0xFFFFFFFF
#define GLX_X_VISUAL_TYPE   0x22
#define GLX_TRANSPARENT_TYPE    0x23
#define GLX_TRANSPARENT_INDEX_VALUE 0x24
#define GLX_TRANSPARENT_RED_VALUE 0x25
#define GLX_TRANSPARENT_GREEN_VALUE 0x26
#define GLX_TRANSPARENT_BLUE_VALUE  0x27
#define GLX_TRANSPARENT_ALPHA_VALUE 0x28
#define GLX_WINDOW_BIT      0x00000001
#define GLX_PIXMAP_BIT      0x00000002
#define GLX_PBUFFER_BIT     0x00000004
#define GLX_AUX_BUFFERS_BIT   0x00000010
#define GLX_FRONT_LEFT_BUFFER_BIT 0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT  0x00000002
#define GLX_BACK_LEFT_BUFFER_BIT  0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT 0x00000008
#define GLX_DEPTH_BUFFER_BIT    0x00000020
#define GLX_STENCIL_BUFFER_BIT    0x00000040
#define GLX_ACCUM_BUFFER_BIT    0x00000080
#define GLX_NONE      0x8000
#define GLX_SLOW_CONFIG     0x8001
#define GLX_TRUE_COLOR      0x8002
#define GLX_DIRECT_COLOR    0x8003
#define GLX_PSEUDO_COLOR    0x8004
#define GLX_STATIC_COLOR    0x8005
#define GLX_GRAY_SCALE      0x8006
#define GLX_STATIC_GRAY     0x8007
#define GLX_TRANSPARENT_RGB   0x8008
#define GLX_TRANSPARENT_INDEX   0x8009
#define GLX_VISUAL_ID     0x800B
#define GLX_SCREEN      0x800C
#define GLX_NON_CONFORMANT_CONFIG 0x800D
#define GLX_DRAWABLE_TYPE   0x8010
#define GLX_RENDER_TYPE     0x8011
#define GLX_X_RENDERABLE    0x8012
#define GLX_FBCONFIG_ID     0x8013
#define GLX_RGBA_TYPE     0x8014
#define GLX_COLOR_INDEX_TYPE    0x8015
#define GLX_MAX_PBUFFER_WIDTH   0x8016
#define GLX_MAX_PBUFFER_HEIGHT    0x8017
#define GLX_MAX_PBUFFER_PIXELS    0x8018
#define GLX_PRESERVED_CONTENTS    0x801B
#define GLX_LARGEST_PBUFFER   0x801C
#define GLX_WIDTH     0x801D
#define GLX_HEIGHT      0x801E
#define GLX_EVENT_MASK      0x801F
#define GLX_DAMAGED     0x8020
#define GLX_SAVED     0x8021
#define GLX_WINDOW      0x8022
#define GLX_PBUFFER     0x8023
#define GLX_PBUFFER_HEIGHT              0x8040
#define GLX_PBUFFER_WIDTH               0x8041
#define GLX_RGBA_BIT      0x00000001
#define GLX_COLOR_INDEX_BIT   0x00000002
#define GLX_PBUFFER_CLOBBER_MASK  0x08000000


/*
 * GLX 1.4 and later:
 */
#define GLX_SAMPLE_BUFFERS              0x186a0 /*100000*/
#define GLX_SAMPLES                     0x186a1 /*100001*/

typedef struct __GLXcontextRec *GLXContext;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
/* GLX 1.3 and later */
typedef struct __GLXFBConfigRec *GLXFBConfig;
typedef XID GLXFBConfigID;
typedef XID GLXContextID;
typedef XID GLXWindow;
typedef XID GLXPbuffer;

/*
** Events.
** __GLX_NUMBER_EVENTS is set to 17 to account for the BufferClobberSGIX
**  event - this helps initialization if the server supports the pbuffer
**  extension and the client doesn't.
*/
#define GLX_PbufferClobber  0
#define GLX_BufferSwapComplete  1

#define __GLX_NUMBER_EVENTS 17

/*** Should these go here, or in another header? */
/*
** GLX Events
*/
typedef struct {
    int event_type;   /* GLX_DAMAGED or GLX_SAVED */
    int draw_type;    /* GLX_WINDOW or GLX_PBUFFER */
    unsigned long serial; /* # of last request processed by server */
    Bool send_event;    /* true if this came for SendEvent request */
    Display *display;   /* display the event was read from */
    GLXDrawable drawable; /* XID of Drawable */
    unsigned int buffer_mask; /* mask indicating which buffers are affected */
    unsigned int aux_buffer;  /* which aux buffer was affected */
    int x, y;
    int width, height;
    int count;      /* if nonzero, at least this many more */
} GLXPbufferClobberEvent;

typedef struct {
    int type;
    unsigned long serial; /* # of last request processed by server */
    Bool send_event;    /* true if this came from a SendEvent request */
    Display *display;   /* Display the event was read from */
    GLXDrawable drawable; /* drawable on which event was requested in event mask */
    int event_type;
    int64_t ust;
    int64_t msc;
    int64_t sbc;
} GLXBufferSwapComplete;

typedef union __GLXEvent {
    GLXPbufferClobberEvent glxpbufferclobber;
    GLXBufferSwapComplete glxbufferswapcomplete;
    long pad[24];
} GLXEvent;

/* GLX 1.3 function pointer typedefs */
typedef GLXFBConfig * (* PFNGLXGETFBCONFIGSPROC) (Display *dpy, int screen, int *nelements);
typedef GLXFBConfig * (* PFNGLXCHOOSEFBCONFIGPROC) (Display *dpy, int screen, const int *attrib_list, int *nelements);
typedef int (* PFNGLXGETFBCONFIGATTRIBPROC) (Display *dpy, GLXFBConfig config, int attribute, int *value);
typedef XVisualInfo * (* PFNGLXGETVISUALFROMFBCONFIGPROC) (Display *dpy, GLXFBConfig config);
typedef GLXWindow (* PFNGLXCREATEWINDOWPROC) (Display *dpy, GLXFBConfig config, Window win, const int *attrib_list);
typedef void (* PFNGLXDESTROYWINDOWPROC) (Display *dpy, GLXWindow win);
typedef GLXPixmap (* PFNGLXCREATEPIXMAPPROC) (Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list);
typedef void (* PFNGLXDESTROYPIXMAPPROC) (Display *dpy, GLXPixmap pixmap);
typedef GLXPbuffer (* PFNGLXCREATEPBUFFERPROC) (Display *dpy, GLXFBConfig config, const int *attrib_list);
typedef void (* PFNGLXDESTROYPBUFFERPROC) (Display *dpy, GLXPbuffer pbuf);
typedef void (* PFNGLXQUERYDRAWABLEPROC) (Display *dpy, GLXDrawable draw, int attribute, unsigned int *value);
typedef GLXContext (* PFNGLXCREATENEWCONTEXTPROC) (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
typedef Bool (* PFNGLXMAKECONTEXTCURRENTPROC) (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef GLXDrawable (* PFNGLXGETCURRENTREADDRAWABLEPROC) (void);
typedef Display * (* PFNGLXGETCURRENTDISPLAYPROC) (void);
typedef int (* PFNGLXQUERYCONTEXTPROC) (Display *dpy, GLXContext ctx, int attribute, int *value);
typedef void (* PFNGLXSELECTEVENTPROC) (Display *dpy, GLXDrawable draw, unsigned long event_mask);
typedef void (* PFNGLXGETSELECTEDEVENTPROC) (Display *dpy, GLXDrawable draw, unsigned long *event_mask);

typedef void (*__GLXextFuncPtr)(void);

/* GLX 1.4 function pointer typedefs */
typedef __GLXextFuncPtr (* PFNGLXGETPROCADDRESSPROC) (const GLubyte *procName);

typedef void * ( * PFNGLXALLOCATEMEMORYNVPROC) (GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
typedef void ( * PFNGLXFREEMEMORYNVPROC) (GLvoid *pointer);

typedef int (*PFNGLXGETFRAMEUSAGEMESAPROC) (Display *dpy, GLXDrawable drawable, float *usage);
typedef int (*PFNGLXBEGINFRAMETRACKINGMESAPROC)(Display *dpy, GLXDrawable drawable);
typedef int (*PFNGLXENDFRAMETRACKINGMESAPROC)(Display *dpy, GLXDrawable drawable);
typedef int (*PFNGLXQUERYFRAMETRACKINGMESAPROC)(Display *dpy, GLXDrawable drawable, int64_t *swapCount, int64_t *missedFrames, float *lastMissedUsage);

typedef int (*PFNGLXSWAPINTERVALMESAPROC)(unsigned int interval);
typedef int (*PFNGLXGETSWAPINTERVALMESAPROC)(void);

#endif
