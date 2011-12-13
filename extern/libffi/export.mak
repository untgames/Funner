export.compile.extern.libffi.INCLUDE_DIRS               := include
export.compile.extern.libffi.x86.INCLUDE_DIRS           := sources/x86
export.compile.extern.libffi.arm.INCLUDE_DIRS           := sources/arm
export.compile.extern.libffi.x86_win32.COMPILER_DEFINES := X86_WIN32
export.compile.extern.libffi.msvc.COMPILER_DEFINES      := HAVE_LONG_LONG=0
export.compile.extern.libffi.macosx.COMPILER_DEFINES    := HAVE_LONG_LONG=1
export.compile.extern.libffi.arm.COMPILER_DEFINES       := HAVE_LONG_LONG=1

export.link.extern.libffi.LIBS := funner.extern.libffi
