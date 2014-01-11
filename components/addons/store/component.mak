###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STORE_COMPONENTS

#Цель №1 - пакет
STORE_COMPONENTS.TYPE               := package
STORE_COMPONENTS.COMPONENTS         := core
STORE_COMPONENTS.android.COMPONENTS := android_store amazon_store
STORE_COMPONENTS.iphone.COMPONENTS  := ios_store

