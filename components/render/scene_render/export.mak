# Compile exports
export.compile.render.scene_render.INCLUDE_DIRS := include ../../xtl/include

# Link exports
export.link.render.scene_render.LIBS     := funner.render.scene_render
export.link.render.scene_render.IMPORTS  := link.common link.scene_graph.core link.render.mid_level link.media.font link.media.image

# Link exports
export.link.render.scene_render.render2d.LIBS          := funner.render.scene_render.render2d
export.link.render.scene_render.render2d.LINK_INCLUDES := SceneRender2d
export.link.render.scene_render.render2d.IMPORTS       := link.render.scene_render link.media.rfx link.media.video
