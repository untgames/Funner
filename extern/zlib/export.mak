# Compile exports
export.compile.extern.zlib.INCLUDE_DIRS            := include
export.compile.extern.zlib.unistd.COMPILER_DEFINES := HAVE_UNISTD_H
export.compile.extern.zlib.bada.COMPILER_DEFINES   := HAVE_UNISTD_H

# Link exports
export.link.extern.zlib.LIBS := funner.extern.zlib
