export.compile.extern.mongoose.INCLUDE_DIRS  := include

export.link.extern.mongoose.LIBS       := funner.extern.mongoose
export.link.extern.mongoose.mingw.LIBS := wsock32 msvcr90
export.link.extern.mongoose.linux.LIBS := dl

