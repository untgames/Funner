# Dynamic library compile exports
export.compile.extern.zzip.INCLUDE_DIRS                   := include
export.compile.extern.zzip.macosx.COMPILER_DEFINES        := ZZIP_1_H
export.compile.extern.zzip.linux.COMPILER_DEFINES         := ZZIP_1_H
export.compile.extern.zzip.iphone.COMPILER_DEFINES        := ZZIP_1_H
export.compile.extern.zzip.mingw.COMPILER_DEFINES         := ZZIP_2_H
export.compile.extern.zzip.bada.COMPILER_DEFINES          := ZZIP_3_H
export.compile.extern.zzip.native_client.COMPILER_DEFINES := ZZIP_1_H
export.compile.extern.zzip.tabletos.COMPILER_DEFINES      := ZZIP_1_H
export.compile.extern.zzip.alchemy.COMPILER_DEFINES       := ZZIP_1_H

ifeq (,$(filter no_dll,$(PROFILES)))
export.compile.extern.zzip.COMPILER_DEFINES            := ZZIP_DLL
endif

# Dynamic library link exports
export.link.extern.zzip.LIBS    := funner.extern.zzip
export.link.extern.zzip.IMPORTS := link.extern.zlib

# Run exports
ifeq (,$(filter no_dll,$(PROFILES)))
export.run.extern.zzip.DLLS := funner.extern.zzip
else
export.run.extern.zzip.DLLS :=
endif