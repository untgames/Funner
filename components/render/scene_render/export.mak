# Compile exports
export.compile.render.scene_render.INCLUDE_DIRS := include ../../xtl/include

# Link exports
export.link.render.scene_render.LIBS     := funner.render.scene_render
export.link.render.scene_render.IMPORTS  := link.common link.scene_graph.core link.render.manager compile.render.manager compile.scene_graph.core

export.link.render.scene_render.render3d.LIBS          := funner.render.scene_render.render3d
export.link.render.scene_render.render3d.IMPORTS       := link.render.scene_render
export.link.render.scene_render.render3d.LINK_INCLUDES := SceneRender3D
