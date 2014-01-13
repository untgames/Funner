# Compile exports
export.compile.push_notifications.core.INCLUDE_DIRS := include

# Link exports
export.link.push_notifications.core.LIBS    := funner.push_notifications.core
export.link.push_notifications.core.IMPORTS := link.common
