###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.SHINYLUA

#Цель №1 - SHINYLUA
EXTERN.SHINYLUA.TYPE         := static-lib
EXTERN.SHINYLUA.NAME         := funner.extern.shinylua
EXTERN.SHINYLUA.INCLUDE_DIRS := include
EXTERN.SHINYLUA.SOURCE_DIRS  := sources
EXTERN.SHINYLUA.IMPORTS      := compile.extern.lua compile.extern.shiny
