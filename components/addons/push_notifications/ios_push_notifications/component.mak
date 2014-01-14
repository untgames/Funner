###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PUSH_NOTIFICATIONS.IOS_PUSH_NOTIFICATIONS.SOURCES

#Цель - sources
PUSH_NOTIFICATIONS.IOS_PUSH_NOTIFICATIONS.SOURCES.TYPE        := static-lib
PUSH_NOTIFICATIONS.IOS_PUSH_NOTIFICATIONS.SOURCES.NAME        := funner.push_notifications.ios_push_notifications
PUSH_NOTIFICATIONS.IOS_PUSH_NOTIFICATIONS.SOURCES.SOURCE_DIRS := sources
PUSH_NOTIFICATIONS.IOS_PUSH_NOTIFICATIONS.SOURCES.IMPORTS			:= compile.push_notifications.core compile.system
