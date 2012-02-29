# Compile exports
export.compile.render.scene_render2d.INCLUDE_DIRS := include/obsolete ../../xtl/include

# Link exports
export.link.render.scene_render2d.LIBS          := funner.render.scene_render2d
export.link.render.scene_render2d.IMPORTS       := link.common link.scene_graph.core link.render.low_level link.render.mid_level link.media.font link.media.image link.media.rfx.obsolete link.media.video
export.link.render.scene_render2d.LINK_INCLUDES := SceneRender2d
