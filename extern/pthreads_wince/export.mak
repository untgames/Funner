export.compile.extern.pthreads_wince.COMPILER_DEFINES := __CLEANUP_C PTW32_STATIC_LIB HAVE_PTW32_CONFIG_H
export.compile.extern.pthreads_wince.INCLUDE_DIRS     := include

export.link.extern.pthreads_wince.LIBS    := funner.extern.pthreads_wince ws2
export.link.extern.pthreads_wince.IMPORTS := link.extern.wcecompat
