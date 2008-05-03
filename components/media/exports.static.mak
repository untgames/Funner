###################################################################################################
#Ёкспортируемые настройки компонента
###################################################################################################
export.INCLUDE_DIRS     := include
export.LIBS             := media extern.vorbisfile_static extern.vorbis_static extern.ogg extern.devil extern.ilu media.collada
export.LINK_INCLUDES    := XMeshLoaderComponent XMeshSaverComponent #Убрать!!!
export.IMPORTS          := ../common/exports.static.mak
