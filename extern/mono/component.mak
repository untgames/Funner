###################################################################################################
#Список целей
###################################################################################################
TARGETS += EXTERN.MONO.EGLIB
TARGETS += EXTERN.MONO.LIBGC
TARGETS += EXTERN.MONO.LIBMONO
TARGETS += EXTERN.MONO_CS_INVOKE
TARGETS += EXTERN.MONO_TESTS

#Цель - EGLIB
EXTERN.MONO.EGLIB.TYPE                   := static-lib
EXTERN.MONO.EGLIB.NAME                   := funner.extern.mono.eglib
EXTERN.MONO.EGLIB.INCLUDE_DIRS           := sources sources/eglib sources/eglib/src
EXTERN.MONO.EGLIB.SOURCE_DIRS            := sources/eglib/src
EXTERN.MONO.EGLIB.LIB_DIRS               :=
EXTERN.MONO.EGLIB.LIBS                   :=
EXTERN.MONO.EGLIB.COMPILER_DEFINES       :=
EXTERN.MONO.EGLIB.x86.COMPILER_DEFINES   := __i386__ TARGET_X86 i386
EXTERN.MONO.EGLIB.win32.COMPILER_DEFINES := UNICODE _UNICODE WIN32 _WIN32 __WIN32__ _WINDOWS WINDOWS HOST_WIN32 TARGET_WIN32
EXTERN.MONO.EGLIB.win32.SOURCE_DIRS      := sources/eglib/src/win32
EXTERN.MONO.EGLIB.unistd.SOURCE_DIRS     := sources/eglib/src/unix

#Цель - LibGC
EXTERN.MONO.LIBGC.TYPE                   := static-lib
EXTERN.MONO.LIBGC.NAME                   := funner.extern.mono.libgc
EXTERN.MONO.LIBGC.INCLUDE_DIRS           := sources sources/libgc sources/libgc/include
EXTERN.MONO.LIBGC.SOURCE_DIRS            := sources/libgc
EXTERN.MONO.LIBGC.LIB_DIRS               :=
EXTERN.MONO.LIBGC.LIBS                   :=
EXTERN.MONO.LIBGC.COMPILER_DEFINES       += __STDC__ PACKAGE_NAME="libgc-mono" PACKAGE_TARNAME="libgc-mono" PACKAGE_BUGREPORT="Hans_Boehm%40hp.com" GC_INSIDE_DLL=1 GC_NOT_DLL=1 JAVA_FINALIZATION=1 GC_GCJ_SUPPORT=1 ATOMIC_UNCOLLECTABLE=1 _IN_LIBGC=1 FD_SETSIZE=1024
EXTERN.MONO.LIBGC.COMPILER_DEFINES       += PACKAGE_VERSION="6.6"
EXTERN.MONO.LIBGC.COMPILER_DEFINES       += PACKAGE_STRING="libgc-mono 6.6"
EXTERN.MONO.LIBGC.x86.COMPILER_DEFINES   := __i386__ TARGET_X86 i386 
EXTERN.MONO.LIBGC.win32.COMPILER_DEFINES := WIN32_THREADS _WIN32_IE=0x0501 _WIN32_WINNT=0x0500 WINVER=0x0500 UNICODE _UNICODE WIN32 _WIN32 __WIN32__ _WINDOWS WINDOWS HOST_WIN32 TARGET_WIN32 NO_GETENV=1
EXTERN.MONO.LIBGC.win32.COMPILER_DEFINES += HAVE_SYS_TYPES_H=1 HAVE_SYS_STAT_H=1 HAVE_STDLIB_H=1 HAVE_STRING_H=1 HAVE_MEMORY_H=1 HAVE_STRINGS_H=1 HAVE_INTTYPES_H=1 HAVE_STDINT_H=1 HAVE_UNISTD_H=1 SILENT=1 NO_SIGNALS=1 NO_EXECUTE_PERMISSION=1 STDC_HEADERS=1
EXTERN.MONO.LIBGC.win32.SOURCE_DIRS      := sources/libgc/win32
EXTERN.MONO.LIBGC.msvc.COMPILER_CFLAGS   := -wd4018 -wd4244 -wd4996 -wd4005 -wd4116 -wd4101 -wd4013 -wd4113 -wd4146 -wd4172

#Цель - LibMono
EXTERN.MONO.LIBMONO.TYPE                   := dynamic-lib
EXTERN.MONO.LIBMONO.NAME                   := funner.extern.mono.libmono
EXTERN.MONO.LIBMONO.INCLUDE_DIRS           := sources sources/libgc/include sources/mono sources/mono/jit sources/eglib/src sources/eglib
EXTERN.MONO.LIBMONO.SOURCE_DIRS            += sources/mono/utils
EXTERN.MONO.LIBMONO.SOURCE_DIRS            += sources/mono/metadata
EXTERN.MONO.LIBMONO.SOURCE_DIRS            += sources/mono/mini
EXTERN.MONO.LIBMONO.LIB_DIRS               :=
EXTERN.MONO.LIBMONO.LIBS                   += funner.extern.mono.eglib
EXTERN.MONO.LIBMONO.LIBS                   += funner.extern.mono.libgc
EXTERN.MONO.LIBMONO.COMPILER_DEFINES       += GC_NOT_DLL HAVE_CONFIG_H __default_codegen__
EXTERN.MONO.LIBMONO.x86.COMPILER_DEFINES   := TARGET_X86 __i386__ i386
EXTERN.MONO.LIBMONO.win32.COMPILER_DEFINES := UNICODE _UNICODE  WIN32 _WIN32 __WIN32__ _WINDOWS WINDOWS HOST_WIN32 TARGET_WIN32 WINVER=0x0500 _WIN32_WINNT=0x0500 _WIN32_IE=0x0501 WIN32_THREADS FD_SETSIZE=1024
EXTERN.MONO.LIBMONO.win32.COMPILER_DEFINES +=
EXTERN.MONO.LIBMONO.win32.LIBS             := version ws2_32 psapi winmm mswsock user32 kernel32 ole32 advapi32 oleaut32 shell32
EXTERN.MONO.LIBMONO.msvc.COMPILER_CFLAGS   := -wd4018 -wd4244 -wd4996 -wd4005 -wd4101 -wd4133 -wd4146 -wd4047 -wd4024 -wd4098 -wd4013 -wd4716 -wd4715

#Цель - MONO_CS_INVOKE
EXTERN.MONO_CS_INVOKE.TYPE              := cs-dynamic-lib
EXTERN.MONO_CS_INVOKE.NAME              := funner.extern.mono-test-invoke
EXTERN.MONO_CS_INVOKE.SOURCE_DIRS       := tests/invoke_assembly

#Цель - MONO_TESTS
EXTERN.MONO_TESTS.TYPE              := test-suite
EXTERN.MONO_TESTS.SOURCE_DIRS       := tests
EXTERN.MONO_TESTS.USED_APPLICATIONS := mono
EXTERN.MONO_TESTS.INCLUDE_DIRS      := sources
EXTERN.MONO_TESTS.LIBS              := funner.extern.mono
