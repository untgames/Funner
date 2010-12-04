/* include/IL/config.h.in.  Generated from configure.ac by autoheader.  */

/* Altivec extension found */
#undef ALTIVEC_GCC

/* "Enable debug code features" */
#undef DEBUG

/* PPC_ASM assembly found */
#undef GCC_PCC_ASM

/* X86_64_ASM assembly found */
#undef GCC_X86_64_ASM

/* X86_ASM assembly found */
#undef GCC_X86_ASM

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <GLUT/glut.h> header file. */
#undef HAVE_GLUT_GLUT_H

/* Define to 1 if you have the <GL/glut.h> header file. */
#undef HAVE_GL_GLUT_H

/* Define to 1 if you have the <GL/glu.h> header file. */
#undef HAVE_GL_GLU_H

/* Define to 1 if you have the <GL/gl.h> header file. */
#undef HAVE_GL_GL_H

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
#undef HAVE_LIBM

/* Define to 1 if you have the `z' library (-lz). */
#undef HAVE_LIBZ

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <OpenGL/glu.h> header file. */
#undef HAVE_OPENGL_GLU_H

/* Define to 1 if you have the <OpenGL/gl.h> header file. */
#undef HAVE_OPENGL_GL_H

/* Define if you have POSIX threads libraries and header files. */
#undef HAVE_PTHREAD //???????????

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Use nonstandard varargs form for the GLU tesselator callback */
#undef HAVE_VARARGS_GLU_TESSCB

/* Define to 1 if you have the <windows.h> header file. */
#undef HAVE_WINDOWS_H

/* Support Allegro API */
#undef ILUT_USE_ALLEGRO

/* Support DirectX8 API */
#undef ILUT_USE_DIRECTX8

/* Support DirectX9 API */
#undef ILUT_USE_DIRECTX9

/* Define if system supports OpenGL API */
#undef ILUT_USE_OPENGL

/* Support SDL API */
#undef ILUT_USE_SDL

/* Support X11 API */
#undef ILUT_USE_X11

/* Support X11 XRender extension */
#undef ILUT_USE_XRENDER

/* Support X11 XShm extension */
#undef ILUT_USE_XSHM

/* Define if you want to build ILU part of DevIL (used in DevIL
   examples/tests) */
#define ILU_ENABLED

/* Define if you can support at least some ASM */
#undef IL_INLINE_ASM

/* bmp support */
#undef IL_NO_BMP

/* dcx support */
#define IL_NO_DCX

/* exr support */
#define IL_NO_EXR

/* gif support */
#define IL_NO_GIF

/* hdr support */
#define IL_NO_HDR

/* icns support */
#define IL_NO_ICNS

/* icon support */
#define IL_NO_ICON

/* iff support */
#define IL_NO_IFF

/* jp2 support */
#define IL_NO_JP2

/* jpeg support */
#undef IL_NO_JPG

/* lcms support */
#define IL_NO_LCMS

/* lif support */
#define IL_NO_LIF

/* mdl support */
#define IL_NO_MDL

/* mng support */
#define IL_NO_MNG

/* pcd support */
#define IL_NO_PCD

/* pcx support */
#define IL_NO_PCX

/* pic support */
#define IL_NO_PIC

/* pix support */
#define IL_NO_PIX

/* png support */
#undef IL_NO_PNG

/* pnm support */
#define IL_NO_PNM

/* psd support */
#undef IL_NO_PSD

/* psp support */
#define IL_NO_PSP

/* pxr support */
#define IL_NO_PXR

/* raw support */
#define IL_NO_RAW

/* sgi support */
#define IL_NO_SGI

/* tga support */
#undef IL_NO_TGA

/* tiff support */
#undef IL_NO_TIF

/* doom support */
#define IL_NO_WAD

/* wal support */
#define IL_NO_WAL

/* wdp support */
#define IL_NO_WDP

/* xpm support */
#define IL_NO_XPM

/* Define if you have nvidia texture tools library installed */
#undef IL_USE_DXTC_NVIDIA

/* Define if you have libsquish installed */
#undef IL_USE_DXTC_SQUISH

/* Use libjpeg without modification. always enabled. */
#define IL_USE_JPEGLIB_UNMODIFIED

/* LCMS include without lcms/ support */
#define LCMS_NODIRINCLUDE

/* Building on Mac OS X */
#ifdef __APPLE__
#define MAX_OS_X
#else
#undef MAX_OS_X
#endif

/* define if you have memalign memory allocation */
#undef MEMALIGN

/* define if you have mm_malloc.h header */
#undef MM_MALLOC

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
#undef NO_MINUS_C_MINUS_O

/* Name of package */
#define PACKAGE "DevIL"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "DevIL"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "DevIL 1.7.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7.7"

/* define if you have posix_memalign memory allocation */
#undef POSIX_MEMALIGN

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
#undef PTHREAD_CREATE_JOINABLE

/* restric keyword available */
#undef RESTRICT_KEYWORD

/* SSE extension found */
#undef SSE

/* SSE2 extension found */
#undef SSE2

/* SSE3 extension found */
#undef SSE3

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* define if you have valloc memory allocation */
#undef VALLOC

/* Memory must be vector aligned */
#undef VECTORMEM

/* Version number of package */
#define VERSION "1.7.7"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1
