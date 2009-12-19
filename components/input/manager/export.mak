# Manager compile exports
export.compile.input.manager.INCLUDE_DIRS := include ../../xtl/include
export.compile.input.manager.IMPORTS      := compile.math.vecmath

# Manager link exports
export.link.input.manager.LIBS    := funner.input.manager
export.link.input.manager.IMPORTS := link.input.low_level

# xkeydet link exports
export.link.input.manager.xkeydet.LINK_INCLUDES := XKeydetLoader XKeydetSaver
export.link.input.manager.xkeydet.IMPORTS       := link.input.manager link.common.xml

# xkeymap link exports
export.link.input.manager.xkeymap.LINK_INCLUDES := XKeymapLoader XKeymapSaver
export.link.input.manager.xkeymap.IMPORTS       := link.input.manager link.common.xml

# xkeyreg link exports
export.link.input.manager.xkeyreg.LINK_INCLUDES := XKeyregLoader XKeyregSaver
export.link.input.manager.xkeyreg.IMPORTS       := link.input.manager link.common.xml
