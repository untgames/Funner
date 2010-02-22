#Цели
TARGETS := COPY_LAUNCHER COPY_SHADERS COPY_TEXTURES COPY_MATERIALS COPY_CONFIG

#Переменные сборки
OUT_DIR         := ../../../../dist
TOOLS_DIR       := bin
APPLICATION_DIR := apps/capsule3d

#Конфигурирование целей
COPY_LAUNCHER.TYPE              := copy-files
COPY_LAUNCHER.SOURCE_FILES       = $(TOOLS_DIR)/capsule3d* $(TOOLS_DIR)/funner*
COPY_LAUNCHER.DESTINATION_DIR   :=

COPY_SHADERS.TYPE            := copy-files
COPY_SHADERS.SOURCE_FILES    := media/shaders
COPY_SHADERS.DESTINATION_DIR := data/shaders

COPY_TEXTURES.TYPE            := copy-files
COPY_TEXTURES.SOURCE_FILES    := media/textures
COPY_TEXTURES.DESTINATION_DIR := data/textures

COPY_MATERIALS.TYPE            := copy-files
COPY_MATERIALS.SOURCE_FILES    := media/materials
COPY_MATERIALS.DESTINATION_DIR := data/materials

COPY_CONFIG.TYPE            := copy-files
COPY_CONFIG.SOURCE_FILES    := config.xml
COPY_CONFIG.DESTINATION_DIR :=

define convert_collada
CONVERT_COLLADA_$1.TYPE             := convert-collada
CONVERT_COLLADA_$1.OUT_NAME         := $1
CONVERT_COLLADA_$1.SOURCE_FILE      := media/models/$1/model.dae
CONVERT_COLLADA_$1.INCLUDE_DIRS     := media/models/$1/textures media/textures media/textures/environment
CONVERT_COLLADA_$1.MAX_TEXTURE_SIZE := 1024
CONVERT_COLLADA_$1.EXCLUDE_NODES    := pSphere1
TARGETS                             += CONVERT_COLLADA_$1
endef

COLLADA_MODELS := main_ship filter_35 ship_01a ship_04a ship_10a ship_11a ship_20a ship_21a ship_23a ship_24a ship_34a

#CONVERT_COLLADA_main_ship.EXCLUDE_NODES := pSphere1
#CONVERT_COLLADA_filter_35.EXCLUDE_NODES := pSphere1

$(foreach file,$(COLLADA_MODELS),$(eval $(call convert_collada,$(file))))
