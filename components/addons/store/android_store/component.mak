###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := STORE.ANDROID_STORE.SOURCES STORE.ANDROID_STORE.JAVA

#Target - sources
STORE.ANDROID_STORE.SOURCES.TYPE        := static-lib
STORE.ANDROID_STORE.SOURCES.NAME        := funner.store.android_store
STORE.ANDROID_STORE.SOURCES.SOURCE_DIRS := sources
STORE.ANDROID_STORE.SOURCES.IMPORTS			:= compile.store.core compile.system

#Target - java
STORE.ANDROID_STORE.JAVA.TYPE        := android-jar
STORE.ANDROID_STORE.JAVA.NAME        := funner.store
STORE.ANDROID_STORE.JAVA.SOURCE_DIRS := java_sources
STORE.ANDROID_STORE.JAVA.JARS        := funner.application
STORE.ANDROID_STORE.JAVA.IMPORTS     := compile.extern.google_play_billing
