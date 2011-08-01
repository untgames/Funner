ifeq (,$(filter has_iconv,$(PROFILES)))

# Compile exports
export.compile.extern.iconv.INCLUDE_DIRS := include

# Link exports
export.link.extern.iconv.LIBS := funner.extern.iconv

else

# Link exports
export.link.extern.iconv.LIBS := iconv

endif
