# Dynamic library compile exports
export.compile.extern.zzip.INCLUDE_DIRS                := include
export.compile.extern.zzip.nds.g++.COMPILER_DEFINES    := ZZIP_1_H
export.compile.extern.zzip.macosx.COMPILER_DEFINES     := ZZIP_1_H
export.compile.extern.zzip.iphone.COMPILER_DEFINES     := ZZIP_1_H
export.compile.extern.zzip.mingw.g++.COMPILER_DEFINES  := ZZIP_2_H

ifeq (,$(filter iphone,$(PROFILES)))
export.compile.extern.zzip.COMPILER_DEFINES            := ZZIP_DLL
endif

# Dynamic library link exports
export.link.extern.zzip.LIBS := funner.extern.zzip

# Run exports
export.run.extern.zzip.DLLS := funner.extern.zzip
