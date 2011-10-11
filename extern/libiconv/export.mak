ifeq (,$(filter has_iconv,$(PROFILES)))

# Compile exports
export.compile.extern.iconv.INCLUDE_DIRS := include

# Link exports
export.link.extern.iconv.LIBS := funner.extern.iconv
export.link.extern.iconv.wince.IMPORTS   := link.extern.wcecompat

else

# Compile exports
export.compile.extern.iconv.INCLUDE_DIRS :=

# Link exports
export.link.extern.iconv.LIBS := iconv

endif
