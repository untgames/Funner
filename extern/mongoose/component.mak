TARGETS := EXTERN.MONGOOSE

EXTERN.MONGOOSE.TYPE                 := static-lib
EXTERN.MONGOOSE.NAME                 := funner.extern.mongoose
EXTERN.MONGOOSE.INCLUDE_DIRS         := include
EXTERN.MONGOOSE.SOURCE_DIRS          := sources
#EXTERN.MONGOOSE.COMPILER_DEFINES    := DEBUG
EXTERN.MONGOOSE.g++.COMPILER_CFLAGS  := --no-warn
EXTERN.MONGOOSE.wp8.COMPILER_CFLAGS  := -FI winsock2.h
EXTERN.MONGOOSE.wp8.COMPILER_DEFINES += NO_SSL
EXTERN.MONGOOSE.wp8.COMPILER_DEFINES += NO_SSL_DL
EXTERN.MONGOOSE.wp8.COMPILER_DEFINES += NO_CGI
